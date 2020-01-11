#include "filesystem.hpp"

#include <memory>
#include <mutex>

#include "file.hpp"
#include "utils/registry.hpp"

#ifdef AWS_SDK_VERSION_MAJOR
#include "s3.hpp"
#endif

namespace runtime {
namespace filesystem {

struct FilesystemFactory {
    virtual ~FilesystemFactory() = default;
    virtual auto MakeFilesystem() -> std::unique_ptr<FileSystem> = 0;
};

template <typename FileSystemType>
struct DefaultFilesystemFactory : public FilesystemFactory {
    auto MakeFilesystem() -> std::unique_ptr<FileSystem> override {
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

    static std::mutex mutex;
    std::lock_guard guard(mutex);

    // cppcheck-suppress identicalConditionAfterEarlyExit
    // cppcheck doesn't seem to know about other threads updating has_loaded.
    if (has_loaded) return;

    DefaultRegister<LocalFileSystem>::Register("file");
#ifdef AWS_SDK_VERSION_MAJOR
    DefaultRegister<S3FileSystem>::Register("s3");
#endif

    has_loaded = true;
}

auto MakeFilesystem(const std::string &name) -> std::unique_ptr<FileSystem> {
    LoadFilesystemFactories();
    return Registry::at(name)->MakeFilesystem();
}

}  // namespace filesystem
}  // namespace runtime
