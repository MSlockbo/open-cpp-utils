// =====================================================================================================================
// Copyright 2024 Medusa Slockbower
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =====================================================================================================================

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <memory>
#include <type_traits>

#include "math.h"
#include "optional.h"

namespace open_cpp_utils
{

template<typename T, class Hash = std::hash<T>, class Alloc = std::allocator<T>>
class hash_table
{
// Typedefs ============================================================================================================

public:
    friend class iterator;

private:
    struct _Node;

public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

    using hash_type       = Hash;
    using allocator_type  = typename std::allocator_traits<Alloc>::template rebind_alloc<_Node>;

    using size_type       = size_t;
    using iterator_type   = iterator;

private:
    using table_type = _Node*;
    using node       = int64_t;
    static constexpr node nullnode = std::integral_constant<node, -1>{};


// Structs =============================================================================================================

private:
    struct _Node
    {
        optional<T> value;
        int         psl;

        _Node() : value(), psl(0) { }
    };


// Functions ===========================================================================================================

public:

// Constructors & Destructor -------------------------------------------------------------------------------------------

    hash_table() : table_(nullptr), capacity_(0), size_(0), load_factor_(0.8), hash_(), alloc_() { }
    hash_table(const hash_table&);
    hash_table(hash_table&&) = default; // Default Move Constructor should suffice
    ~hash_table() { clear(); }

// Modifiers -----------------------------------------------------------------------------------------------------------

    void clear();
    void insert(const_reference x);
    void erase(const_reference x);
    bool contains(const_reference x);
    iterator find(const_reference x) const { node res = _find(x); return iterator(this, res == nullnode ? capacity_ : res); }

// Modifiers -----------------------------------------------------------------------------------------------------------

    [[nodiscard]] size_type capacity()  const { return capacity_; }
    [[nodiscard]] size_type size()      const { return size_; }
    [[nodiscard]] bool      empty()     const { return size_ == 0; }
    [[nodiscard]] double    occupancy() const { return size_ / static_cast<double>(capacity_); }

// Helpers -------------------------------------------------------------------------------------------------------------

private:
    void _increase_capacity();

    [[nodiscard]] node _hash(const_reference v) const;
    [[nodiscard]] node _find(const_reference x) const;
    [[nodiscard]] node _next(node n)            const { return (n + 1)             % capacity_; }
    [[nodiscard]] node _prev(node n)            const { return (n - 1 + capacity_) % capacity_; }

    static size_type _next_prime(size_type x);
    static size_type _prev_prime(size_type x);


// Iterators ===========================================================================================================

    class iterator
    {
    public:
        iterator(hash_table* table, int idx) : table_(table), idx_(idx) { _next_index(); }
        iterator(const iterator&) = default;
        iterator(iterator&&) = default;
        ~iterator() = default;

        iterator& operator++() { ++idx_; _next_index(); return *this; }
        iterator  operator++(int) { iterator ret = *this; ++idx_; _next_index(); return ret; }

        bool operator==(const iterator& o) const = default;
        bool operator!=(const iterator& o) const = default;

        reference operator*()  const { return  table_->table_[idx_].value; }
        pointer   operator->() const { return &table_->table_[idx_].value; }

    private:
        void _next_index() { while(idx_ < table_->capacity_ && table_[idx_].value()) ++idx_; }

        hash_table* table_;
        int         idx_;
    };

