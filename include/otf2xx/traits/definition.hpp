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

#ifndef INCLUDE_OTF2XX_TRAITS_DEFINITION_HPP
#define INCLUDE_OTF2XX_TRAITS_DEFINITION_HPP

#include <otf2xx/definition/pre_fwd.hpp>
#include <otf2xx/traits/traits.hpp>
#include <type_traits>

namespace otf2
{
namespace traits
{

    template <typename Type>
    struct is_definition : std::false_type
    {
    };

    template <>
    struct is_definition<otf2::definition::attribute> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::comm> : std::true_type
    {
    };

    template <typename T, otf2::common::group_type GroupType>
    struct is_definition<otf2::definition::group<T, GroupType>> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::location> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::location_group> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::parameter> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::region> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::string> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::system_tree_node> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::metric_class> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::metric_instance> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::metric_member> : std::true_type
    {
    };

    template <typename Definition>
    struct is_definition<otf2::definition::property<Definition>> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::source_code_location> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::calling_context> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::interrupt_generator> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::mapping_table> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::marker> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::io_file> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::io_regular_file> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::io_directory> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::io_handle> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::io_paradigm> : std::true_type
    {
    };

    template <>
    struct is_definition<otf2::definition::io_pre_created_handle_state> : std::true_type
    {
    };
} // namespace traits
} // namespace otf2

#endif // INCLUDE_OTF2XX_TRAITS_DEFINITION_HPP
