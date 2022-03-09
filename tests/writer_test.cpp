/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2017, Technische Universität Dresden, Germany
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

#include <otf2xx/otf2.hpp>

#include <chrono>
#include <iostream>

std::chrono::high_resolution_clock::time_point get_time(void)
{
    static std::size_t count = 0;
    return std::chrono::high_resolution_clock::time_point(
        std::chrono::high_resolution_clock::duration(count++));
}

int main()
{
    otf2::writer::archive ar("otf2xx_writer_trace", "traces");

    ar.set_post_flush_callback(
        [](otf2::reference<otf2::definition::location> loc)
        {
            std::cout << "Buffer flush event on location #" << loc << std::endl;
            return otf2::chrono::convert_time_point(get_time());
        });

    otf2::definition::container<otf2::definition::string> strings;

    strings.add_definition({ 0, "MyHost" });
    strings.add_definition({ 1, "node" });
    strings.add_definition({ 2, "Master Process" });
    strings.add_definition({ 3, "MainThread" });
    strings.add_definition({ 4, "MyFunction" });
    strings.add_definition({ 5, "Alternative function name (e.g. mangled one)" });
    strings.add_definition({ 6, "Computes something" });
    strings.add_definition({ 7, "" });

    otf2::definition::system_tree_node root_node(0, strings[0], strings[1]);

    otf2::definition::location_group lg(
        0, strings[2], otf2::definition::location_group::location_group_type::process, root_node);

    otf2::definition::location location(0, strings[3], lg,
                                        otf2::definition::location::location_type::cpu_thread);

    otf2::definition::region region(23, strings[4], strings[5], strings[6],
                                    otf2::definition::region::role_type::function,
                                    otf2::definition::region::paradigm_type::user,
                                    otf2::definition::region::flags_type::none, strings[7], 0, 0);

    ar << strings;

    ar << root_node << lg << region << location;

    ar << otf2::definition::clock_properties(otf2::chrono::ticks(1e9), otf2::chrono::ticks(0),
                                             otf2::chrono::ticks(19));

    auto& arl = ar(location);

    for (int i = 0; i < 10; ++i)
        arl << otf2::event::enter(otf2::chrono::convert_time_point(get_time()), region);

    for (int i = 0; i < 10; ++i)
        arl << otf2::event::leave(otf2::chrono::convert_time_point(get_time()), region);
}
