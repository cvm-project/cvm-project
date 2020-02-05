#include "aggressive_retry_strategy.hpp"

#include <algorithm>

#include <aws/core/client/AWSError.h>

using Aws::Client::AWSError;
using Aws::Client::CoreErrors;

namespace runtime::aws {

auto AggressiveRetryStrategy::ShouldRetry(const AWSError<CoreErrors>& error,
                                          // NOLINTNEXTLINE(google-runtime-int)
                                          const long attempted_retries) const
        -> bool {
    if (attempted_retries >= max_retries_) return false;

    return error.ShouldRetry();
}

auto AggressiveRetryStrategy::CalculateDelayBeforeNextRetry(
        const AWSError<CoreErrors>& /*error*/,
        // NOLINTNEXTLINE(google-runtime-int)
        const long attempted_retries) const -> long {
    if (attempted_retries == 0) {
        return 0;
    }

    return std::min((1UL << static_cast<unsigned int>(attempted_retries)) *
                            scale_factor_,
                    max_delay_);
}

}  // namespace runtime::aws
