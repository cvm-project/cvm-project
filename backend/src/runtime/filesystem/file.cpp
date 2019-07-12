#include "file.hpp"

#include <arrow/buffer.h>
#include <arrow/io/file.h>

namespace runtime {
namespace filesystem {

std::shared_ptr<::arrow::io::RandomAccessFile> LocalFileSystem::OpenForRead(
        const std::string &path) {
    std::shared_ptr<::arrow::io::ReadableFile> handle;
    const auto status = ::arrow::io::ReadableFile::Open(
            path, arrow::default_memory_pool(), &handle);
    if (!status.ok()) {
        throw std::runtime_error("File error " + status.CodeAsString() + ": " +
                                 status.message());
    }
    return handle;
}

}  // namespace filesystem
}  // namespace runtime
