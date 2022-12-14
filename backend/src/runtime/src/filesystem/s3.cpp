#include "s3.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

#include <arrow/buffer.h>
#include <arrow/io/interfaces.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <boost/format.hpp>
#include <skyr/url.hpp>

#include "aws/aws.hpp"
#include "aws/s3.hpp"

using boost::format;

namespace runtime::filesystem {

static const char* const kS3FileSystemAllocationTag = "S3FileSystemAllocation";

class S3ReadableFile : public ::arrow::io::RandomAccessFile {
public:
    S3ReadableFile(std::shared_ptr<Aws::S3::S3Client> s3_client,
                   std::string bucket, std::string key)
        : bucket_(std::move(bucket)),
          key_(std::move(key)),
          s3_client_(std::move(s3_client)),
          file_size_(-1),
          pool_(arrow::default_memory_pool()) {
        if (bucket_.empty()) {
            throw std::runtime_error("Path has empty bucket.");
        }
        if (key_.empty()) {
            throw std::runtime_error("Path has empty key.");
        }
    }

    ~S3ReadableFile() override = default;

    /*
     * Implement RandomAccessFile interface
     */

    auto GetSize() -> arrow::Result<int64_t> override {
        if (file_size_ < 0) {
            Aws::S3::Model::HeadObjectRequest request;
            request.WithBucket(bucket_.c_str()).WithKey(key_.c_str());
            request.SetResponseStreamFactory([]() {
                return Aws::New<Aws::StringStream>(kS3FileSystemAllocationTag);
            });

            const auto outcome = s3_client_->HeadObject(request);
            if (!outcome.IsSuccess()) {
                auto const& error = outcome.GetError();
                auto const error_name =
                        error.GetExceptionName().empty()
                                ? ""
                                : error.GetExceptionName() + " - ";

                return arrow::Status::IOError(
                        (format("Error %1% (%2%%3%) when fetching metadata "
                                "of file s3://%5%%6%: %4%") %
                         static_cast<int>(error.GetErrorType()) %
                         error.GetExceptionName() %
                         aws::s3::LookupErrorString(error.GetErrorType()) %
                         error.GetMessage() % bucket_ % key_)
                                .str());
            }
            file_size_ = outcome.GetResult().GetContentLength();
        }

        return file_size_;
    }

    [[nodiscard]] auto supports_zero_copy() const -> bool override {
        return false;
    }

    auto ReadAt(const int64_t position, const int64_t nbytes, void* const out)
            -> arrow::Result<int64_t> override {
        Aws::S3::Model::GetObjectRequest request;
        request.WithBucket(bucket_.c_str()).WithKey(key_.c_str());
        request.SetRange(
                (format("bytes=%1%-%2%") % position % (position + nbytes - 1))
                        .str());
        request.SetResponseStreamFactory([]() {
            return Aws::New<Aws::StringStream>(kS3FileSystemAllocationTag);
        });

        auto outcome = this->s3_client_->GetObject(request);
        if (!outcome.IsSuccess()) {
            auto const& error = outcome.GetError();
            return arrow::Status::IOError(
                    (format("Error %1% (%2%) when fetching file content: %3%") %
                     static_cast<int>(error.GetErrorType()) %
                     aws::s3::LookupErrorString(error.GetErrorType()) %
                     error.GetMessage())
                            .str());
        }

        outcome.GetResult().GetBody().read((reinterpret_cast<char*>(out)),
                                           nbytes);

        auto const bytes_read = outcome.GetResult().GetContentLength();
        assert(bytes_read == nbytes);
        return bytes_read;
    }

    auto ReadAt(const int64_t position, const int64_t nbytes)
            -> arrow::Result<std::shared_ptr<arrow::Buffer>> override {
        auto maybe_buffer = AllocateResizableBuffer(nbytes, pool_);

        if (!maybe_buffer.ok()) return maybe_buffer.status();
        const std::shared_ptr<arrow::Buffer> buffer =
                std::move(maybe_buffer).ValueOrDie();

        auto const maybe_bytes_read =
                ReadAt(position, nbytes, buffer->mutable_data());
        if (!maybe_bytes_read.ok()) return maybe_bytes_read.status();

        return buffer;
    }

    /*
     * Implement FileInterface interface
     */

    auto Close() -> arrow::Status override { return arrow::Status::OK(); }

