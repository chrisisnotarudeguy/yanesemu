#ifndef HEAPARRAY_HPP_INCLUDED
#define HEAPARRAY_HPP_INCLUDED

#include <cstddef>
#include <memory>
#include <iterator>

/* The C++ standard defines 2 array classes: std::array<T, N>, for arrays
 * created on the stack, and std::vector<T>, for dynamic array. If the size of
 * the array isn't known at compile-time, one must use std::vector<T>. However,
 * there are a few cases where one would like to use a fixed size array only
 * known at run-time, and there are a few cases where using such fixed size
 * array would be faster. This class is an attempt at that.
 * Note that this class looks and acts like an actual array. That means it's
 * still undefined behavior to read uninitialized elements of the array (one
 * can quickly initialize the elements by calling clear() or fill()).
 */
template <typename T>
class HeapArray {
    std::size_t len;
    std::unique_ptr<T[]> arrptr;

public:
    explicit HeapArray(std::size_t l)
        : len(l), arrptr(new T[len])
    { }
    HeapArray(std::initializer_list<T> lst)
        : HeapArray(lst.size())
    {
        std::size_t i = 0;
        for (const auto &x : lst)
            arrptr[i++] = x;
    }
    HeapArray(const HeapArray &oth) { operator=(oth); }

    inline T & operator[](std::size_t i) const { return arrptr[i]; }
    inline HeapArray<T> & operator=(const HeapArray<T> &oth)
    {
        std::size_t size = oth.size();
        reset(size);
        for (std::size_t i = 0; i != size; i++)
            arrptr[i] = std::move(oth[i]);
        return *this;
    }
    inline bool operator==(const HeapArray &arr)
    {
        for (std::size_t i = 0; i != arr.size(); i++)
            if (arrptr[i] != arr[i])
                return false;
        return true;
    }

    inline std::size_t size() const { return len; }

    inline bool empty() const { return len == 0; }

    inline void clear() { std::memset(arrptr, 0, len); }

    inline void fill(T elem)
    {
        for (std::size_t i = 0; i < len; i++)
            arrptr[i] = elem;
    }

    inline void reset(std::size_t newlen)
    {
        len = newlen;
        arrptr.reset(new T[len]);
    }

    // basic iterator support
    using value_type = T;
    using reference  = T &;
    using const_reference = const T &;
    using pointer    = T *;
    using difference_type = std::ptrdiff_t;
    using size_type  = std::size_t;

    class iterator {
        HeapArray *arr;
        std::size_t index = 0;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using reference  = T &;
        using pointer    = T *;
        using difference_type = std::ptrdiff_t;

        iterator(HeapArray *a) : arr(a) { }
        iterator(HeapArray *a, std::size_t i) : arr(a), index(i) { }
        iterator(const iterator &i) : arr(i.arr), index(i.index) { }
        iterator &  operator= (const iterator &i)       { arr = i.arr; index = i.index; return *this; }
        iterator &  operator++()                        { index++; return *this; }
        iterator &  operator--()                        { index--; return *this; }
        iterator    operator++(int)                     { iterator res(*this, index); ++(*this); return res; }
        iterator    operator--(int)                     { iterator res(*this, index); --(*this); return res; }
        iterator &  operator+=(std::size_t n)           { index += n; return *this; }
        iterator &  operator-=(std::size_t n)           { index -= n; return *this; }
        iterator    operator+ (std::size_t n) const     { return iterator(arr, index+n); }
        iterator    operator- (std::size_t n) const     { return iterator(arr, index-n); }
        std::size_t operator+ (const iterator &i) const { return index + i.index; }
        std::size_t operator- (const iterator &i) const { return index - i.index; }
        bool        operator==(const iterator &i) const { return arr == i.arr && index == i.index; }
        bool        operator!=(const iterator &i) const { return arr != i.arr || index != i.index; }
        bool        operator< (const iterator &i) const { return index < i.index; }
        reference   operator*() const                   { return (*arr)[index]; }
        pointer     operator->()                        { return &arr[index]; }
    };

    iterator begin() { return iterator(this, 0); }
    iterator end()   { return iterator(this, len); }
};

#endif
