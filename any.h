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

#ifndef OPEN_CPP_UTILS_ANY_H
#define OPEN_CPP_UTILS_ANY_H

#include "template_utils.h"

namespace open_cpp_utils
{

/**
 * \brief Wrapper for a value with multiple types
 * \tparam Ts Types to include, must be unique
 */
template<typename...Ts> class any;

template<> class any<> { };

template<typename T, typename...Rest>
class any<T, Rest...> : public any<Rest...>
{
// Assertions ==========================================================================================================

    static_assert(is_unique<T, Rest...>);


// Typedefs ============================================================================================================

public:
    using base_type = any<Rest...>;

    using this_type       = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;


// Constructors ========================================================================================================

public:
    any()                                            : base_type()        , Value() { }
    any(const this_type& value, const Rest&...other) : base_type(other...), Value(value) { }
    any(this_type&& value, Rest&&...other)           : base_type(other...), Value(value) { }
    any(const any& other)                            = default;
    any(any&& other)                                 = default;
    ~any()                                           = default;


// Operators ===========================================================================================================

public:

// Assignment operators ------------------------------------------------------------------------------------------------

    any& operator=(const any&) = default;
    any& operator=(any&&)      = default;


// Access --------------------------------------------------------------------------------------------------------------

    template<typename V>
    V& get() { static_assert(std::disjunction<std::is_same<V, T>, std::is_same<V, Rest>...>{}); return static_cast<V&>(*this); }


// Cast operators ------------------------------------------------------------------------------------------------------

    operator       reference()       { return  Value; }
    operator const_reference() const { return  Value; }
    operator         pointer()       { return &Value; }
    operator   const_pointer() const { return &Value; }


// Variables ===========================================================================================================

private:
    static constexpr size_t Size = sizeof...(Rest);
    this_type               Value;
};

}


#endif // OPEN_CPP_UTILS_ANY_H
