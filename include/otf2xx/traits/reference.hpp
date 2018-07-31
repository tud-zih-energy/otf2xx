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

#ifndef INCLUDE_OTF2XX_TRAITS_REFERENCE_HPP
#define INCLUDE_OTF2XX_TRAITS_REFERENCE_HPP

#include <otf2xx/definition/fwd.hpp>
#include <otf2xx/traits/traits.hpp>

#include <type_traits>

#include <cstdint>

namespace otf2
{
namespace traits
{

    template <typename Type>
    struct reference_type;

    // Type traits for Definitions
    template <>
    struct reference_type<definition::location> : identity<std::uint64_t>
    {
    };

    template <>
    struct reference_type<definition::location_group> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::system_tree_node> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::string> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::detail::group_base> : identity<std::uint32_t>
    {
    };

    template <typename Def, otf2::common::group_type Type>
    struct reference_type<definition::group<Def, Type>>
    : reference_type<definition::detail::group_base>
    {
    };

    template <>
    struct reference_type<definition::comm> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::attribute> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::parameter> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::region> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::calling_context> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::source_code_location> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::interrupt_generator> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::detail::metric_base> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::marker> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::metric_class>
    : reference_type<definition::detail::metric_base>
    {
    };

    template <>
    struct reference_type<definition::metric_instance>
    : reference_type<definition::detail::metric_base>
    {
    };

    template <>
    struct reference_type<definition::metric_member> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::io_file> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::io_regular_file> : reference_type<definition::io_file>
    {
    };

    template <>
    struct reference_type<definition::io_directory> : reference_type<definition::io_file>
    {
    };

    template <>
    struct reference_type<definition::io_handle> : identity<std::uint32_t>
    {
    };

    template <>
    struct reference_type<definition::io_paradigm> : identity<std::uint8_t>
    {
    };

    template <typename Definition>
    struct reference_type<definition::property<Definition>> : identity<std::uint32_t>
    {
    };

    template <typename T, typename... Args>
    struct reference_type_var : reference_type<T>
    {
        static_assert(std::is_same<typename reference_type<T>::type,
                                   typename reference_type_var<Args...>::type>::value,
                      "Can only use this for compatible definitions");
    };

    template <typename T>
    struct reference_type_var<T> : reference_type<T>
    {
    };

} // namespace traits
} // namespace otf2

#endif // INCLUDE_OTF2XX_TRAITS_REFERENCE_HPP
