#ifndef UTILS_VISITOR_HPP
#define UTILS_VISITOR_HPP

#include <type_traits>
#include <typeinfo>  // Boost v1.58 (and potentially others) do not include
                     // the STL code for std::bad_cast from the polymorphic
                     // pointer cast even though it needs it. <typeinfo>
                     // provides it.

#include <cassert>

#include <boost/mpl/for_each.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/polymorphic_pointer_cast.hpp>

namespace impl {

template <class>
struct sfinae_true : std::true_type {};

// Helper that allows to test whether a type T is callable with an argument of
// type Arg (see https://stackoverflow.com/a/9154394/651937)
template <class T, class Arg>
static auto HasCallOperator(int32_t /*unused*/)
        -> sfinae_true<decltype(std::declval<T>()(std::declval<Arg>()))>;
template <class, class Arg>
static auto HasCallOperator(int64_t /*unused*/) -> std::false_type;

}  // namespace impl

template <typename Visitor>
class VisitorFunctor {
public:
    explicit VisitorFunctor(Visitor *const visitor) : visitor_(visitor) {}

    template <typename... Args>
    auto operator()(Args... args) {
        visitor_->Visit(args...);
    }

private:
    Visitor *const visitor_;
};

template <typename Visitor>
auto MakeVisitorFunctor(Visitor *const visitor) -> VisitorFunctor<Visitor> {
    return VisitorFunctor<Visitor>(visitor);
}

template <typename ConcreteVisitor, typename VisitableBase,
          typename VisitableTypeList, typename ReturnType = void>
struct Visitor {
    using ThisType = Visitor<ConcreteVisitor, VisitableBase, VisitableTypeList,
                             ReturnType>;
    using VisitablePointerTypeList = typename boost::mpl::transform_view<
            VisitableTypeList,
            std::add_pointer<boost::mpl::placeholders::_>>::type;

    virtual ~Visitor() = default;

    auto Visit(VisitableBase *const visitable) const -> ReturnType {
        return VisitImpl<true>(visitable);
    }
    auto Visit(VisitableBase *const visitable) -> ReturnType {
        return VisitImpl<false>(visitable);
    }

    auto functor() -> VisitorFunctor<ThisType> {
        return MakeVisitorFunctor(this);
    }
    auto functor() const -> VisitorFunctor<const ThisType> {
        return MakeVisitorFunctor(this);
    }

private:
    using ReturnValueType =
            typename std::conditional<std::is_void<ReturnType>::value,  //
                                      void *, ReturnType>::type;

    template <const bool is_visitor_const>
    auto VisitImpl(VisitableBase *const visitable) const -> ReturnType {
        static_assert(
                std::is_base_of<ThisType, ConcreteVisitor>::value,
                "'YourVisitor' must inherit from 'Visitor<YourVisitor, T, U>'");

        using QualifiedConcreteVisitor = typename std::conditional<
                is_visitor_const,  //
                typename std::add_const<ConcreteVisitor>::type,
                typename std::remove_const<ConcreteVisitor>::type>::type;

        // Casting away constness of 'this' is OK: QualifiedConcreteVisitor is
        // const if 'this' was const on the original call to 'Visit', hence,
        // 'concrete_visitor' is const again.
        auto const concrete_visitor =  //
                boost::polymorphic_pointer_downcast<QualifiedConcreteVisitor>(
                        const_cast<typename std::remove_const<ThisType>::type
                                           *>(this));

        bool has_called = false;
        ReturnValueType return_value{};
        Functor<QualifiedConcreteVisitor> visitor_functor{
                concrete_visitor, visitable, &has_called, &return_value};
        boost::mpl::for_each<VisitablePointerTypeList>(visitor_functor);

        if (!has_called) {
            using HasCatchAll =
                    decltype(impl::HasCallOperator<QualifiedConcreteVisitor,
                                                   const VisitableBase *>(0));

            VisitCatchAll(&visitor_functor, visitable, HasCatchAll());
        }

        // NOLINTNEXTLINE  // False positive from clang-tidy
        if constexpr (!std::is_same_v<ReturnType, void>) {
            return return_value;
        }
    }

    template <typename QualifiedConcreteVisitor>
    struct Functor {
        QualifiedConcreteVisitor *visitor;
        VisitableBase *visitable;
        bool *has_called;
        ReturnValueType *return_value;

        template <typename T>
        void operator()(T * /*unused*/) {
            using ConstQualifiedT = typename std::conditional<
                    std::is_const<VisitableBase>::value,
                    typename std::add_const<T>::type,
                    typename std::remove_const<T>::type>::type;

            auto t_ptr = dynamic_cast<ConstQualifiedT *>(visitable);
            if (t_ptr) {
                assert(!*has_called);
                *has_called = true;
                DoCall(t_ptr, std::is_void<ReturnType>());
            }
        }

        template <typename T>
        void DoCall(T *t, std::true_type /*unused*/) {
            (*visitor)(t);
        }

        template <typename T>
        void DoCall(T *t, std::false_type /*unused*/) {
            *return_value = (*visitor)(t);
        }
    };

    template <typename QualifiedConcreteVisitor>
    void VisitCatchAll(Functor<QualifiedConcreteVisitor> *functor,
                       VisitableBase *const visitable,
                       std::true_type /*unused*/) const {
        (*functor)(visitable);
    }

    template <typename QualifiedConcreteVisitor>
    void VisitCatchAll(Functor<QualifiedConcreteVisitor> * /*functor*/,
                       VisitableBase *const /*visitable*/,
                       std::false_type /*unused*/) const {}
};

#endif  // UTILS_VISITOR_HPP
