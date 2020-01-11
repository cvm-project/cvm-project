#ifndef FILESYSTEM_S3_HPP
#define FILESYSTEM_S3_HPP

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

    auto OpenForRead(const std::string& path)
            -> std::shared_ptr<::arrow::io::RandomAccessFile> override;
    auto OpenForWrite(const std::string& path)
            -> std::shared_ptr<::arrow::io::OutputStream> override;

private:
    std::shared_ptr<Aws::S3::S3Client> s3_client_;
};

}  // namespace filesystem
}  // namespace runtime

#endif  // FILESYSTEM_S3_HPP
