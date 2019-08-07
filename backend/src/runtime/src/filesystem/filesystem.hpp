#ifndef FILESYSTEM_FILESYSTEM_HPP
#define FILESYSTEM_FILESYSTEM_HPP

#include <memory>
#include <string>

#include <arrow/io/interfaces.h>

namespace runtime {
namespace filesystem {

class FileSystem {
public:
    virtual ~FileSystem() = default;

    virtual std::shared_ptr<::arrow::io::RandomAccessFile> OpenForRead(
            const std::string &path) = 0;
    virtual std::shared_ptr<::arrow::io::OutputStream> OpenForWrite(
            const std::string &path) = 0;
};

std::unique_ptr<FileSystem> MakeFilesystem(const std::string &name);

}  // namespace filesystem
}  // namespace runtime

#endif  // FILESYSTEM_FILESYSTEM_HPP
