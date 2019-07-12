#include "filesystem.hpp"

#include <memory>

#include "file.hpp"
#include "utils/registry.hpp"

#ifdef AWS_SDK_VERSION_MAJOR
#include "s3.hpp"
#endif

namespace runtime {
namespace filesystem {

struct FilesystemFactory {
    virtual ~FilesystemFactory() = default;
    virtual std::unique_ptr<FileSystem> MakeFilesystem() = 0;
};

template <typename FileSystemType>
struct DefaultFilesystemFactory : public FilesystemFactory {
    std::unique_ptr<FileSystem> MakeFilesystem() override {
        return std::make_unique<FileSystemType>();
    }
};

using Registry = utils::Registry<FilesystemFactory>;

template <typename FileSystemType>
struct DefaultRegister {
    void static Register(const std::string &name) {
        Registry::Register(
                name,
                std::make_unique<DefaultFilesystemFactory<FileSystemType>>());
    }
};

void LoadFilesystemFactories() {
    static bool has_loaded = false;
    if (has_loaded) return;

    DefaultRegister<LocalFileSystem>::Register("file");
#ifdef AWS_SDK_VERSION_MAJOR
    DefaultRegister<S3FileSystem>::Register("s3");
#endif

    has_loaded = true;
}

std::unique_ptr<FileSystem> MakeFilesystem(const std::string &name) {
    LoadFilesystemFactories();
    return Registry::at(name)->MakeFilesystem();
}

}  // namespace filesystem
}  // namespace runtime
