#include "aws.hpp"

#include <mutex>

#include <aws/core/Aws.h>
#include <aws/core/utils/logging/AWSLogging.h>

#include "aws/stderr_log_system.hpp"

namespace runtime::aws {

struct AwsApiHandle {
    explicit AwsApiHandle(Aws::SDKOptions&& options)
        : options_(std::move(options)) {
        Aws::InitAPI(options_);
    }
    AwsApiHandle(const AwsApiHandle&) = delete;
    AwsApiHandle(AwsApiHandle&&) = delete;
    auto operator=(const AwsApiHandle&) -> AwsApiHandle& = delete;
    auto operator=(AwsApiHandle&&) -> AwsApiHandle& = delete;

    ~AwsApiHandle() { Aws::ShutdownAPI(options_); }

private:
    Aws::SDKOptions options_;
};

void EnsureApiInitialized() {
    static bool is_initialized = false;
    if (is_initialized) return;

    static std::mutex mutex;
    std::lock_guard guard(mutex);

    // cppcheck-suppress identicalConditionAfterEarlyExit
    // cppcheck doesn't seem to know about other threads updating
    // is_initialized.
    if (is_initialized) return;

    Aws::SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Warn;
    options.loggingOptions.logger_create_fn = [=]() {
        return std::shared_ptr<Aws::Utils::Logging::LogSystemInterface>(
                new StderrLogSystem(options.loggingOptions.logLevel));
    };

    static AwsApiHandle handle(std::move(options));

    is_initialized = true;
}

}  // namespace runtime::aws
