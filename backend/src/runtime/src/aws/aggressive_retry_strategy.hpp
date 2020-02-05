#ifndef AWS_AGGRESSIVE_RETRY_STRATEGY_HPP
#define AWS_AGGRESSIVE_RETRY_STRATEGY_HPP

#include <string>

#include <aws/core/client/AWSError.h>
#include <aws/core/client/RetryStrategy.h>

namespace runtime {
namespace aws {

class AggressiveRetryStrategy : public Aws::Client::RetryStrategy {
public:
    explicit AggressiveRetryStrategy(const uint64_t max_retries = 10,
                                     const uint64_t max_delay = 1000,
                                     const uint64_t scale_factor = 25)
        : scale_factor_(scale_factor),
          max_delay_(max_delay),
          max_retries_(max_retries) {}

    [[nodiscard]] auto ShouldRetry(
            const Aws::Client::AWSError<Aws::Client::CoreErrors>& error,
            // NOLINTNEXTLINE(google-runtime-int)
            long attempted_retries) const -> bool override;

    [[nodiscard]] auto CalculateDelayBeforeNextRetry(
            const Aws::Client::AWSError<Aws::Client::CoreErrors>& error,
            // NOLINTNEXTLINE(google-runtime-int)
            long attempted_retries) const -> long override;

private:
    const uint64_t scale_factor_;
    const uint64_t max_delay_;
    const uint64_t max_retries_;
};

}  // namespace aws
}  // namespace runtime

#endif  // AWS_AGGRESSIVE_RETRY_STRATEGY_HPP
