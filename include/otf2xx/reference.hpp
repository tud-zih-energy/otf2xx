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

/*
 * otf2xx - reference.hpp
 *
 * Copyright (c) 2013 TU Dresden
 *
 * All rights reserved.
 *
 * author: Mario Bielert <mario.bielert@tu-dresden.de>
 *
 */

#ifndef INCLUDE_OTF2XX_REFERENCE_HPP
#define INCLUDE_OTF2XX_REFERENCE_HPP

#include <otf2xx/definition/fwd.hpp>
#include <otf2xx/traits/reference.hpp>

namespace otf2
{

template <class T_Definition>
struct reference_map;

/**
 * @brief represents a reference number for definitions
 *
 * For each definition should be an own reference type, so the address space is seperated in a
 * typesafe manner.
 *
 * \tparam T_Definition Definition this reference should refer to
 */
template <class T_Definition>
using reference = typename reference_map<T_Definition>::type;

namespace detail
{
    /**
     * @brief represents a reference number for definitions
     *
     * For each definition should be an own reference type, so the address space is seperated in a
     * typesafe manner.
     *
     * \tparam T_Tag Used to seperate address spaces for different definitions
     */
    template <typename T_Tag>
    class reference
    {
    public:
        /**
         * @brief Tag type used for this reference number space
         */
        typedef T_Tag Tag;
        /**
         * @brief ref_type the underlying type of reference numbers
         *
         * Mostly uint64_t or uint32_t
         */
        typedef typename traits::reference_type<Tag>::type ref_type;

        reference() = delete;

        /**
         * @brief construct by value
         * @param ref the number
         */
        reference(ref_type ref) : handle(ref)
        {
        }

        /**
         * @brief returns the reference number
         * @return the reference number
         */
        ref_type get() const
        {
            return handle;
        }

        ~reference() = default;

        /**
         * @brief returns if the number equals to OTF2_UNDEFINED_UINT64
         * @return true or false
         */
        bool is_undefined() const
        {
            return handle == undefined();
        }

        /**
         * @brief operator ref_type
         *
         * implicitly convertible to ref_type
         *
         */
        operator ref_type() const
        {
            return handle;
        }

        /**
         * @brief returns the undefined representing number
         * @return OTF2_UNDEFINED_UINT64
         */
        template <typename as_type = ref_type>
        static ref_type undefined()
        {
            return static_cast<as_type>(-1);
        }

    protected:
        ref_type handle;
    };
}

template <class T_Definition>
struct reference_map
{
    using type = detail::reference<T_Definition>;
};

template <typename T, otf2::common::group_type Type>
struct reference_map<definition::group<T, Type>>
{
    using type = detail::reference<definition::detail::group_base>;
};

template <>
struct reference_map<definition::metric_class>
{
    using type = detail::reference<definition::detail::metric_base>;
};

template <>
struct reference_map<definition::metric_instance>
{
    using type = detail::reference<definition::detail::metric_base>;
};

template <>
struct reference_map<definition::io_directory>
{
    using type = detail::reference<definition::io_file>;
};

template <>
struct reference_map<definition::io_regular_file>
{
    using type = detail::reference<definition::io_file>;
};

template <typename T>
struct reference_map<definition::property<T>>
{
    using type = detail::reference<T>;
};

template <>
struct reference_map<definition::io_pre_created_handle_state>
{
    using type = detail::reference<definition::io_handle>;
};

} // namespace otf2

#endif // INCLUDE_OTF2XX_REFERENCE_HPP