    iterator begin() { return iterator(this, 0);        }
    iterator end()   { return iterator(this, capacity_); }


// Variables ===========================================================================================================

private:
    table_type     table_;
    size_type      capacity_, size_;
    double         load_factor_;
    hash_type      hash_;
    allocator_type alloc_;
};

template<typename T, class Hash, class Alloc>
hash_table<T, Hash, Alloc>::hash_table(const hash_table& other)
    : table_(nullptr)
    , capacity_(other.capacity_)
    , size_(other.size_)
    , load_factor_(0.8)
{

}

template<typename T, class Hash, class Alloc>
void hash_table<T, Hash, Alloc>::clear()
{
    alloc_.deallocate(table_, capacity_);
    capacity_ = size_ = 0;
}

template<typename T, class Hash, class Alloc>
void hash_table<T, Hash, Alloc>::insert(const_reference x)
{
    if(occupancy() > load_factor_ || capacity_ == 0) _increase_capacity();

    node idx = _hash(x);
    int  psl = 0;
    T  value = x;

    while(table_[idx].value())
    {
        _Node& node = table_[idx];

        if(node.value == x) return;
        if(psl > node.psl) { std::swap(psl, node.psl); std::swap(value, node.value); }

        idx = _next(idx);
        ++psl;
    }

    table_[idx].value = value;
    table_[idx].psl   = psl;
    ++size_;
}

template<typename T, class Hash, class Alloc>
void hash_table<T, Hash, Alloc>::erase(const_reference x)
{
    node idx = _find(x);
    if(idx == nullnode) return;

    table_[idx].value.reset();
    --size_;

    node prev = idx; idx = _next(idx);
    while(table_[idx].value() && table_[idx].psl > 0)
    {
        _Node &a = table_[prev], &b = table_[idx];
        std::swap(a, b);
        --a.psl; prev = idx; idx = _next(idx);
    }
}

template<typename T, class Hash, class Alloc>
bool hash_table<T, Hash, Alloc>::contains(const_reference x)
{
    return _find(x) != nullnode;
}

template<typename T, class Hash, class Alloc>
void hash_table<T, Hash, Alloc>::_increase_capacity()
{
    table_type old = table_;
    size_type  old_capacity = capacity_;
    capacity_ = _next_prime(capacity_);
    table_    = alloc_.allocate(capacity_);
    memset(table_, 0, capacity_ * sizeof(_Node));
    size_ = 0;

    for(node i = 0; i < old_capacity; ++i)
    {
        if(old[i].value()) insert(old[i].value);
    }

    alloc_.deallocate(old, old_capacity);
}

template<typename T, class Hash, class Alloc>
typename hash_table<T, Hash, Alloc>::node hash_table<T, Hash, Alloc>::_hash(const_reference v) const
{
    node x = hash_(v);

    x ^= x >> 33U;
    x *= UINT64_C(0xff51afd7ed558ccd);
    x ^= x >> 33U;
    x *= UINT64_C(0xc4ceb9fe1a85ec53);
    x ^= x >> 33U;

    return x;
}

template<typename T, class Hash, class Alloc>
typename hash_table<T, Hash, Alloc>::node hash_table<T, Hash, Alloc>::_find(const_reference x) const
{
    if(capacity_ == 0) return nullnode;
    node idx = _hash(x);
    int psl = 0;

    while(table_[idx].value())
    {
        _Node& node = table_[idx];

        if(node.psl > psl)  return nullnode;
        if(node.value == x) return idx;

        idx = _next(idx); ++psl;
    }

    return nullnode;
}

template<typename T, class Hash, class Alloc>
typename hash_table<T, Hash, Alloc>::size_type hash_table<T, Hash, Alloc>::_next_prime(size_type x)
{
    size_type n = (x + 1) / 6;
    n *= 2;

    while(true)
    {
        x = (n * 6) - 1;
        if(!is_prime(x)) x = (n * 6) + 1;
        if(!is_prime(x)) { ++n; continue; }
        return std::max(x, 7ull);
    }
}

template<typename T, class Hash, class Alloc>
typename hash_table<T, Hash, Alloc>::size_type hash_table<T, Hash, Alloc>::_prev_prime(size_type x)
{
    size_type n = (x + 1) / 6;
    n *= 2;

    while(true)
    {
        x = (n * 6) - 1;
        if(!is_prime(x)) x = (n * 6) + 1;
        if(!is_prime(x)) { --n; continue; }
        return std::max(x, 7ull);
    }
}

}

#endif //HASH_TABLE_H
