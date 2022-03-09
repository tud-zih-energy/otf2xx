/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2018, Technische Universität Dresden, Germany
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <map>
#include <otf2xx/otf2.hpp>

struct debug_ptr
{
    struct call_nums
    {
        uint64_t retain, release;
    };
    static std::map<debug_ptr*, int64_t> refCts;
    static call_nums calls;

    debug_ptr()
    {
        refCts[this] = 0;
    }
    ~debug_ptr()
    {
        REQUIRE(refCts[this] == 0);
    }
    void retain()
    {
        REQUIRE(refCts[this] >= 0);
        ++calls.retain;
        ++refCts[this];
    }
    int64_t release()
    {
        REQUIRE(refCts[this] > 0);
        ++calls.release;
        return --refCts[this];
    }
};

std::map<debug_ptr*, int64_t> debug_ptr::refCts{};
debug_ptr::call_nums debug_ptr::calls = { 0, 0 };

using intrusive_ptr = otf2::intrusive_ptr<debug_ptr>;

TEST_CASE("basic test", "[intrusive_ptr]")
{
    auto raw_ptr = new debug_ptr;
    intrusive_ptr ptr(raw_ptr);
    REQUIRE(raw_ptr == ptr.get());
    REQUIRE(raw_ptr == &(*ptr));

    SECTION("Equality")
    {
        intrusive_ptr ptr2(raw_ptr);
        REQUIRE(ptr2.get() == ptr.get());
        REQUIRE(ptr == ptr);
        REQUIRE(ptr2 == ptr);
        intrusive_ptr empty, empty2;
        REQUIRE(empty == empty);
        REQUIRE(empty == empty2);
    }
    SECTION("Inequality")
    {
        intrusive_ptr ptr2(new debug_ptr);
        REQUIRE(ptr2.get() != ptr.get());
        REQUIRE(ptr2 != ptr);
        intrusive_ptr empty;
        REQUIRE(empty != ptr2);
        REQUIRE(empty != ptr);
    }
}

