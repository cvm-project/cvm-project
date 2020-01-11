#include "utils/visitor.hpp"

#include <memory>

#include <boost/mpl/list.hpp>
#include <gtest/gtest.h>

// Sample type hierarchy (non-DefaultConstructable)
struct Visitable {
    explicit Visitable(int /*unused*/) {}
    virtual ~Visitable() = default;
    virtual auto f() -> int = 0;
    virtual auto g() const -> int = 0;
};
struct A : public Visitable {
    explicit A(int i) : Visitable(i) {}
    auto f() -> int override { return g(); }
    auto g() const -> int override { return 1; }
};
struct B : public Visitable {
    explicit B(int i) : Visitable(i) {}
    auto f() -> int override { return g(); }
    auto g() const -> int override { return 2; }
};
struct C : public Visitable {
    explicit C(int i) : Visitable(i) {}
    auto f() -> int override { return g(); }
    auto g() const -> int override { return 3; }
};

// Overloaded function with observable side effect
size_t count_a, count_b, count_c, count_other, count_const, count_non_const;

void ResetCounters() {
    count_a = count_b = count_c = count_other = 0;
    count_const = count_non_const = 0;
}

void count(const Visitable * /*unused*/) { count_other++; }
void count(const A * /*unused*/) { count_a++; }
void count(const B * /*unused*/) { count_b++; }
void count(const C * /*unused*/) { count_c++; }

// Example visitor
using SampleTypes = boost::mpl::list<A, B>;

struct NonConstVisitableVoidReturningVisitor
    : public Visitor<NonConstVisitableVoidReturningVisitor, Visitable,
                     SampleTypes> {
    template <typename T>
    void operator()(T *t) {
        count_non_const++;
        count(t);
        t->f();
        t->g();
    }
    template <typename T>
    void operator()(T *t) const {
        count_const++;
        count(t);
        t->f();
        t->g();
    }
};

struct ConstVisitableVoidReturningVisitor
    : public Visitor<ConstVisitableVoidReturningVisitor, const Visitable,
                     SampleTypes> {
    template <typename T>
    void operator()(T *t) {
        count_non_const++;
        count(t);
        // t->f();  // Not allowed: T is const
        t->g();
    }
    template <typename T>
    void operator()(T *t) const {
        count_const++;
        count(t);
        // t->f();  // Not allowed: T is const
        t->g();
    }
};

struct NonConstVisitableIntReturningVisitor
    : public Visitor<NonConstVisitableIntReturningVisitor, Visitable,
                     SampleTypes, int> {
    template <typename T>
    auto operator()(T *t) -> int {
        count_non_const++;
        count(t);
        t->f();
        return t->g();
    }
    template <typename T>
    auto operator()(T *t) const -> int {
        count_const++;
        count(t);
        t->f();
        return t->g();
    }
};

struct ConstVisitableIntReturningVisitor
    : public Visitor<ConstVisitableIntReturningVisitor, const Visitable,
                     SampleTypes, int> {
    template <typename T>
    auto operator()(T *t) -> int {
        count_non_const++;
        count(t);
        // t->f();  // Not allowed: T is const
        return t->g();
    }
    template <typename T>
    auto operator()(T *t) const -> int {
        count_const++;
        count(t);
        // t->f();  // Not allowed: T is const
        return t->g();
    }
};

struct NoCatchAllVisitor
    : public Visitor<NoCatchAllVisitor, Visitable, SampleTypes> {
    void operator()(A *t) { count(t); }
    void operator()(B *t) { count(t); }
    void operator()(C *t) { count(t); }
};

struct MoveOnlyReturnTypeVisitor
    : public Visitor<MoveOnlyReturnTypeVisitor, Visitable, SampleTypes,
                     std::unique_ptr<int>> {
    auto operator()(A * /*t*/) -> std::unique_ptr<int> { return {}; }
    auto operator()(B * /*t*/) -> std::unique_ptr<int> { return {}; }
    auto operator()(C * /*t*/) -> std::unique_ptr<int> { return {}; }
};

