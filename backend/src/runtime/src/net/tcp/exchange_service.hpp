#ifndef NET_TCP_EXCHANGE_SERVICE_HPP
#define NET_TCP_EXCHANGE_SERVICE_HPP

#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/fiber/buffered_channel.hpp>
#include <boost/fiber/fiber.hpp>
#include <boost/fiber/mutex.hpp>

namespace runtime::net::tcp {

class ExchangeService {
public:
    explicit ExchangeService(std::vector<std::pair<std::string, size_t>> hosts,
                             size_t host_id);
    ExchangeService(const ExchangeService& other) = delete;
    ExchangeService(ExchangeService&& other) noexcept = delete;
    auto operator=(const ExchangeService& other) -> ExchangeService& = delete;
    // cppcheck-suppress operatorEq  // false positive
    auto operator=(ExchangeService&& other) noexcept
            -> ExchangeService& = delete;

    ~ExchangeService();

    /*
     * Register a new exchange through its ID.
     */
    void StartExchange(size_t exchange_id);

    /*
     * Finish the exchange with the given ID.
     *
     * The caller must ensure (by calling ReceiveMessage) that all remote hosts
     * have sent all data, i.e., they have sent kEndStream.
     */
    void FinishExchange(size_t exchange_id);

    /*
     * Sends message to the given remote host. Sending an empty optional
     * signals that no more data is going to be sent, i.e., sends kEndStream.
     */
    void SendMessage(size_t exchange_id, size_t receiver_id,
                     std::optional<std::string> payload);

    /*
     * Try to receive message. An empty optional signals that no more messages
     * will arrive, i.e., all other hosts have sent kEndStream.
     */
    auto ReceiveMessage(size_t exchange_id) -> std::optional<std::string>;

    /*
     * Accessors
     */
    auto hosts() -> const std::vector<std::pair<std::string, size_t>>& {
        return hosts_;
    }
    auto num_hosts() const -> size_t { return hosts_.size(); }

private:
    using SinkChannel = boost::fibers::buffered_channel<
            std::pair<size_t, std::optional<std::string>>>;
    using SourceChannel = boost::fibers::buffered_channel<std::string>;

    /*
     * Wire format
     */
    struct MessageHeader {
        enum OpCode : size_t { kEndStream, kMessage };

        OpCode op_code_ = OpCode::kMessage;
        size_t exchange_id_ = 0;
        size_t message_length_ = 0;
    };

    struct Exchange {
        std::unique_ptr<SourceChannel> source_;
        std::atomic<size_t> num_remaining_receivers_;
    };

    /*
     * Runs the boost::asio IO service.
     */
    void RunIoThread();

    /*
     * Handles the connection for receiving data from a remote host.
     *
     * Reads a sequence of message headers + optional payloads until the remote
     * side closes the socket. Demultiplexes messages by the exchange ID sent
     * in the message header. Delivers payloads into the channel correpsonding
     * to the target exchange; closes that channel on kEndStream.
     */
    void RunReceiver(std::unique_ptr<boost::asio::ip::tcp::socket> socket);

    /*
     * Listen on port and accept incoming connections until port is closed.
     * Each incoming connection corresponds to a remote host and is run in a
     * new fiber.
     */
    void RunAcceptor();

    /*
     * Handles the connection for sending data to a remote host.
     *
     * Tries to connect to other side until it succeeds. Multiplexes messages
     * from any exchange by indicate the exchange ID in the message header.
     * Reads payloads from a queue that all exchanges push to. An empty payload
     * in that queue indicates that no more data will be sent for that
     * exchange, i.e., end-of-stream should be signalled to the other side.
     * Returns when the queue is closed, i.e., when the ExchangeService is
     * being shut down.
     */
    void RunSender(size_t receiver_id);

    auto exchange(const size_t exchange_id) -> std::shared_ptr<Exchange> {
        std::lock_guard<boost::fibers::mutex> lock(exchanges_mutex_);
        return exchanges_.at(exchange_id);
    }

    auto emplace_exchange(const size_t exchange_id,
                          std::shared_ptr<Exchange> exchange) -> bool {
        std::lock_guard<boost::fibers::mutex> lock(exchanges_mutex_);
        return exchanges_.emplace(exchange_id, std::move(exchange)).second;
    }

    auto erase_exchange(const size_t exchange_id) -> bool {
        std::lock_guard<boost::fibers::mutex> lock(exchanges_mutex_);
        return exchanges_.erase(exchange_id) == 1;
    }

    const std::vector<std::pair<std::string, size_t>> hosts_;
    std::shared_ptr<boost::asio::io_service> io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;

    // One per host
    const std::vector<std::shared_ptr<SinkChannel>> sinks_;
    std::vector<boost::fibers::fiber> senders_{};
    std::vector<boost::fibers::fiber> receivers_{};

    // One per exchange
    std::unordered_map<size_t, std::shared_ptr<Exchange>> exchanges_{};
    boost::fibers::mutex exchanges_mutex_{};

    // These threads/fibers are started in the constructor and need the
    // previous class members --> they need to come last.
    std::thread io_thread_;
    boost::fibers::fiber acceptor_fiber_;
};

}  // namespace runtime::net::tcp

#endif  // NET_TCP_EXCHANGE_SERVICE_HPP
