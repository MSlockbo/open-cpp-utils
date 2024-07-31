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

#ifndef TEMPLATEUTILS_H
#define TEMPLATEUTILS_H

namespace open_cpp_utils
{

/**
 * \file template_utils.h
 * \brief Provides compile time evaluation utilities for templates and template packs
 */

/**
 * \brief Compile-time constant value
 * \tparam T Type
 * \tparam V Value
 */
template<typename T, T V>
struct constant_value
{
	using type = T;
	static constexpr type value = V;

	constexpr operator type() const noexcept { return value; }
	[[nodiscard]] constexpr type operator()() const { return value; }
};

/**
 * \brief Compile-time constant boolean value
 * \tparam V Value
 */
template<bool V>
using bool_constant = constant_value<bool, V>;

using true_type  = bool_constant<true>; //!< Constant True value
using false_type = bool_constant<false>; //!< Constant False value

/**
 * \brief Check if two types are the same
 */
template<typename, typename>
inline static constexpr bool is_same = false_type{};

/**
 * \brief is_same true case for pair of identical types T
 */
template<typename T>
inline static constexpr bool is_same<T, T> = true_type{};

/**
 * \brief Check if a sequence of types is unique
 * \tparam Ts
 */
template<typename...Ts>
inline static constexpr bool is_unique = true_type{};

template<typename T, typename...Ts>
inline constexpr bool is_unique<T, Ts...> = bool_constant<(!is_same<T, Ts> && ...) && is_unique<Ts...>>{};

}

#endif //TEMPLATEUTILS_H
