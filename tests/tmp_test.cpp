/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2016, Technische Universität Dresden, Germany
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

#include <otf2xx/tmp.hpp>
#include <tuple>
#include <type_traits>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

namespace tmp = otf2::tmp;

/**
 * Required because we get something like (std::true_type) (with parens)
 * on which we cannot do (std::true_type)::value
 * Instead we (pretend) to call a function with a function pointer taking our type as an argument
 * and return our argument which can then be queried by decltype
 */
template <typename Pred>
Pred get_type(void (*)(Pred));
// Make it less ugly
#define TMP_GET_TYPE(PRED) decltype(get_type((void(*) PRED)0))
static_assert(std::is_same<TMP_GET_TYPE((int)), int>::value, "get_type is wrong");
static_assert(!std::is_same<TMP_GET_TYPE((int)), float>::value, "get_type is wrong");

// Note: All asserts require double-parens: TMP_ASSERT((mycheck))
// Shorter static assert for any predicate having a ::value member. Requires true
#define TMP_ASSERT(PRED) static_assert(TMP_GET_TYPE(PRED)::value, #PRED)
// Shorter static assert for any predicate having a ::value member. Requires false
#define TMP_ASSERT_NOT(PRED) static_assert(!TMP_GET_TYPE(PRED)::value, #PRED)
// Assert the the passed types are the same
#define TMP_ASSERT_SAME(LHS, RHS)                                                                  \
    static_assert(std::is_same<TMP_GET_TYPE(LHS), TMP_GET_TYPE(RHS)>::value, #LHS "!=" #RHS)

using list1 = tmp::typelist<int, double>;
using list2 = tmp::typelist<float>;
using list3 = tmp::typelist<double>;
using list4 = tmp::typelist<float, double, double>;
using list5 = tmp::typelist<double, float, double>;
using list6 = tmp::typelist<double, double, float>;
using empty = tmp::typelist<>;

TMP_ASSERT_SAME((tmp::concat_t<list1, empty>), (list1));
TMP_ASSERT_SAME((tmp::concat_t<list1, list1>), (tmp::typelist<int, double, int, double>));
TMP_ASSERT_SAME((tmp::concat_t<list1, list3>), (tmp::typelist<int, double, double>));
TMP_ASSERT_SAME((tmp::concat_t<list1, list2>), (tmp::typelist<int, double, float>));
TMP_ASSERT_SAME((tmp::concat_t<list1, list3>), (tmp::typelist<int, double, double>));

TMP_ASSERT((tmp::contains<list1, int>));
TMP_ASSERT((tmp::contains<list1, double>));
TMP_ASSERT_NOT((tmp::contains<list1, float>));
TMP_ASSERT_NOT((tmp::contains<list2, int>));
TMP_ASSERT((tmp::contains<list2, float>));
TMP_ASSERT_NOT((tmp::contains<empty, int>));

TMP_ASSERT_SAME((tmp::apply_t<list1, std::tuple>), (std::tuple<int, double>));
TMP_ASSERT_SAME((tmp::apply_t<list2, std::tuple>), (std::tuple<float>));
TMP_ASSERT_SAME((tmp::apply_t<empty, std::tuple>), (std::tuple<>));

template <typename T>
struct make_not_double
{
    using type = T;
};
template <>
struct make_not_double<double>
{
    using type = float;
};

TMP_ASSERT_SAME((tmp::transform_t<list1, make_not_double>), (tmp::typelist<int, float>));
TMP_ASSERT_SAME((tmp::transform_t<list2, make_not_double>), (tmp::typelist<float>));
TMP_ASSERT_SAME((tmp::transform_t<list3, make_not_double>), (tmp::typelist<float>));
TMP_ASSERT_SAME((tmp::transform_t<empty, make_not_double>), (tmp::typelist<>));

template <typename T>
struct is_not_double : std::true_type
{
};
template <>
struct is_not_double<double> : std::false_type
{
};

template <typename T, typename = void>
struct sfinae_user : std::false_type
{
};
template <typename T>
struct sfinae_user<T, tmp::void_t<typename T::type>> : std::true_type
{
};

TMP_ASSERT_SAME((tmp::filter_t<list1, is_not_double>), (tmp::typelist<int>));
TMP_ASSERT_SAME((tmp::filter_t<list2, is_not_double>), (tmp::typelist<float>));
TMP_ASSERT_SAME((tmp::filter_t<list3, is_not_double>), (tmp::typelist<>));
TMP_ASSERT_SAME((tmp::filter_t<list4, is_not_double>), (tmp::typelist<float>));
TMP_ASSERT_SAME((tmp::filter_t<list5, is_not_double>), (tmp::typelist<float>));
TMP_ASSERT_SAME((tmp::filter_t<list6, is_not_double>), (tmp::typelist<float>));
TMP_ASSERT_SAME((tmp::filter_t<empty, is_not_double>), (tmp::typelist<>));
// Common filters might use sfinae/detection idiom. Make sure we support this directly
TMP_ASSERT_SAME((tmp::filter_t<tmp::typelist<std::true_type, float, std::false_type>, sfinae_user>),
                (tmp::typelist<std::true_type, std::false_type>));

struct print_type
{
    std::string txt;
    void operator()(int)
    {
        txt += "int,";
    }
    void operator()(unsigned)
    {
        txt += "unsigned,";
    }
    template <typename T>
    void operator()(T)
    {
        txt += "T,";
    }
};

TEST_CASE("foreach", "[tmp]")
{
    const std::tuple<int, unsigned, int, float> values{ 1, 1, 1, 1 };

    print_type visitor;
    SECTION("Visit full sequence")
    {
        tmp::foreach (values, visitor);
        REQUIRE(visitor.txt == "int,unsigned,int,T,");
    }

    SECTION("Visit single sequence")
    {
        tmp::foreach (std::make_tuple(1), visitor);
        REQUIRE(visitor.txt == "int,");
    }

    SECTION("Visit empty sequence")
    {
        tmp::foreach (std::tuple<>{}, visitor);
        REQUIRE(visitor.txt == "");
    }

    SECTION("Modify sequence")
    {
        auto values2 = values;
        tmp::foreach (values2, [](auto& val) { ++val; });
        decltype(values2) expected{ 2, 2, 2, 2 };
        REQUIRE(values2 == expected);
    }
}