// cppcheck-suppress missingOverride
TEST(VisitorTest, NonConstVisitableVoidReturning) {  // NOLINT
    NonConstVisitableVoidReturningVisitor v;
    std::unique_ptr<Visitable> visitable_ptr;

    ResetCounters();
    visitable_ptr = std::make_unique<A>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 1);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);

    ResetCounters();
    visitable_ptr = std::make_unique<B>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 1);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);

    ResetCounters();
    visitable_ptr = std::make_unique<C>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 1);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);
}

// cppcheck-suppress missingOverride
TEST(VisitorTest, NonConstVisitorConstVisitableVoidReturning) {  // NOLINT
    ConstVisitableVoidReturningVisitor v;
    std::unique_ptr<const Visitable> visitable_ptr;

    ResetCounters();
    visitable_ptr = std::make_unique<A>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 1);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);

    ResetCounters();
    visitable_ptr = std::make_unique<B>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 1);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);

    ResetCounters();
    visitable_ptr = std::make_unique<C>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 1);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);
}

// cppcheck-suppress missingOverride
TEST(VisitorTest, ConstVisitorConstVisitableVoidReturning) {  // NOLINT
    const ConstVisitableVoidReturningVisitor v;
    std::unique_ptr<const Visitable> visitable_ptr;

    ResetCounters();
    visitable_ptr = std::make_unique<A>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 1);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
    EXPECT_EQ(count_const, 1);
    EXPECT_EQ(count_non_const, 0);

    ResetCounters();
    visitable_ptr = std::make_unique<B>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 1);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
    EXPECT_EQ(count_const, 1);
    EXPECT_EQ(count_non_const, 0);

    ResetCounters();
    visitable_ptr = std::make_unique<C>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 1);
    EXPECT_EQ(count_const, 1);
    EXPECT_EQ(count_non_const, 0);
}

// cppcheck-suppress missingOverride
TEST(VisitorTest, NonConstVisitableIntReturning) {  // NOLINT
    NonConstVisitableIntReturningVisitor v;
    std::unique_ptr<Visitable> visitable_ptr;
    int i;

    ResetCounters();
    visitable_ptr = std::make_unique<A>(0);
    i = v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 1);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);
    EXPECT_EQ(i, 1);

    ResetCounters();
    visitable_ptr = std::make_unique<B>(0);
    i = v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 1);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);
    EXPECT_EQ(i, 2);

    ResetCounters();
    visitable_ptr = std::make_unique<C>(0);
    i = v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 1);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);
    EXPECT_EQ(i, 3);
}

// cppcheck-suppress missingOverride
TEST(VisitorTest, ConstVisitableIntReturning) {  // NOLINT
    ConstVisitableIntReturningVisitor v;
    std::unique_ptr<const Visitable> visitable_ptr;
    int i;

    ResetCounters();
    visitable_ptr = std::make_unique<A>(0);
    i = v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 1);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);
    EXPECT_EQ(i, 1);

    ResetCounters();
    visitable_ptr = std::make_unique<B>(0);
    i = v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 1);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);
    EXPECT_EQ(i, 2);

    ResetCounters();
    visitable_ptr = std::make_unique<C>(0);
    i = v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 1);
    EXPECT_EQ(count_const, 0);
    EXPECT_EQ(count_non_const, 1);
    EXPECT_EQ(i, 3);
}

// cppcheck-suppress missingOverride
TEST(VisitorTest, NoCatchAll) {  // NOLINT
    NoCatchAllVisitor v;
    std::unique_ptr<Visitable> visitable_ptr;

    ResetCounters();
    visitable_ptr = std::make_unique<A>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 1);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);

    ResetCounters();
    visitable_ptr = std::make_unique<B>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 1);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);

    ResetCounters();
    visitable_ptr = std::make_unique<C>(0);
    v.Visit(visitable_ptr.get());
    EXPECT_EQ(count_a, 0);
    EXPECT_EQ(count_b, 0);
    EXPECT_EQ(count_c, 0);
    EXPECT_EQ(count_other, 0);
}

// cppcheck-suppress missingOverride
TEST(VisitorTest, MoveOnlyReturnType) {  // NOLINT
    MoveOnlyReturnTypeVisitor v;
    std::unique_ptr<Visitable> visitable_ptr = std::make_unique<A>(0);
    v.Visit(visitable_ptr.get());
}
