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

#ifndef OPEN_CPP_UTILS_OPTIONAL_H
#define OPEN_CPP_UTILS_OPTIONAL_H

#include <assert.h>

namespace open_cpp_utils
{
    template<typename T>
    class optional
    {
    public:
        using value_type = T;

        optional() : valid_(false) { }
        optional(const value_type& data) : data_(data), valid_(true) { }
        optional(value_type&& data) : data_(data), valid_(true) { }
        optional(const optional& other) = default;
        optional(optional&& other) = default;

        optional& operator=(const optional& other) = default;
        optional& operator=(optional&& other) = default;

	    optional& operator=(const value_type& data) { data_ = data; valid_ = true; return *this; }
	    optional& operator=(value_type&& data) { data_ = data; valid_ = true; return *this; }

        bool operator==(const optional& b) const
	    {
	        if(not(valid_ || b.valid_)) return valid_ == b.valid_;
	        return data_ == b.data_;
	    }

        value_type& operator+=(const value_type& data) { assert(valid_); data_ += data; return data_; }
        value_type& operator-=(const value_type& data) { assert(valid_); data_ += data; return data_; }
        value_type& operator*=(const value_type& data) { assert(valid_); data_ += data; return data_; }
        value_type& operator/=(const value_type& data) { assert(valid_); data_ += data; return data_; }
        value_type& operator%=(const value_type& data) { assert(valid_); data_ += data; return data_; }

        value_type& operator<<=(const value_type& data) { assert(valid_); data_ <<= data; return data_; }
        value_type& operator>>=(const value_type& data) { assert(valid_); data_ >>= data; return data_; }
        value_type& operator|=(const value_type& data)  { assert(valid_); data_ |= data;  return data_; }
        value_type& operator&=(const value_type& data)  { assert(valid_); data_ &= data;  return data_; }
        value_type& operator^=(const value_type& data)  { assert(valid_); data_ ^= data;  return data_; }

        [[nodiscard]] bool operator()() const { return valid_; }

        operator value_type() const { assert(valid_); return data_; }

        operator       value_type&()       { assert(valid_); return data_; }
        operator const value_type&() const { assert(valid_); return data_; }

	          value_type* operator->()       { assert(valid_); return &data_; }
        const value_type* operator->() const { assert(valid_); return &data_; }

	          value_type& operator*()        { assert(valid_); return data_; }
        const value_type& operator*() const  { assert(valid_); return data_; }

        void reset() { valid_ = false; }

    private:
	    value_type data_;
        bool valid_;
    };
}

#endif // OPEN_CPP_UTILS_OPTIONAL_H
