#include "file.hpp"

#include <arrow/buffer.h>
#include <arrow/io/file.h>

#include "operators/arrow_helpers.hpp"

namespace runtime::filesystem {

auto LocalFileSystem::OpenForRead(const std::string &path)
        -> std::shared_ptr<::arrow::io::RandomAccessFile> {
    auto const result = ::arrow::io::ReadableFile::Open(path);
    operators::ThrowIfNotOK(result);
    return result.ValueOrDie();
}

auto LocalFileSystem::OpenForWrite(const std::string &path)
        -> std::shared_ptr<::arrow::io::OutputStream> {
    auto const result = ::arrow::io::FileOutputStream::Open(path);
    operators::ThrowIfNotOK(result);
    return result.ValueOrDie();
}

}  // namespace runtime::filesystem
