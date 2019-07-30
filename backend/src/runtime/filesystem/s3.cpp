#include "s3.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

#include <arrow/buffer.h>
#include <arrow/io/interfaces.h>
#include <arrow/status.h>

#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

#include <boost/format.hpp>

#include <skyr/url.hpp>

#include "runtime/aws/aws.hpp"
#include "runtime/aws/s3.hpp"

using boost::format;

namespace runtime {
namespace filesystem {

static const char* const kS3FileSystemAllocationTag = "S3FileSystemAllocation";

class S3ReadableFile : public ::arrow::io::RandomAccessFile {
public:
    // cppcheck-suppress passedByValue
    S3ReadableFile(std::shared_ptr<Aws::S3::S3Client> s3_client,
                   // cppcheck-suppress passedByValue
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

    arrow::Status GetSize(int64_t* const size) override {
        if (file_size_ < 0) {
            Aws::S3::Model::HeadObjectRequest request;
            request.WithBucket(bucket_.c_str()).WithKey(key_.c_str());
            request.SetResponseStreamFactory([]() {
                return Aws::New<Aws::StringStream>(kS3FileSystemAllocationTag);
            });

            const auto outcome = s3_client_->HeadObject(request);
            if (!outcome.IsSuccess()) {
                auto const& error = outcome.GetError();
                return arrow::Status::IOError(
                        (format("Error %1% (%2%) when fetching file metadata: "
                                "%3%") %
                         static_cast<int>(error.GetErrorType()) %
                         aws::s3::LookupErrorString(error.GetErrorType()) %
                         error.GetMessage())
                                .str());
            }
            file_size_ = outcome.GetResult().GetContentLength();
        }

        *size = file_size_;
        return arrow::Status::OK();
    }

    bool supports_zero_copy() const override { return false; }

    arrow::Status ReadAt(const int64_t position, const int64_t nbytes,
                         int64_t* const bytes_read,
                         void* const buffer) override {
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

        outcome.GetResult().GetBody().read((reinterpret_cast<char*>(buffer)),
                                           nbytes);
        *bytes_read = outcome.GetResult().GetContentLength();

        return arrow::Status::OK();
    }

    arrow::Status ReadAt(const int64_t position, const int64_t nbytes,
                         std::shared_ptr<arrow::Buffer>* const out) override {
        std::shared_ptr<arrow::ResizableBuffer> buffer;

        ARROW_RETURN_NOT_OK(AllocateResizableBuffer(pool_, nbytes, &buffer));

        int64_t bytes_read = 0;
        ARROW_RETURN_NOT_OK(
                ReadAt(position, nbytes, &bytes_read, buffer->mutable_data()));

        assert(bytes_read == nbytes);
        *out = buffer;
        return arrow::Status::OK();
    }

    /*
     * Implement FileInterface interface
     */

    arrow::Status Close() override { return arrow::Status::OK(); }

    arrow::Status Tell(int64_t* /*position*/) const override {
        return arrow::Status::OK();
    }

    bool closed() const override { return false; }

    /*
     * Implement Readable interface
     */

    arrow::Status Read(const int64_t nbytes, int64_t* const bytes_read,
                       void* const out) override {
        return ReadAt(0, nbytes, bytes_read, out);
    }

    arrow::Status Read(const int64_t nbytes,
                       std::shared_ptr<arrow::Buffer>* const out) override {
        // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
        return ReadAt(0, nbytes, out);
    }

    /*
     * Implement Seekable interface
     */

    arrow::Status Seek(int64_t /*position*/) override {
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
    // cppcheck-suppress passedByValue
    S3OutputStream(std::shared_ptr<Aws::S3::S3Client> s3_client,
                   // cppcheck-suppress passedByValue
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

    arrow::Status Close() override {
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

    arrow::Status Tell(int64_t* const position) const override {
        *position = data_.size();
        return arrow::Status::OK();
    }

    bool closed() const override { return false; }

    /*
     * Implement Writable
     */

    arrow::Status Write(const void* const data, const int64_t nbytes) override {
        auto const ptr = reinterpret_cast<const uint8_t*>(data);
        data_.insert(data_.end(), ptr, ptr + nbytes);
        return arrow::Status::OK();
    }

    arrow::Status Flush() override { return arrow::Status::OK(); }

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

std::shared_ptr<::arrow::io::RandomAccessFile> S3FileSystem::OpenForRead(
        const std::string& s3_path) {
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

std::shared_ptr<::arrow::io::OutputStream> S3FileSystem::OpenForWrite(
        const std::string& path) {
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

}  // namespace filesystem
}  // namespace runtime
