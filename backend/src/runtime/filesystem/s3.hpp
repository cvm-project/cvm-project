#ifndef RUNTIME_FILESYSTEM_S3_HPP
#define RUNTIME_FILESYSTEM_S3_HPP

#include <memory>
#include <string>

#include <arrow/io/interfaces.h>

#include <aws/s3/S3Client.h>

#include "filesystem.hpp"

namespace runtime {
namespace filesystem {

class S3FileSystem : public FileSystem {
public:
    S3FileSystem();

    std::shared_ptr<::arrow::io::RandomAccessFile> OpenForRead(
            const std::string& path) override;

private:
    std::shared_ptr<Aws::S3::S3Client> s3_client_;
};

}  // namespace filesystem
}  // namespace runtime

#endif  // RUNTIME_FILESYSTEM_S3_HPP
