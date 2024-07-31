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

#ifndef ANY_H
#define ANY_H

#include "template_utils.h"

namespace open_cpp_utils
{

/**
 * \brief Wrapper for a value with multiple types
 * \tparam Ts Types to include, must be unique
 */
template<typename...Ts> class any;

template<typename T, typename...Rest>
class any<T, Rest...> : public any<Rest...>
{
// Assertions ==========================================================================================================

    static_assert(is_unique<Rest...>);


// Typedefs ============================================================================================================

public:
    using base_type = any<Rest...>;
    using this_type = T;


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


// Cast operators ------------------------------------------------------------------------------------------------------

    operator       this_type  () const { return  Value; }
    operator       this_type& ()       { return  Value; }
    operator const this_type& () const { return  Value; }
    operator       this_type&&()       { return  Value; }
    operator       this_type* ()       { return &Value; }
    operator const this_type* () const { return &Value; }


// Variables ===========================================================================================================

private:
    static constexpr size_t Size = sizeof...(Rest);
    this_type               Value;
};

template<>
class any<> { };

}


#endif //ANY_H
