#include "file.hpp"

#include <arrow/buffer.h>
#include <arrow/io/file.h>

#include "operators/arrow_helpers.hpp"

namespace runtime::filesystem {

auto LocalFileSystem::OpenForRead(const std::string &path)
        -> std::shared_ptr<::arrow::io::RandomAccessFile> {
    std::shared_ptr<::arrow::io::ReadableFile> handle;
    ARROW_TRHOW_NOT_OK(::arrow::io::ReadableFile::Open(
            path, arrow::default_memory_pool(), &handle));
    return handle;
}

auto LocalFileSystem::OpenForWrite(const std::string &path)
        -> std::shared_ptr<::arrow::io::OutputStream> {
    std::shared_ptr<::arrow::io::FileOutputStream> handle;
    ARROW_TRHOW_NOT_OK(
            ::arrow::io::FileOutputStream::Open(path, false, &handle));
    return handle;
}

}  // namespace runtime::filesystem
