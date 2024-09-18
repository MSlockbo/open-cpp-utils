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

#ifndef OPEN_CPP_UTILS_DYNARRAY_H
#define OPEN_CPP_UTILS_DYNARRAY_H

#include <memory>
#include <vector>

namespace open_cpp_utils
{

template<typename T, class Alloc = std::allocator<T>>
using dynarray = std::vector<T, Alloc>;

}

#endif // OPEN_CPP_UTILS_DYNARRAY_H
