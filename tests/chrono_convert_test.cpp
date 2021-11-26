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

#include <otf2xx/otf2.hpp>

#include <map>

auto tp = [](auto ticks) { return otf2::chrono::time_point(otf2::chrono::duration(ticks)); };

TEST_CASE("chrono conversions work")
{
    CHECK(otf2::chrono::duration::period::den == std::pico::den);

    GIVEN("a converter with 1000 ticks/s and an offset of 42")
    {
        otf2::chrono::convert c(otf2::chrono::ticks(1000), otf2::chrono::ticks(42));

        WHEN("converting null back and forth")
        {
            auto res = c(tp(0));

            THEN("the intermediate is what one would expect")
            {
                REQUIRE(res.count() == otf2::chrono::ticks(42).count());
            }

            THEN("it stays null")
            {
                REQUIRE(c(res) == tp(0));
            }
        }

        WHEN("converting a random time_point back and forth")
        {
            auto res = c(tp(std::pico::den));

            THEN("the intermediate is what one would expect")
            {
                REQUIRE(res.count() == otf2::chrono::ticks(1042).count());
            }

            THEN("it stays null")
            {
                REQUIRE(c(res) == tp(1'000'000'000'000));
            }
        }

        WHEN("converting a random tick back and forth")
        {
            auto res = c(otf2::chrono::ticks(1042));

            THEN("the intermediate is what one would expect")
            {
                REQUIRE(res == tp(1'000'000'000'000));
            }

            THEN("it stays null")
            {
                REQUIRE(c(res).count() == otf2::chrono::ticks(1042).count());
            }
        }
    }

    GIVEN("a converter with sane settings")
    {
        auto TICKS_PER_SECOND = otf2::chrono::ticks(std::pico::den);
        auto OFFSET = otf2::chrono::ticks(1000 * std::pico::den);

        otf2::chrono::convert c(TICKS_PER_SECOND, OFFSET);

        WHEN("converting a second")
        {
            auto res = c(otf2::chrono::time_point(std::chrono::seconds(1)));

            THEN("it should retrun the offset + ticks per second")
            {
                REQUIRE(res.count() ==
                        otf2::chrono::ticks(OFFSET.count() + TICKS_PER_SECOND.count()).count());
            }
        }

        WHEN("converting null back and forth")
        {
            auto res = c(tp(0));

            THEN("the intermediate is what one would expect")
            {
                REQUIRE(res.count() == OFFSET.count());
            }

            THEN("it stays null")
            {
                REQUIRE(c(res) == tp(0));
            }
        }

        WHEN("converting a time_point back and forth")
        {
            auto res = c(tp(1024 * std::pico::den));

            THEN("the intermediate is what one would expect")
            {
                REQUIRE(res.count() ==
                        otf2::chrono::ticks(OFFSET.count() + 1024 * std::pico::den).count());
            }

            THEN("it stays the same")
            {
                REQUIRE(c(res) == tp(1024 * std::pico::den));
            }
        }

        WHEN("converting a tick back and forth")
        {
            auto res = c(otf2::chrono::ticks(1042 * std::pico::den));

            THEN("the intermediate is what one would expect")
            {
                REQUIRE(res == tp(42 * std::pico::den));
            }

            THEN("it stays the same")
            {
                REQUIRE(c(res).count() == otf2::chrono::ticks(1042 * std::pico::den).count());
            }
        }

        WHEN("converting max-1 tick back and forth")
        {
            auto res = c(otf2::chrono::ticks(std::numeric_limits<std::int64_t>::max() - 1));

            THEN("the intermediate is what one would expect")
            {
                REQUIRE(res == tp(std::numeric_limits<std::int64_t>::max() - OFFSET.count() + 1));
            }

            THEN("it stays the same, sort of")
            {
                REQUIRE(
                    c(res).count() ==
                    otf2::chrono::ticks(
                        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1)
                        .count());
            }
        }

        WHEN("converting max tick back and forth")
        {
            auto res = c(otf2::chrono::ticks(std::numeric_limits<std::int64_t>::max()));

            THEN("the intermediate is what one would expect")
            {
                REQUIRE(res == tp(std::numeric_limits<std::int64_t>::max() - OFFSET.count() + 1));
            }

            THEN("it stays the same, sort of")
            {
                REQUIRE(
                    c(res).count() ==
                    otf2::chrono::ticks(
                        static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1)
                        .count());
            }
        }
    }

    GIVEN("a converter with typical settings")
    {
        auto TICKS_PER_SECOND = otf2::chrono::ticks(2494226548);
        auto OFFSET = otf2::chrono::ticks(5202756409534672);

        otf2::chrono::convert c(TICKS_PER_SECOND, OFFSET);

        WHEN("converting a second")
        {
            auto res = c(otf2::chrono::time_point(std::chrono::seconds(1)));

            THEN("it should retrun the offset + ticks per second")
            {
                REQUIRE(res.count() ==
                        otf2::chrono::ticks(OFFSET.count() + TICKS_PER_SECOND.count()).count());
            }
        }

        WHEN("converting null back and forth")
        {
            auto res = c(tp(0));

            THEN("the intermediate is what one would expect")
            {
                REQUIRE(res.count() == OFFSET.count());
            }

            THEN("it stays null")
            {
                REQUIRE(c(res) == tp(0));
            }
        }

        WHEN("converting a time_point back and forth")
        {
            auto res = c(tp(15202756409534672));

            THEN("the intermediate is what one would expect")
            {
                REQUIRE(res.count() == otf2::chrono::ticks(5240675528174110).count());
            }

            THEN("it stays the same")
            {
                // rounding errors are a beautiful
                REQUIRE(c(res) == tp(15202756409534456));
            }
        }
    }
}
