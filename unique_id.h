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

#ifndef ENGINE_UNIQUEID_H
#define ENGINE_UNIQUEID_H

#include <cstdint>

namespace open_cpp_utils
{

// Internal function for incrementing an id associated with a type
template<typename Base> uint64_t _increment_id()
{
    static uint64_t current = 0;
    return current++;
}

/**
 * \brief Generate a unique id for a type given a base type
 * \tparam Base Base type for id categorization
 * \tparam Type Type for id generation
 * \return Generated ID for Type and Base combination
 *
 * Example:
 *
 * unique_id<void, int>() = 0
 * unique_id<void, unsigned int>() = 1
 *
 * unique_id<int, int>() = 0
 */
template<typename Base, typename Type> uint64_t unique_id()
{
    static bool initialized = false;
    static uint64_t id = 0;

    if(initialized) return id;
    initialized = true;
    return id = _increment_id<Base>();
}

}

#endif //ENGINE_UNIQUEID_H
