/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2018, Technische Universitaet Dresden, Germany
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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <otf2xx/otf2.hpp>
#include <otf2xx/reference_generator.hpp>
#include <set>

template <typename T>
bool contains(const std::set<T>& container, const T& el)
{
    return container.find(el) != container.end();
}

TEST_CASE("test string reference generator")
{
    otf2::trace_reference_generator ref_gen;
    otf2::definition::string s1{ 1, "Foo" };
    using string_ref = otf2::reference<otf2::definition::string>;
    std::set<string_ref> string_refs;
    SECTION("Register with function")
    {
        ref_gen.register_definition(s1);
        string_refs.insert(s1.ref());
    }
    SECTION("Register with functor")
    {
        ref_gen(s1);
        string_refs.insert(s1.ref());
        SECTION("Duplicate register should be allowed")
        {
            ref_gen(s1);
        }
    }
    SECTION("Register multiple definitions")
    {
        // Use some with gaps
        otf2::definition::string s2{ 2, "Foo" };
        otf2::definition::string s3{ 4, "Foo" };
        otf2::definition::string s4{ 10, "Foo" };
        ref_gen(s2);
        string_refs.insert(s2.ref());
        ref_gen(s3);
        string_refs.insert(s3.ref());
        ref_gen(s4);
        string_refs.insert(s4.ref());
    }
    // Check that the new couple of next() calls return unique refs (for every leaf section above)
    for (int i = 0; i < 100; i++)
    {
        auto new_ref = ref_gen.next<otf2::definition::string>();
        REQUIRE_FALSE(contains(string_refs, new_ref));
        string_refs.insert(new_ref);
    }
}

TEST_CASE("test io reference generator")
{
    otf2::trace_reference_generator ref_gen;
    otf2::definition::string str{0, ""};
    otf2::definition::system_tree_node sys_node{1, str, str};
    otf2::definition::io_regular_file file{ 1, str, sys_node };
    otf2::definition::io_directory dir{ 2, str, sys_node };
    using ref = otf2::reference<otf2::definition::io_file>;
    std::set<ref> refs;
    SECTION("Directory ref must be unique after adding file ref")
    {
        ref_gen(file);
        refs.insert(file.ref());
        for (int i = 0; i < 100; i++)
        {
            auto new_ref = ref_gen.next<otf2::definition::io_directory>();
            REQUIRE_FALSE(contains(refs, new_ref));
            refs.insert(new_ref);
        }
    }
    SECTION("File ref must be unique after adding directory ref")
    {
        ref_gen(dir);
        refs.insert(dir.ref());
        for (int i = 0; i < 100; i++)
        {
            auto new_ref = ref_gen.next<otf2::definition::io_regular_file>();
            REQUIRE_FALSE(contains(refs, new_ref));
            refs.insert(new_ref);
        }
    }
    SECTION("All refs must be unqiue after adding both")
    {
        ref_gen(file);
        refs.insert(file.ref());
        ref_gen(dir);
        refs.insert(dir.ref());
        for (int i = 0; i < 100; i++)
        {
            auto new_ref = ref_gen.next<otf2::definition::io_file>();
            REQUIRE_FALSE(contains(refs, new_ref));
            refs.insert(new_ref);
            new_ref = ref_gen.next<otf2::definition::io_regular_file>();
            REQUIRE_FALSE(contains(refs, new_ref));
            refs.insert(new_ref);
            new_ref = ref_gen.next<otf2::definition::io_directory>();
            REQUIRE_FALSE(contains(refs, new_ref));
            refs.insert(new_ref);
        }
    }
}
