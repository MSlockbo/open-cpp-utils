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

#ifndef OPEN_CPP_UTILS_TEMPLATE_UTILS_H
#define OPEN_CPP_UTILS_TEMPLATE_UTILS_H

namespace open_cpp_utils
{

/**
 * \file template_utils.h
 * \brief Provides compile time evaluation utilities for templates and template packs
 */

/**
 * \brief Check if a sequence of types is unique
 * \tparam Ts
 */
template<typename...Ts>
inline static constexpr bool is_unique = std::true_type{};

template<typename T, typename...Ts>
inline constexpr bool is_unique<T, Ts...> = std::bool_constant<(!std::is_same_v<T, Ts> && ...) && is_unique<Ts...>>{};

}

#endif // OPEN_CPP_UTILS_TEMPLATE_UTILS_H
