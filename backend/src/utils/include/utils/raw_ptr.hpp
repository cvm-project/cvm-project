#ifndef UTILS_RAW_PTR_HPP
#define UTILS_RAW_PTR_HPP

template <typename Pointee>
class raw_ptr {
public:
    explicit raw_ptr(Pointee *const ptr) : ptr_(ptr) {}

    raw_ptr() = default;
    raw_ptr(const raw_ptr &other) = default;
    raw_ptr(raw_ptr &&other) noexcept = default;
    auto operator=(const raw_ptr &other) -> raw_ptr & = default;
    // cppcheck-suppress operatorEq  // false positive
    auto operator=(raw_ptr &&other) noexcept -> raw_ptr & = default;
    ~raw_ptr() = default;

    auto operator-> () const -> Pointee * { return ptr_; }
    auto operator*() const -> Pointee & { return *ptr_; }
    [[nodiscard]] auto get() const -> Pointee * { return ptr_; }

private:
    Pointee *ptr_;
};

template <typename Pointee>
static auto make_raw(Pointee *ptr) -> raw_ptr<Pointee> {
    return raw_ptr<Pointee>(ptr);
}

#endif  // UTILS_RAW_PTR_HPP
