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

#include <stack>
#include <vector>
#include <tuple>
#include <unordered_map>

#include "template_utils.h"

namespace open_cpp_utils
{

template<typename T, typename _Hash = std::unordered_map<uint64_t, int64_t>>
class object_pool
{
// Typedefs ============================================================================================================

public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

    using index_type = int64_t;
    using uuid_type  = uint64_t;

private:
    using node = std::tuple<value_type, bool>;


// Constants ===========================================================================================================

public:
    static constexpr std::integral_constant<index_type, -1> nullidx{};


// Functions ===========================================================================================================

public:

// Constructors & Destructor -------------------------------------------------------------------------------------------

    object_pool();

    void clear();
    void reset();
    void cleanup();

    uuid_type insert(const_reference& value);
    void      erase(uuid_type id);
    void      erase(index_type idx);

// Accessors -----------------------------------------------------------------------------------------------------------

    reference       operator[](uuid_type id);
    const_reference operator[](uuid_type id) const;
    bool            operator()(uuid_type id) const;

    reference       operator[](index_type idx);
    const_reference operator[](index_type idx) const;
    bool            operator()(index_type idx) const;


private:
    std::vector<node>      data_;
    _Hash                  map_;
    std::stack<index_type> freed_;
};

} // open_cpp_utils

#endif //OBJECT_POOL_H
