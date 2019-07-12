#ifndef RUNTIME_FILESYSTEM_FILESYSTEM_HPP
#define RUNTIME_FILESYSTEM_FILESYSTEM_HPP

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
};

std::unique_ptr<FileSystem> MakeFilesystem(const std::string &name);

}  // namespace filesystem
}  // namespace runtime

#endif  // RUNTIME_FILESYSTEM_FILESYSTEM_HPP
