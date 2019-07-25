#ifndef RUNTIME_FILESYSTEM_LOCAL_HPP
#define RUNTIME_FILESYSTEM_LOCAL_HPP

#include "filesystem.hpp"

#include <memory>
#include <string>

#include <arrow/io/interfaces.h>

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

#endif  // RUNTIME_FILESYSTEM_LOCAL_HPP
