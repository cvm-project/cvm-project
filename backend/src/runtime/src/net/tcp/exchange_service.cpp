#include "exchange_service.hpp"

#include <cstdlib>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/fiber/all.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include "fibers/asio/round_robin.hpp"
#include "fibers/asio/yield.hpp"
#include "fibers/trace.hpp"

using namespace std::literals::chrono_literals;
using runtime::fibers::print;
using runtime::fibers::tag;
using runtime::fibers::this_thread_name;

namespace runtime::net::tcp {

using boost::asio::ip::tcp;

ExchangeService::ExchangeService(
        std::vector<std::pair<std::string, size_t>> hosts, const size_t host_id)
    : hosts_(std::move(hosts)),
      io_service_(new boost::asio::io_service()),
      acceptor_(*io_service_,
                tcp::endpoint(tcp::v4(), hosts_.at(host_id).second)),
      // Initialize sinks
      sinks_([this]() {
          std::vector<std::shared_ptr<SinkChannel>> sinks;
          for (size_t i = 0; i < num_hosts(); i++) {
              sinks.emplace_back(std::make_shared<SinkChannel>(4));
          }
          return sinks;
      }()),
      // Initialize senders
      senders_([this]() {
          print(tag(), ": Setting up senders.");
          std::vector<boost::fibers::fiber> senders;
          for (size_t i = 0; i < num_hosts(); i++) {
              senders.emplace_back([this, i]() { RunSender(i); });
          }
          print(tag(), ": Done setting up senders.");
          return senders;
      }()),
      // Start threads/fibers
      io_thread_([this]() { RunIoThread(); }),
      acceptor_fiber_([this]() { RunAcceptor(); }) {}

ExchangeService::~ExchangeService() {
    print(tag(), ": Closing sinks...");
    for (const auto &sink : sinks_) {
        sink->close();
    }
    print(tag(), ": All sinks closed.");

    print(tag(), ": Waiting for senders to join...");
    for (auto &f : senders_) {
        f.join();
    }
    print(tag(), ": All senders have returned.");

    print(tag(), ": Closing acceptor...");
    acceptor_.close();
    print(tag(), ": Done closing acceptor.");

    print(tag(), ": Waiting for acceptor fiber...");
    acceptor_fiber_.join();
    print(tag(), ": Acceptor fiber returned.");

    print(tag(), ": Waiting for receivers to join...");
    for (auto &f : receivers_) {
        f.join();
    }
    print(tag(), ": All receivers have returned.");

    print(tag(), ": Stopping IO service...");
    io_service_->stop();
    print(tag(), ": IO service stopped.");

    print(tag(), ": Waiting for IO thread...");
    io_thread_.join();
    print(tag(), ": IO thread returned.");
}

void ExchangeService::StartExchange(const size_t exchange_id) {
    std::shared_ptr<Exchange> exchange(
            new Exchange{.source_ = std::make_unique<SourceChannel>(4),
                         .num_remaining_receivers_ = num_hosts()});

    auto const has_emplaced =
            emplace_exchange(exchange_id, std::move(exchange));

    if (has_emplaced) {
        print(tag(), ": [", exchange_id, "] Just started...");
    } else {
        print(tag(), ": [", exchange_id, "] Had already started...");
    }
}

void ExchangeService::FinishExchange(const size_t exchange_id) {
    // cppcheck-suppress assertWithSideEffect  // has no side effect if true
    assert(!ReceiveMessage(exchange_id));

    // XXX: This should wait for all message to be sent

    auto const has_erased = erase_exchange(exchange_id);
    assert(has_erased);

    print(tag(), ": [", exchange_id, "] Finished.");
}

void ExchangeService::SendMessage(const size_t exchange_id,
                                  const size_t receiver_id,
                                  std::optional<std::string> payload) {
    print(tag(), ": [", exchange_id, "] Enqueuing message to ", receiver_id);

    auto const &sink = sinks_.at(receiver_id);
    sink->push({exchange_id, std::move(payload)});
}

auto ExchangeService::ReceiveMessage(const size_t exchange_id)
        -> std::optional<std::string> {
    auto const exchange = this->exchange(exchange_id);
    std::string payload;
    if (exchange->source_->pop(payload) ==
        boost::fibers::channel_op_status::success) {
        print(tag(), ": [", exchange_id, "] Delivered message.");
        return payload;
    }
    print(tag(), ": [", exchange_id, "] Signalling end-of-stream.");
    assert(exchange->num_remaining_receivers_ == 0);
    return {};
}

void ExchangeService::RunIoThread() {
    // Set up IO service and fiber scheduler
    boost::fibers::use_scheduling_algorithm<boost::fibers::asio::round_robin>(
            io_service_);

    print("Thread ", this_thread_name(), ": started");
    print(tag(), ": Running io_service");

    // Run IO service
    io_service_->run();

    // Shut down
    print(tag(), ": io_service returned");
    print("Thread ", this_thread_name(), ": stopping");
}

void ExchangeService::RunReceiver(const std::unique_ptr<tcp::socket> socket) {
    auto const sender_host = socket->remote_endpoint().address().to_string();

    print(tag(), ": Starting to receive messages from ", sender_host, "...");

    while (true) {
        print(tag(), ": Waiting for next message from ", sender_host, "...");

        // Read message header
        MessageHeader header;

        boost::system::error_code ec;
        boost::asio::async_read(*socket,
                                boost::asio::buffer(&header, sizeof(header)),
                                boost::asio::transfer_at_least(sizeof(header)),
                                boost::fibers::asio::yield[ec]);

        // Return when socket closed
        if (ec == boost::asio::error::eof) {
            print(tag(), ": Socket closed by other side (", sender_host,
                  "), returning...");
            break;
        }
        if (ec) {
            throw boost::system::system_error(ec);
        }

        print(tag(), ": [", header.exchange_id_,
              "] Will receive message of size ", header.message_length_);

        auto const exchange_id = header.exchange_id_;
        StartExchange(exchange_id);
        auto const &exchange = this->exchange(exchange_id);
        auto const &source = exchange->source_;

        // Handle end-of-stream
        if (header.op_code_ == MessageHeader::OpCode::kEndStream) {
            print(tag(), ": [", exchange_id,
                  "] Received kEndStream. Refcounting...");
            if (--exchange->num_remaining_receivers_ == 0) {
                print(tag(), ": [", exchange_id,
                      "] Received message from all other sides. "
                      "Closing channel...");
                source->close();
            }
            continue;
        }

        // Read and deliver payload
        std::string payload(header.message_length_, '*');
        boost::asio::async_read(*socket, boost::asio::buffer(payload),
                                boost::asio::transfer_at_least(payload.size()),
                                boost::fibers::asio::yield);

        print(tag(), ": [", exchange_id, "] Received message of size ",
              payload.size(), ".");

        source->push(std::move(payload));
    }
}

void ExchangeService::RunAcceptor() {
    print(tag(), ": Running acceptor...");

    for (size_t i = 0; i < num_hosts(); i++) {
        print(tag(), ": Waiting for next incoming connection...");

        auto socket = std::make_unique<tcp::socket>(*io_service_);
        boost::system::error_code ec;
        tcp::endpoint endpoint;
        acceptor_.async_accept(*socket, endpoint,
                               boost::fibers::asio::yield[ec]);

        if (ec == boost::asio::error::operation_aborted) {
            break;
        }
        if (ec) {
            throw boost::system::system_error(ec);
        }

        print(tag(), ": Incoming connection established with ",
              endpoint.address().to_string(), ", spawning new fiber...");

        receivers_.emplace_back(&ExchangeService::RunReceiver, this,
                                std::move(socket));
    }

    print(tag(), ": Acceptor closed, returning...");
}

void ExchangeService::RunSender(const size_t receiver_id) {
    print(tag(), ": Starting sender for other host ", receiver_id, "...");

    // Resolver endpoint of other side
    auto const [receiver_host, receiver_port] = hosts_.at(receiver_id);
    tcp::resolver resolver(*io_service_);
    tcp::resolver::iterator iterator =
            resolver.async_resolve(receiver_host, std::to_string(receiver_port),
                                   boost::fibers::asio::yield);

    // Retry until connection is established
    auto const socket = std::make_unique<tcp::socket>(*io_service_);
    while (true) {
        print(tag(), ": Establishing connection with ", receiver_host, ":",
              receiver_port, "...");

        boost::system::error_code ec;
        boost::asio::async_connect(*socket, iterator,
                                   boost::fibers::asio::yield[ec]);

        if (!ec) {
            break;
        }
        if (ec != boost::asio::error::connection_refused) {
            throw boost::system::system_error(ec);
        }

        assert(ec == boost::asio::error::connection_refused);
        print(tag(), ": Connection with ", receiver_host, ":", receiver_port,
              " refused, retrying.");
        boost::this_fiber::sleep_for(1s);
    }
    print(tag(), ": Connection with ", receiver_host, ":", receiver_port,
          " established.");

    // Send data from sink channel
    auto const &sink = sinks_.at(receiver_id);

    std::pair<size_t, std::optional<std::string>> message;
    while (boost::fibers::channel_op_status::success == sink->pop(message)) {
        auto const &[exchange_id, payload] = message;

        // Signal end-of-stream
        if (!payload) {
            print(tag(), ": [", exchange_id, "] Done sending data to ",
                  receiver_host, ":", receiver_port, ". Sending kEndStream...");

            MessageHeader header{
                    .op_code_ = MessageHeader::OpCode::kEndStream,  //
                    .exchange_id_ = exchange_id,                    //
                    .message_length_ = 0                            //
            };

            boost::asio::async_write(
                    *socket, boost::asio::buffer(&header, sizeof(header)),
                    boost::fibers::asio::yield);

            print(tag(), ": [", exchange_id, "] kEndStream sent.");

            continue;
        }

        // Send header of payload message
        MessageHeader header{
                .op_code_ = MessageHeader::OpCode::kMessage,  //
                .exchange_id_ = exchange_id,                  //
                .message_length_ = payload->size()            //
        };

        print(tag(), ": Sending message of size ", header.message_length_,
              " to ", receiver_host, ":", receiver_port);

        boost::asio::async_write(*socket,
                                 boost::asio::buffer(&header, sizeof(header)),
                                 boost::fibers::asio::yield);

        // Send payload
        boost::asio::async_write(*socket, boost::asio::buffer(payload.value()),
                                 boost::fibers::asio::yield);

        print(tag(), ": Sent message to ", receiver_host, ":", receiver_port);
    }

    print(tag(), ": Sink closed. Returning...");
}

}  // namespace runtime::net::tcp
