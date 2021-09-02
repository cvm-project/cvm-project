#include "http_client.hpp"

#include <memory>
#include <mutex>
#include <vector>

#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/http/HttpClient.h>
#include <aws/core/http/HttpClientFactory.h>
#include <aws/core/http/curl/CurlHttpClient.h>
#include <aws/core/http/standard/StandardHttpRequest.h>
#include <curl/curl.h>

using namespace std::chrono_literals;

namespace runtime::aws {

// How often should the progress be traced
static constexpr auto kReportFrequency = 1ms;

// What is the maximim time with zero progress after which requests should be
// aborted
static constexpr auto kMaxIdleDuration = 400ms;

struct RequestInfo {
    std::chrono::steady_clock::time_point last_dlnow_time;
    std::chrono::steady_clock::time_point last_ulnow_time;
    curl_off_t last_dlnow;
    curl_off_t last_ulnow;
};

// Called by CURL "frequently". `clientp` is user-provided; we use the request
// state provided by our HttpClient.
auto progress_callback(void* const clientp, const curl_off_t /*dltotal*/,
                       const curl_off_t dlnow, const curl_off_t /*ultotal*/,
                       const curl_off_t ulnow) -> int {
    auto const now = std::chrono::steady_clock::now();

    auto& request = *reinterpret_cast<RequestInfo*>(clientp);

    // Update if request has made progress
    if (dlnow > request.last_dlnow) {
        request.last_dlnow = dlnow;
        request.last_dlnow_time = now;
    }
    if (ulnow > request.last_ulnow) {
        request.last_ulnow = ulnow;
        request.last_ulnow_time = now;
    }

    // Compute when the last prgress was made and if we should abort
    auto const no_dlnow_since =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - request.last_dlnow_time);
    auto const no_ulnow_since =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - request.last_ulnow_time);
    auto const no_progress_since = std::min(no_dlnow_since, no_ulnow_since);
    auto const do_abort_request = no_progress_since > kMaxIdleDuration;

    // Signal to CURL that request should be aborted
    if (do_abort_request) {
        return 1;
    }

    return 0;
}

class HttpClient : public Aws::Http::CurlHttpClient {
    using Base = Aws::Http::CurlHttpClient;

public:
    explicit HttpClient(const Aws::Client::ClientConfiguration& clientConfig)
        : Base(clientConfig) {}

protected:
    void OverrideOptionsOnConnectionHandle(
            CURL* const connectionHandle) const override {
        // Create state for this request
        auto const now = std::chrono::steady_clock::now();
        {
            std::lock_guard l(requests_mutex_);
            requests_.emplace_back(
                    std::make_unique<RequestInfo>(RequestInfo{now, now, 0, 0}));
        }

        // Set up CURL's progress function
        curl_easy_setopt(connectionHandle, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(connectionHandle, CURLOPT_XFERINFODATA,
                         requests_.back().get());
        curl_easy_setopt(connectionHandle, CURLOPT_XFERINFOFUNCTION,
                         progress_callback);
    }

private:
    // Keep the state of requests such that CURL's progress function can track
    // their progress between calls. Since the client owns them, they are all
    // destroyed when the client is destroyed, at which point all requests have
    // finished.
    mutable std::vector<std::unique_ptr<RequestInfo>> requests_;
    mutable std::mutex requests_mutex_;
};

auto HttpClientFactory::CreateHttpClient(
        const Aws::Client::ClientConfiguration& clientConfiguration) const
        -> std::shared_ptr<Aws::Http::HttpClient> {
    return std::make_shared<HttpClient>(clientConfiguration);
}

// Copied from aws-cpp-sdk-core/source/http/HttpClientFactory.cpp
auto HttpClientFactory::CreateHttpRequest(
        const Aws::String& uri, const Aws::Http::HttpMethod method,
        const Aws::IOStreamFactory& streamFactory) const
        -> std::shared_ptr<Aws::Http::HttpRequest> {
    return CreateHttpRequest(Aws::Http::URI(uri), method, streamFactory);
}

// Copied from aws-cpp-sdk-core/source/http/HttpClientFactory.cpp
auto HttpClientFactory::CreateHttpRequest(
        const Aws::Http::URI& uri, const Aws::Http::HttpMethod method,
        const Aws::IOStreamFactory& streamFactory) const
        -> std::shared_ptr<Aws::Http::HttpRequest> {
    auto request = std::make_shared<Aws::Http::Standard::StandardHttpRequest>(
            uri, method);
    request->SetResponseStreamFactory(streamFactory);

    return request;
}

}  // namespace runtime::aws
