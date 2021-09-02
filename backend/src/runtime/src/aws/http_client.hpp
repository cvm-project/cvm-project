#ifndef AWS_HTTP_CLIENT_HPP
#define AWS_HTTP_CLIENT_HPP

#include <memory>

#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/http/HttpClient.h>
#include <aws/core/http/HttpClientFactory.h>
#include <aws/core/http/curl/CurlHttpClient.h>

namespace runtime::aws {

class HttpClientFactory : public Aws::Http::HttpClientFactory {
    [[nodiscard]] auto CreateHttpClient(
            const Aws::Client::ClientConfiguration& clientConfiguration) const
            -> std::shared_ptr<Aws::Http::HttpClient> override;

    [[nodiscard]] auto CreateHttpRequest(
            const Aws::String& uri, Aws::Http::HttpMethod method,
            const Aws::IOStreamFactory& streamFactory) const
            -> std::shared_ptr<Aws::Http::HttpRequest> override;

    [[nodiscard]] auto CreateHttpRequest(
            const Aws::Http::URI& uri, Aws::Http::HttpMethod method,
            const Aws::IOStreamFactory& streamFactory) const
            -> std::shared_ptr<Aws::Http::HttpRequest> override;
};

}  // namespace runtime::aws

#endif  // AWS_HTTP_CLIENT_HPP
