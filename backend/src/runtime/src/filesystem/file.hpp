#ifndef FILESYSTEM_FILE_HPP
#define FILESYSTEM_FILE_HPP

#include <memory>
#include <string>

#include <arrow/io/interfaces.h>

#include "filesystem.hpp"

namespace runtime {
namespace filesystem {

class LocalFileSystem : public FileSystem {
public:
    std::shared_ptr<::arrow::io::RandomAccessFile> OpenForRead(
            const std::string &path) override;
    std::shared_ptr<::arrow::io::OutputStream> OpenForWrite(
            const std::string &path) override;
};

}  // namespace filesystem
}  // namespace runtime

#endif  // FILESYSTEM_FILE_HPP