TEST_CASE("test lifetime", "[intrusive_ptr]")
{
    {
        auto raw_ptr = new debug_ptr;
        intrusive_ptr ptr(raw_ptr);
        REQUIRE(raw_ptr == ptr.get());
        REQUIRE(raw_ptr == &(*ptr));
        REQUIRE(debug_ptr::refCts[raw_ptr] == 1);
        SECTION("Copy ctor")
        {
            {
                auto calls = debug_ptr::calls;
                intrusive_ptr new_ptr(ptr);
                REQUIRE(debug_ptr::refCts[raw_ptr] == 2);
                REQUIRE(ptr == new_ptr);
                REQUIRE(debug_ptr::calls.retain == calls.retain + 1);
            }
            REQUIRE(debug_ptr::refCts[raw_ptr] == 1);
        }
        SECTION("Default construction and copy assignment")
        {
            {
                auto calls = debug_ptr::calls;
                intrusive_ptr new_ptr;
                REQUIRE(debug_ptr::refCts[raw_ptr] == 1);
                REQUIRE(!new_ptr);
                REQUIRE(new_ptr.get() == nullptr);
                new_ptr = ptr;
                REQUIRE(debug_ptr::refCts[raw_ptr] == 2);
                REQUIRE(!!new_ptr);
                REQUIRE(ptr == new_ptr);
                REQUIRE(debug_ptr::calls.retain == calls.retain + 1);
            }
            REQUIRE(debug_ptr::refCts[raw_ptr] == 1);
        }
        SECTION("Move ctor")
        {
            {
                auto calls = debug_ptr::calls;
                intrusive_ptr tmp_ptr(ptr);
                intrusive_ptr new_ptr(std::move(tmp_ptr));
                REQUIRE(debug_ptr::refCts[raw_ptr] == 2);
                REQUIRE(ptr == new_ptr);
                REQUIRE(debug_ptr::calls.retain == calls.retain + 1);
            }
            REQUIRE(debug_ptr::refCts[raw_ptr] == 1);
        }
        SECTION("Move assign")
        {
            {
                auto calls = debug_ptr::calls;
                intrusive_ptr tmp_ptr(ptr);
                intrusive_ptr new_ptr;
                new_ptr = std::move(tmp_ptr);
                REQUIRE(debug_ptr::refCts[raw_ptr] == 2);
                REQUIRE(ptr == new_ptr);
                REQUIRE(debug_ptr::calls.retain == calls.retain + 1);
            }
            REQUIRE(debug_ptr::refCts[raw_ptr] == 1);
        }
        SECTION("Different ptrs")
        {
            {
                auto calls = debug_ptr::calls;
                auto raw_ptr2 = new debug_ptr;
                intrusive_ptr new_ptr(raw_ptr2);
                REQUIRE(debug_ptr::refCts[raw_ptr] == 1);
                REQUIRE(debug_ptr::refCts[raw_ptr2] == 1);
                REQUIRE(ptr.get() == raw_ptr);
                REQUIRE(new_ptr.get() == raw_ptr2);
                REQUIRE(debug_ptr::calls.retain == calls.retain + 1);
                auto ptr_copy = ptr;
                REQUIRE(debug_ptr::refCts[raw_ptr] == 2);
                REQUIRE(debug_ptr::refCts[raw_ptr2] == 1);
                REQUIRE(debug_ptr::calls.retain == calls.retain + 2);
                ptr_copy = new_ptr;
                REQUIRE(debug_ptr::refCts[raw_ptr] == 1);
                REQUIRE(debug_ptr::refCts[raw_ptr2] == 2);
                REQUIRE(debug_ptr::calls.retain == calls.retain + 3);
            }
            REQUIRE(debug_ptr::refCts[raw_ptr] == 1);
        }
        SECTION("Self assignment")
        {
            {
                auto calls = debug_ptr::calls;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
                // Note: We actually want to test the behavior of self-assignment here, so tHiS iS
                // fInE!
                ptr = ptr;
#pragma GCC diagnostic pop
                REQUIRE(ptr.get() == raw_ptr);
                REQUIRE(debug_ptr::refCts[raw_ptr] == 1);
                REQUIRE(debug_ptr::calls.retain == calls.retain);
            }
        }
        SECTION("Same handle assignment")
        {
            {
                auto calls = debug_ptr::calls;
                // Note: Using the raw pointer out of a smart pointer is only valid for
                // intrusive_ptrs!
                intrusive_ptr new_ptr(ptr.get());
                REQUIRE(new_ptr == ptr);
                REQUIRE(debug_ptr::calls.retain == calls.retain + 1);
                REQUIRE(debug_ptr::refCts[raw_ptr] == 2);
                // The smart pointers are different but the pointess (handles) are the same
                // We don't want any reference counter handling here
                calls = debug_ptr::calls;
                SECTION("Copy")
                {
                    ptr = new_ptr;
                    REQUIRE(ptr.get() == raw_ptr);
                    REQUIRE(debug_ptr::calls.retain == calls.retain);
                    REQUIRE(debug_ptr::refCts[raw_ptr] == 2);
                }
                SECTION("Move")
                {
                    ptr = std::move(new_ptr);
                    REQUIRE(ptr.get() == raw_ptr);
                    REQUIRE(debug_ptr::calls.retain == calls.retain);
                    // Note: The moved from state is undefined
                    // So in the case of self-handle assignment a do-nothing or a free of the
                    // moved-from object is valid
                    REQUIRE_THAT(debug_ptr::refCts[raw_ptr],
                                 Catch::Predicate<int64_t>([](int64_t val) -> bool
                                                           { return val == 1 || val == 2; }));
                }
            }
        }
    }
    // Those must be equal as all objects should be freed by now.
    // The Catch2-sections allow to assert this for all cases combined
    REQUIRE(debug_ptr::calls.retain == debug_ptr::calls.release);
}
