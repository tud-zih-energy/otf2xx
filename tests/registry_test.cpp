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
#include <otf2xx/registry.hpp>
#include <set>

template <typename T>
bool contains(const std::set<T>& container, const T& el)
{
    return container.find(el) != container.end();
}

TEST_CASE("Add and get strings")
{
    otf2::Registry reg;
    SECTION("Use predefined id")
    {
        const std::string value = "The answer.";
        SECTION("Add existing string")
        {
            otf2::definition::string str(42, value);
            reg.register_definition(str);
            REQUIRE(str == reg.get<otf2::definition::string>(42));
        }
        SECTION("Create with id")
        {
            auto str = reg.create<otf2::definition::string>(42, value);
            REQUIRE(str.str() == value);
            REQUIRE(str.ref() == 42);
            REQUIRE(str == reg.get<otf2::definition::string>(42));
        }
        auto str = reg.get<otf2::definition::string>(42);
        REQUIRE(str.str() == value);
        REQUIRE(str.ref() == 42);
    }
    SECTION("Use auto-generated id")
    {
        std::set<otf2::reference<otf2::definition::string>> refs;
        SECTION("With manually added string")
        {
            refs.insert(reg.create<otf2::definition::string>(42, "foo").ref());
        }
        SECTION("Without manually added string")
        {
            // Nothing to do
        }
        for (int i = 0; i < int(1e5); i++)
        {
            auto str = reg.create<otf2::definition::string>("Value" + std::to_string(i));
            REQUIRE(!contains(refs, str.ref()));
            refs.insert(str.ref());
        }
    }
}
