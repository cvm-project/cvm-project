#include "operators/concurrent_execute_lambda.hpp"

#include <cstdlib>

#include <iostream>
#include <memory>
#include <string>

#include <boost/format.hpp>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include "runtime/jit/operators/concurrent_execute_lambda.hpp"
#include "runtime/jit/values/json_parsing.hpp"
#include "runtime/jit/values/none.hpp"

namespace py = pybind11;

namespace runtime::operators {

auto lambda_num_workers() -> size_t {
    static const char* const kNumWorkersVarName = "JITQ_LAMBDA_NUM_WORKERS";
    const auto num_workers_str = std::getenv(kNumWorkersVarName);
    if (num_workers_str == nullptr) {
        throw std::runtime_error(
                (boost::format("Environment variable %1% not set.") %
                 kNumWorkersVarName)
                        .str());
    }
    return std::stoul(num_workers_str);
}

auto lambda_worker_id() -> size_t {
    static const char* const kWorkerIdVarName = "JITQ_LAMBDA_WORKER_ID";
    const auto worker_id_str = std::getenv(kWorkerIdVarName);
    if (worker_id_str == nullptr) {
        throw std::runtime_error(
                (boost::format("Environment variable %1% not set.") %
                 kWorkerIdVarName)
                        .str());
    }
    return std::stoul(worker_id_str);
}

auto MakeConcurrentExecuteLambdaOperator(
        std::unique_ptr<ValueOperator>&& upstream, std::string inner_plan)
        -> std::unique_ptr<ValueOperator> {
    return std::make_unique<ConcurrentExecuteLambdaOperator>(
            std::move(upstream), std::move(inner_plan));
}

void ConcurrentExecuteLambdaOperator::open() { upstream_->open(); }

auto ConcurrentExecuteLambdaOperator::next()
        -> std::shared_ptr<runtime::values::Value> {
    if (results_.empty()) MaterializeResult();
    if (result_it_ != results_.end()) {
        return *result_it_++;
    }
    return std::make_shared<runtime::values::None>();
}

void ConcurrentExecuteLambdaOperator::close() { upstream_->close(); }

void ConcurrentExecuteLambdaOperator::MaterializeResult() {
    // Consume input
    std::vector<std::string> str_input;
    while (true) {
        auto const ret = upstream_->next();
        if (dynamic_cast<const runtime::values::None*>(ret.get()) != nullptr) {
            break;
        }
        str_input.push_back(values::ConvertToJsonString({ret}));
    }

    {
        // Work-around for pybind11 weirdness in shared libraries
        // See https://github.com/pybind/pybind11/issues/1738
        auto tstate = PyGILState_Ensure();
        pybind11::get_shared_data("");
        PyGILState_Release(tstate);
    }

    // Run inner plan
    py::gil_scoped_acquire acquire;
    const auto py_driver_mod = py::module::import("jitq.serverless.driver");
    const auto py_driver = py_driver_mod.attr("Driver")("test-lambda-stack");
    const auto results =
            py_driver.attr("run_query")(inner_dag_, str_input.size(), str_input)
                    .cast<std::vector<std::string>>();

    // Materialize result
    for (auto const& v : results) {
        auto const val = values::ConvertFromJsonString(v.c_str()).at(0);
        if (dynamic_cast<const values::None*>(val.get()) != nullptr) {
            continue;
        }
        results_.push_back(val);
    }
    result_it_ = results_.begin();
}

}  // namespace runtime::operators
