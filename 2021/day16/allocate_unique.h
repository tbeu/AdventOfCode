// See https://stackoverflow.com/a/23132307/8520615

#pragma once

#include <memory>

template <typename Alloc>
struct alloc_deleter
{
    alloc_deleter(const Alloc& a) : a(a)
    {
    }

    using AT = std::allocator_traits<Alloc>;
    using pointer = typename std::allocator_traits<Alloc>::pointer;

    void operator()(pointer p) const
    {
        Alloc aa(a);
        AT::destroy(aa, std::addressof(*p));
        AT::deallocate(aa, p, 1);
    }

private:
    Alloc a;
};

template <typename T, typename Alloc, typename... Args>
auto allocate_unique(const Alloc& alloc, Args&&... args)
{
    using AT = std::allocator_traits<Alloc>;
    static_assert(std::is_same<typename AT::value_type, std::remove_cv_t<T> >{}(),
                  "Allocator has the wrong value_type");

    Alloc a(alloc);
    auto p = AT::allocate(a, 1);
    try {
        AT::construct(a, std::addressof(*p), std::forward<Args>(args)...);
        using D = alloc_deleter<Alloc>;
        return std::unique_ptr<T, D>(p, D(a));
    } catch (...) {
        AT::deallocate(a, p, 1);
        throw;
    }
}
