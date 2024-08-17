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

#ifndef MAP_H
#define MAP_H
#include "set.h"

namespace open_cpp_utils
{

template<typename Key, typename Value, class Alloc = std::allocator<std::pair<Key, Value>>>
class map
{
// Typedefs ============================================================================================================

public:
    struct hash { size_t operator()(const pair_type& pair) const { return std::hash<key_type>{}(pair.first); } };

    using key_type   = Key;
    using value_type = Value;
    using pair_type  = std::pair<key_type, value_type>;
    using table_type = set<pair_type, hash, Alloc>;

    using key_pointer         = key_type*;
    using const_key_pointer   = const key_type*;
    using key_reference       = key_type&;
    using const_key_reference = const key_type&;

    using value_pointer         = value_type*;
    using const_value_pointer   = const value_type*;
    using value_reference       = value_type&;
    using const_value_reference = const value_type&;

    using iterator = typename table_type::iterator;


// Functions ===========================================================================================================

// Constructors & Destructor -------------------------------------------------------------------------------------------

public:
    map()           = default;
    map(const map&) = default;
    map(map&&)      = default;
    ~map()          = default;

    void insert(const_key_reference key, const_value_reference value);
    void erase(const_key_reference key);
    value_reference& operator[](const_key_reference key);
    iterator find() { return table_->find(); }
    bool contains(const_key_reference key) { return table_.contains({ key, value_type() }); }

    iterator begin() { return table_.begin(); }
    iterator end()   { return table_.end(); }

// Variables ===========================================================================================================

private:
    set<pair_type, hash, Alloc> table_;
};

template<typename Key, typename Value, class Alloc>
void map<Key, Value, Alloc>::insert(const_key_reference key, const_value_reference value)
{
    iterator it = find({ key, value });
    if(it != end()) table_.insert({ key, value });
}

template<typename Key, typename Value, class Alloc>
void map<Key, Value, Alloc>::erase(const_key_reference key)
{
    table_.erase({ key, value_type() });
}

template<typename Key, typename Value, class Alloc>
typename map<Key, Value, Alloc>::value_reference map<Key, Value, Alloc>::operator[](const_key_reference key)
{
    iterator it = table_.find({ key, value_type() });
    if(it == table_.end())
    {
        table_.insert({ key, value_type() });
        it = table_.find({ key, value_type() });
    }
    return it->second;
}
}

#endif //MAP_H
