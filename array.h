//
// Created by Maddie on 7/25/2024.
//

#ifndef ARRAY_H
#define ARRAY_H

#include "template_utils.h"

#include <stdint.h>

#include <array>

namespace open_cpp_utils
{

// =====================================================================================================================
// Forward Definitions
// =====================================================================================================================
template<typename T, size_t N> struct fixed_array;
template<typename T, size_t N> class  dyn_array;
template<typename T, size_t N> using  array = fixed_array<T, N>;


// =====================================================================================================================
// Fixed Array
// =====================================================================================================================

/**
 * \brief Wrapper for array of type T with fixed length N
 * \tparam T
 * \tparam N
 */
template<typename T, size_t N>
struct fixed_array<T, N>
{
// Typedefs ============================================================================================================

public:
    using value_type = T;
    using array_type = value_type[N];
    using size_type = size_t;

// Functions ===========================================================================================================

public:

// Constructors & Destructor -------------------------------------------------------------------------------------------

    constexpr fixed_array() : data_{ T() } {}
    constexpr fixed_array(const fixed_array& array) : data_{ array.data_ } {}
    constexpr fixed_array(const array_type& data) : data_{ data } {}

    constexpr size_type size() { return N; }


// Variables ===========================================================================================================

private:
    array_type data_;

};

}


#endif //ARRAY_H
