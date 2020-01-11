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
    auto OpenForRead(const std::string &path)
            -> std::shared_ptr<::arrow::io::RandomAccessFile> override;
    auto OpenForWrite(const std::string &path)
            -> std::shared_ptr<::arrow::io::OutputStream> override;
};

}  // namespace filesystem
}  // namespace runtime

#endif  // FILESYSTEM_FILE_HPP
