#include "s3.hpp"

#include <cstdlib>

#include <map>
#include <string>

#include <boost/algorithm/string.hpp>
#include <skyr/url.hpp>

#include "aggressive_retry_strategy.hpp"
#include "aws.hpp"
namespace runtime::aws::s3 {

auto MakeClient() -> Aws::S3::S3Client * {
    aws::EnsureApiInitialized();

    Aws::Client::ClientConfiguration cfg;
    // NOLINTNEXTLINE(readability-magic-numbers)
    cfg.connectTimeoutMs = 3000;
    // NOLINTNEXTLINE(readability-magic-numbers)
    cfg.requestTimeoutMs = 3000;
    // NOLINTNEXTLINE(readability-magic-numbers)
    cfg.retryStrategy = std::make_shared<AggressiveRetryStrategy>(8, 750);

    auto *const endpoint_override = std::getenv("AWS_S3_ENDPOINT");
    if (endpoint_override != nullptr) {
        cfg.endpointOverride = endpoint_override;
    }
    const auto endpoint_url = skyr::make_url(cfg.endpointOverride);
    if (endpoint_url && endpoint_url->protocol() != "https:") {
        cfg.scheme = Aws::Http::Scheme::HTTP;
        cfg.verifySSL = false;
    }

    auto *const region = std::getenv("AWS_DEFAULT_REGION");
    if (region != nullptr) {
        cfg.region = region;
    }

    auto *const url = std::getenv("HTTP_PROXY");
    if (url != nullptr) {
        std::set<std::string> exceptions;
        auto *const no_proxy = std::getenv("NO_PROXY");
        bool is_exception = false;
        if (no_proxy != nullptr && endpoint_url) {
            boost::split(exceptions, no_proxy, boost::is_any_of(","));
            is_exception = exceptions.count(endpoint_url->hostname()) > 0;
        }

        const auto proxy_url = skyr::make_url(url);
        if (proxy_url && !is_exception) {
            cfg.proxyScheme = Aws::Http::Scheme::HTTP;
            if (proxy_url->protocol() == "https:") {
                cfg.proxyScheme = Aws::Http::Scheme::HTTPS;
            }
            cfg.proxyHost = proxy_url->hostname();
            cfg.proxyPort = proxy_url->port<uint32_t>();
        }
    }

    return new Aws::S3::S3Client(
            cfg, Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never,
            false);
}

auto LookupErrorString(const Aws::S3::S3Errors &code) -> std::string {
    using S3Errors = Aws::S3::S3Errors;
    const static std::map<Aws::S3::S3Errors, std::string> strings = {
            {S3Errors::INCOMPLETE_SIGNATURE, "INCOMPLETE_SIGNATURE"},
            {S3Errors::INTERNAL_FAILURE, "INTERNAL_FAILURE"},
            {S3Errors::INVALID_ACTION, "INVALID_ACTION"},
            {S3Errors::INVALID_CLIENT_TOKEN_ID, "INVALID_CLIENT_TOKEN_ID"},
            {S3Errors::INVALID_PARAMETER_COMBINATION,
             "INVALID_PARAMETER_COMBINATION"},
            {S3Errors::INVALID_QUERY_PARAMETER, "INVALID_QUERY_PARAMETER"},
            {S3Errors::INVALID_PARAMETER_VALUE, "INVALID_PARAMETER_VALUE"},
            {S3Errors::MISSING_ACTION, "MISSING_ACTION"},
            {S3Errors::MISSING_AUTHENTICATION_TOKEN,
             "MISSING_AUTHENTICATION_TOKEN"},
            {S3Errors::MISSING_PARAMETER, "MISSING_PARAMETER"},
            {S3Errors::OPT_IN_REQUIRED, "OPT_IN_REQUIRED"},
            {S3Errors::REQUEST_EXPIRED, "REQUEST_EXPIRED"},
            {S3Errors::SERVICE_UNAVAILABLE, "SERVICE_UNAVAILABLE"},
            {S3Errors::THROTTLING, "THROTTLING"},
            {S3Errors::VALIDATION, "VALIDATION"},
            {S3Errors::ACCESS_DENIED, "ACCESS_DENIED"},
            {S3Errors::RESOURCE_NOT_FOUND, "RESOURCE_NOT_FOUND"},
            {S3Errors::UNRECOGNIZED_CLIENT, "UNRECOGNIZED_CLIENT"},
            {S3Errors::MALFORMED_QUERY_STRING, "MALFORMED_QUERY_STRING"},
            {S3Errors::SLOW_DOWN, "SLOW_DOWN"},
            {S3Errors::REQUEST_TIME_TOO_SKEWED, "REQUEST_TIME_TOO_SKEWED"},
            {S3Errors::INVALID_SIGNATURE, "INVALID_SIGNATURE"},
            {S3Errors::SIGNATURE_DOES_NOT_MATCH, "SIGNATURE_DOES_NOT_MATCH"},
            {S3Errors::INVALID_ACCESS_KEY_ID, "INVALID_ACCESS_KEY_ID"},
            {S3Errors::REQUEST_TIMEOUT, "REQUEST_TIMEOUT"},
            {S3Errors::NETWORK_CONNECTION, "NETWORK_CONNECTION"},
            {S3Errors::UNKNOWN, "UNKNOWN"},
            {S3Errors::BUCKET_ALREADY_EXISTS, "BUCKET_ALREADY_EXISTS"},
            {S3Errors::BUCKET_ALREADY_OWNED_BY_YOU,
             "BUCKET_ALREADY_OWNED_BY_YOU"},
            {S3Errors::NO_SUCH_BUCKET, "NO_SUCH_BUCKET"},
            {S3Errors::NO_SUCH_KEY, "NO_SUCH_KEY"},
            {S3Errors::NO_SUCH_UPLOAD, "NO_SUCH_UPLOAD"},
            {S3Errors::OBJECT_ALREADY_IN_ACTIVE_TIER,
             "OBJECT_ALREADY_IN_ACTIVE_TIER"},
            {S3Errors::OBJECT_NOT_IN_ACTIVE_TIER, "OBJECT_NOT_IN_ACTIVE_TIER"}};
    auto const it = strings.find(code);
    if (it == strings.end()) return "unknown error";
    return it->second;
}

}  // namespace runtime::aws::s3
