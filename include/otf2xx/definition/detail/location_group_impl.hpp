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

#ifndef INCLUDE_OTF2XX_DEFINITIONS_DETAIL_LOCATION_GROUP_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_DETAIL_LOCATION_GROUP_HPP

#include <otf2xx/common.hpp>
#include <otf2xx/fwd.hpp>
#include <otf2xx/reference.hpp>

#include <otf2xx/definition/detail/impl_base.hpp>

#include <otf2xx/definition/string.hpp>
#include <otf2xx/definition/system_tree_node.hpp>

namespace otf2
{
namespace definition
{
    namespace detail
    {

        class location_group_impl : public impl_base<location_group_impl>
        {
        public:
            typedef otf2::common::location_group_type location_group_type;

            location_group_impl(reference<location_group> ref, const otf2::definition::string& name,
                                location_group_type type,
                                const otf2::definition::system_tree_node& stm,
                                std::int64_t retain_count = 0)
            : impl_base(retain_count), ref_(ref), name_(name), type_(type), stm_(stm)
            {
            }

            // no implicit copy allowed, see duplicate()
            location_group_impl(const location_group_impl&) = delete;
            location_group_impl& operator=(const location_group_impl&) = delete;

            location_group_impl(location_group_impl&&) = default;
            location_group_impl& operator=(location_group_impl&&) = default;

            static location_group_impl* undefined()
            {
                static location_group_impl undef(reference<location_group>::undefined(),
                                                 string::undefined(), location_group_type::unknown,
                                                 system_tree_node::undefined(), 1);
                return &undef;
            }

            reference<location_group> ref() const
            {
                return ref_;
            }

            const otf2::definition::string& name() const
            {
                return name_;
            }

            location_group_type type() const
            {
                return type_;
            }

            const otf2::definition::system_tree_node& parent() const
            {
                return stm_;
            }

        private:
            reference<location_group> ref_;
            otf2::definition::string name_;
            location_group_type type_;
            otf2::definition::system_tree_node stm_;
        };
    }
}
} // namespace otf2::definition::detail

#endif // INCLUDE_OTF2XX_DEFINITIONS_DETAIL_LOCATION_GROUP_HPP
