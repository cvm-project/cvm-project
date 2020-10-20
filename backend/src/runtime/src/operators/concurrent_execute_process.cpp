#include "concurrent_execute_process.hpp"

#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/format.hpp>
#include <boost/process.hpp>

#include "runtime/jit/operators/concurrent_execute_process.hpp"
#include "runtime/jit/values/json_parsing.hpp"
#include "runtime/jit/values/none.hpp"
#include "utils/lib_path.hpp"

namespace runtime::operators {

auto process_num_workers() -> size_t {
    static const char *const kNumWorkersVarName = "JITQ_PROCESS_NUM_WORKERS";
    const auto num_workers = std::getenv(kNumWorkersVarName);
    if (num_workers == nullptr) {
        throw std::runtime_error(
                (boost::format("Environment variable %1% not set.") %
                 kNumWorkersVarName)
                        .str());
    }
    return std::stoll(num_workers);
}

auto process_worker_id() -> size_t {
    static const char *const kWorkerIdVarName = "JITQ_PROCESS_WORKER_ID";
    const auto worker_id = std::getenv(kWorkerIdVarName);
    if (worker_id == nullptr) {
        throw std::runtime_error(
                (boost::format("Environment variable %1% not set.") %
                 kWorkerIdVarName)
                        .str());
    }
    return std::stoll(worker_id);
}

auto MakeConcurrentExecuteProcessOperator(
        std::unique_ptr<ValueOperator> &&upstream, std::string inner_plan)
        -> std::unique_ptr<ValueOperator> {
    return std::make_unique<ConcurrentExecuteProcessOperator>(
            std::move(upstream), std::move(inner_plan));
}

void ConcurrentExecuteProcessOperator::open() { upstream_->open(); }

auto ConcurrentExecuteProcessOperator::next()
        -> std::shared_ptr<runtime::values::Value> {
    if (results_.empty()) ComputeResult();
    while (result_it_ != results_.end()) {
        auto const &val_str = (*result_it_++);
        auto const val = values::ConvertFromJsonString(val_str.c_str()).at(0);
        if (dynamic_cast<const values::None *>(val.get()) != nullptr) {
            continue;
        }
        return val;
    }
    return std::make_shared<runtime::values::None>();
}

void ConcurrentExecuteProcessOperator::close() { upstream_->close(); }

struct RunnerResult {
    boost::process::child child;
    std::string input;  // Needs to be kept alive for async IO
    std::future<std::string> out;
    std::future<std::string> err;
    size_t num;
};

void ConcurrentExecuteProcessOperator::ComputeResult() {
    // Find runner executable
    boost::filesystem::path runner = get_lib_path() / "backend/build/runner";

    // Write DAG to file
    auto const dag_path = boost::filesystem::unique_path(
            boost::filesystem::temp_directory_path() / "jitq" /
            "dag-%%%%-%%%%-%%%%-%%%%.json");
    {
        boost::filesystem::create_directories(dag_path.parent_path());

        std::ofstream dag_file(dag_path.string());
        if (!dag_file.is_open()) {
            throw std::runtime_error("Could not open DAG file.");
        }
        dag_file << inner_dag_;
    }

    // Start processes with inner plans
    boost::asio::io_service ios;
    std::vector<RunnerResult> runner_results;

    auto const query_id =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count();
    while (true) {
        // Consume input
        auto const ret = upstream_->next();
        if (dynamic_cast<const runtime::values::None *>(ret.get()) != nullptr) {
            break;
        }
        auto input = values::ConvertToJsonString({ret});

        // Run inner plan
        // Execute runner process
        auto const runner_num = runner_results.size();
        std::future<std::string> out;
        std::future<std::string> err;
        boost::process::async_pipe in(ios);

        // NOLINTNEXTLINE(clang-analyzer-core.NonNullParamChecker)
        boost::process::child child(
                runner, "--dag", dag_path.string(),
                boost::process::env["JITQ_PROCESS_WORKER_ID"] =
                        std::to_string(runner_num),
                boost::process::env["JITQ_QUERY_ID"] = std::to_string(query_id),
                boost::process::env["OMP_NUM_THREADS"] = std::to_string(1),
                boost::process::std_out > out,  //
                boost::process::std_err > err,  //
                boost::process::std_in < in,    //
                ios);

        // Write input into stdin of runner
        boost::asio::async_write(
                in, boost::asio::buffer(input),
                [=](const boost::system::error_code &ec, std::size_t /*size*/) {
                    if (ec) {
                        throw std::system_error(ec);
                    }
                });

        runner_results.push_back(RunnerResult{std::move(child),
                                              std::move(input), std::move(out),
                                              std::move(err), runner_num});
    }

    ios.run();

    // Wait for processes to complete and fetch results
    for (auto &result : runner_results) {
        result.child.wait();
        auto const ret = result.child.exit_code();
        auto const out = result.out.get();
        auto const err = result.err.get();
        auto const runner_num = result.num;

        results_.push_back(out);

        // Print stderr
        std::istringstream err_stream(err);
        for (std::string line; std::getline(err_stream, line);) {
            std::cout << "Runner " << runner_num << ": " << line << std::endl;
        }

        // Check for error
        if (ret != 0) {
            throw std::runtime_error("Runner " + std::to_string(runner_num) +
                                     " failed.");
        }
    }

    result_it_ = results_.begin();
}

}  // namespace runtime::operators
