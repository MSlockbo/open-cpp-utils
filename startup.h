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

#ifndef STARTUP_H
#define STARTUP_H

#ifdef __cplusplus
#define STARTUP(f) \
        static void f(void); \
        struct f##_t_ { f##_t_(void) { f(); } }; inline static f##_t_ f##_; \
        static void f(void)
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU",read)
    #define INITIALIZER2_(f,p) \
        static void f(void); \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
        __pragma(comment(linker,"/include:" p #f "_")) \
        static void f(void)
    #ifdef _WIN64
        #define STARTUP(f) INITIALIZER2_(f,"")
    #else
        #define STARTUP(f) INITIALIZER2_(f,"_")
    #endif
#else
    #define STARTUP(f) \
        static void f(void) __attribute__((constructor)); \
        static void f(void)
#endif

#endif //STARTUP_H
