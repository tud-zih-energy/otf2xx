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

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <otf2xx/otf2.hpp>

TEST_CASE("test metric events")
{
    namespace def = otf2::definition;
    namespace common = otf2::common;

    def::container<def::string> strings;
    strings.add_definition({ 0, "0" });
    strings.add_definition({ 1, "1" });
    strings.add_definition({ 2, "2" });
    strings.add_definition({ 3, "3" });
    strings.add_definition({ 4, "4" });
    strings.add_definition({ 5, "5" });
    strings.add_definition({ 6, "6" });

    def::system_tree_node root_node(0, strings[0], strings[1]);

    def::location_group lg(0, strings[2], def::location_group::location_group_type::process,
                           root_node);

    def::location location(0, strings[3], lg, def::location::location_type::cpu_thread);

    def::metric_class mClass(0, common::metric_occurence::strict, common::recorder_kind::abstract);
    def::metric_instance mInstance(0, mClass, location, location);
    def::metric_member mMember(0, strings[4], strings[5], common::metric_type::other,
                               common::metric_mode::accumulated_point, common::type::uint8,
                               common::base_type::binary, 0, strings[6]);
    mClass.add_member(mMember);
    REQUIRE(mInstance.metric_class() == mClass);

    otf2::event::metric ev(otf2::chrono::genesis(), mInstance);

    REQUIRE(!std::holds_alternative<otf2::definition::metric_class>(ev.metric_def()));
    REQUIRE(std::holds_alternative<otf2::definition::metric_instance>(ev.metric_def()));
    REQUIRE(std::get<otf2::definition::metric_class>(ev.metric_def()) == mInstance);
}
