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

#ifndef OPEN_CPP_UTILS_MATH_H
#define OPEN_CPP_UTILS_MATH_H

#include <cmath>

namespace open_cpp_utils
{

template<typename T>
bool is_prime(T x)
{
    if(x <= 1)                   return false;
    if(x == 2     || x == 3)     return true;
    if(x % 2 == 0 || x % 3 == 0) return false;

    T limit = static_cast<T>(sqrt(x));
    for(T i = 5; i <= limit; i += 6)
    {
        if(x % i == 0 || x % (i + 2) == 0) return false;
    }

    return true;
}

}

#endif // OPEN_CPP_UTILS_MATH_H
