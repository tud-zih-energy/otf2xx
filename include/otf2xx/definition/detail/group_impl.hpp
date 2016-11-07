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

#ifndef INCLUDE_OTF2XX_DEFINITIONS_DETAIL_GROUP_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_DETAIL_GROUP_HPP

#include <otf2xx/common.hpp>
#include <otf2xx/definition/fwd.hpp>
#include <otf2xx/reference.hpp>

#include <otf2xx/definition/string.hpp>

#include <otf2xx/traits/definition.hpp>

#include <memory>
#include <vector>

namespace otf2
{
namespace definition
{
    namespace detail
    {

        /**
         * Dummy class to have all group templates in the same id space
         */
        class group_base
        {
        };

        template <class MemberType,
                  otf2::common::group_type GroupType = otf2::common::group_type::unknown>
        class group_impl
        {
            static_assert(otf2::traits::is_definition<MemberType>::value,
                          "The MemberType has to be a otf2::definition.");

            typedef std::vector<MemberType> members_type;

        public:
            typedef otf2::common::group_type group_type;
            typedef otf2::common::group_flag_type group_flag_type;
            typedef otf2::common::paradigm_type paradigm_type;
            typedef MemberType value_type;

        public:
            group_impl(otf2::reference<detail::group_base> ref, string name, paradigm_type paradigm,
                       group_flag_type group_flag)
            : ref_(ref), name_(name), paradigm_(paradigm), group_flag_(group_flag)
            {
            }

            // no implicit copy allowed, see duplicate()
            group_impl(const group_impl&) = delete;
            group_impl& operator=(const group_impl&) = delete;

            group_impl(group_impl&&) = default;
            group_impl& operator=(group_impl&&) = default;

            static std::shared_ptr<group_impl> undefined()
            {
                static std::shared_ptr<group_impl> undef(std::make_shared<group_impl>(
                    otf2::reference<detail::group_base>::undefined(), string::undefined(),
                    paradigm_type::unknown, group_flag_type::none));
                return undef;
            }

            otf2::reference<detail::group_base> ref() const
            {
                return ref_;
            }

            string name() const
            {
                return name_;
            }

            group_type type() const
            {
                return GroupType;
            }

            paradigm_type paradigm() const
            {
                return paradigm_;
            }

            group_flag_type group_flag() const
            {
                return group_flag_;
            }

            std::vector<std::uint64_t> members() const
            {
                std::vector<std::uint64_t> result(members_.size());

                for (std::size_t i = 0; i < members_.size(); i++)
                {
                    result[i] = members_[i].ref();
                }

                return result;
            }

            std::size_t size() const
            {
                return members_.size();
            }

            value_type operator[](std::size_t i) const
            {
                return members_[i];
            }

            void add_member(value_type member)
            {
                members_.push_back(member);
            }

        private:
            otf2::reference<detail::group_base> ref_;
            string name_;
            members_type members_;
            paradigm_type paradigm_;
            group_flag_type group_flag_;
        };
    }
}
} // namespace otf2::definition::detail

#endif // INCLUDE_OTF2XX_DEFINITIONS_DETAIL_GROUP_HPP
