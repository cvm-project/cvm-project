#ifndef UTILS_RAW_PTR_H
#define UTILS_RAW_PTR_H

template <typename Pointee>
class raw_ptr {
public:
    explicit raw_ptr(Pointee *const ptr) : ptr_(ptr) {}

    raw_ptr() = default;
    raw_ptr(const raw_ptr &other) = default;
    raw_ptr(raw_ptr &&other) noexcept = default;
    raw_ptr &operator=(const raw_ptr &other) = default;
    raw_ptr &operator=(raw_ptr &&other) noexcept = default;
    ~raw_ptr() = default;

    Pointee *operator->() const { return ptr_; }
    Pointee &operator*() const { return *ptr_; }
    Pointee *get() const { return ptr_; }

private:
    Pointee *ptr_;
};

template <typename Pointee>
static raw_ptr<Pointee> make_raw(Pointee *ptr) {
    return raw_ptr<Pointee>(ptr);
}

#endif  // UTILS_RAW_PTR_H
