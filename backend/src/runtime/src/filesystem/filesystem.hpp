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

    virtual auto OpenForRead(const std::string &path)
            -> std::shared_ptr<::arrow::io::RandomAccessFile> = 0;
    virtual auto OpenForWrite(const std::string &path)
            -> std::shared_ptr<::arrow::io::OutputStream> = 0;
};

auto MakeFilesystem(const std::string &name) -> std::unique_ptr<FileSystem>;

}  // namespace filesystem
}  // namespace runtime

#endif  // FILESYSTEM_FILESYSTEM_HPP
