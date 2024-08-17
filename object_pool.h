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

#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include "dynarray.h"
#include "optional.h"

namespace open_cpp_utils
{

/**
 * \brief
 * \tparam T
 */
template<typename T>
class object_list
{
// Typedefs ============================================================================================================

public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

    using uuid_type  = uint64_t;

private:
    using node = optional<value_type>;


// Functions ===========================================================================================================

public:

// Constructors & Destructor -------------------------------------------------------------------------------------------

    object_list() = default;
    object_list(const object_list& other) = default;
    object_list(object_list&& other) = default;
    ~object_list() = default;


// Modifiers -----------------------------------------------------------------------------------------------------------

    size_t size() { return data_.size(); }


// Modifiers -----------------------------------------------------------------------------------------------------------

    void clear() { data_.clear(); freed_.clear(); }

    uuid_type insert(const_reference& value);
    void      erase(uuid_type id);

    object_list& operator=(const object_list&) = default;
    object_list& operator=(object_list&&) = default;


// Accessors -----------------------------------------------------------------------------------------------------------

    reference       operator[](uuid_type id)       { assert(data_[id]()); return data_[id]; }
    const_reference operator[](uuid_type id) const { assert(data_[id]()); return data_[id]; }
    bool            operator()(uuid_type id) const { return data_[id](); }

    typename dynarray<node>::iterator begin() { return data_.begin(); }
    typename dynarray<node>::iterator end()   { return data_.end(); }


// Variables ===========================================================================================================

private:
    dynarray<node>      data_;
    dynarray<uuid_type> freed_;
};

template<typename T>
typename object_list<T>::uuid_type object_list<T>::insert(const_reference value)
{
    if(freed_.empty()) { data_.push_back(value); return data_.size() - 1; }

    uuid_type id = freed_.back(); freed_.pop_back();
    data_[id] = value;
    return id;
}

template<typename T>
void object_list<T>::erase(uuid_type id)
{
    data_[id].reset();
    freed_.push_back(id);
}

} // open_cpp_utils

#endif //OBJECT_POOL_H