    auto Tell() const -> arrow::Result<int64_t> override { return 0; }

    [[nodiscard]] auto closed() const -> bool override { return false; }

    /*
     * Implement Readable interface
     */

    auto Read(const int64_t nbytes, void* const out)
            -> arrow::Result<int64_t> override {
        return ReadAt(0, nbytes, out);
    }

    auto Read(const int64_t nbytes)
            -> arrow::Result<std::shared_ptr<arrow::Buffer>> override {
        // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
        return ReadAt(0, nbytes);
    }

    /*
     * Implement Seekable interface
     */

    auto Seek(int64_t /*position*/) -> arrow::Status override {
        return arrow::Status::OK();
    }

private:
    const std::string bucket_;
    const std::string key_;
    std::shared_ptr<Aws::S3::S3Client> s3_client_;
    int64_t file_size_;
    arrow::MemoryPool* const pool_;
};

class S3OutputStream : public ::arrow::io::OutputStream {
public:
    S3OutputStream(std::shared_ptr<Aws::S3::S3Client> s3_client,
                   std::string bucket, std::string key)
        : bucket_(std::move(bucket)),
          key_(std::move(key)),
          s3_client_(std::move(s3_client)) {
        if (bucket_.empty()) {
            throw std::runtime_error("Path has empty bucket.");
        }
        if (key_.empty()) {
            throw std::runtime_error("Path has empty key.");
        }
    }

    ~S3OutputStream() override = default;

    /*
     * Implement FileInterface
     */

    auto Close() -> arrow::Status override {
        Aws::S3::Model::PutObjectRequest object_request;
        object_request.SetBucket(bucket_);
        object_request.SetKey(key_);

        const std::shared_ptr<Aws::IOStream> input_data(new std::stringstream(
                data_, std::ios_base::in | std::ios_base::binary));
        object_request.SetBody(input_data);

        auto const outcome = s3_client_->PutObject(object_request);
        if (!outcome.IsSuccess()) {
            auto const& error = outcome.GetError();
            return arrow::Status::IOError(
                    (format("Error %1% (%2%) when closing file: %3%") %
                     static_cast<int>(error.GetErrorType()) %
                     aws::s3::LookupErrorString(error.GetErrorType()) %
                     error.GetMessage())
                            .str());
        }
        return arrow::Status::OK();
    }

    auto Tell() const -> arrow::Result<int64_t> override {
        return data_.size();
    }

    [[nodiscard]] auto closed() const -> bool override { return false; }

    /*
     * Implement Writable
     */

    auto Write(const void* const data, const int64_t nbytes)
            -> arrow::Status override {
        const auto* const ptr = reinterpret_cast<const uint8_t*>(data);
        data_.insert(data_.end(), ptr, ptr + nbytes);
        return arrow::Status::OK();
    }

    auto Flush() -> arrow::Status override { return arrow::Status::OK(); }

private:
    const std::string bucket_;
    const std::string key_;
    std::shared_ptr<Aws::S3::S3Client> s3_client_;
    std::string data_;
};

S3FileSystem::S3FileSystem() {
    aws::EnsureApiInitialized();
    s3_client_.reset(aws::s3::MakeClient());
}

auto S3FileSystem::OpenForRead(const std::string& s3_path)
        -> std::shared_ptr<::arrow::io::RandomAccessFile> {
    auto const url = skyr::make_url(s3_path);
    if (!url) {
        std::cerr << "Parsing failed: " << url.error().message() << std::endl;
        assert(url);
    }

    assert(url->protocol() == "s3:");
    auto const bucket = url->hostname();
    auto const key = url->pathname();

    return std::shared_ptr<arrow::io::RandomAccessFile>(
            new S3ReadableFile(s3_client_, bucket, key));
}

auto S3FileSystem::OpenForWrite(const std::string& path)
        -> std::shared_ptr<::arrow::io::OutputStream> {
    auto const url = skyr::make_url(path);
    if (!url) {
        std::cerr << "Parsing failed: " << url.error().message() << std::endl;
        assert(url);
    }

    assert(url->protocol() == "s3:");
    auto const bucket = url->hostname();
    auto const key = url->pathname();

    return std::shared_ptr<arrow::io::OutputStream>(
            new S3OutputStream(s3_client_, bucket, key));
}

}  // namespace runtime::filesystem
