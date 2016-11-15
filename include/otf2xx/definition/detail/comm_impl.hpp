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

#ifndef INCLUDE_OTF2XX_DEFINITIONS_DETAIL_COMM_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_DETAIL_COMM_HPP

#include <otf2xx/exception.hpp>

#include <otf2xx/fwd.hpp>
#include <otf2xx/reference.hpp>

#include <otf2xx/definition/group.hpp>
#include <otf2xx/definition/string.hpp>
//#include <otf2xx/definition/comm.hpp>

#include <sstream>

namespace otf2
{
namespace definition
{
    namespace detail
    {

        class comm_impl
        {
        public:
            comm_impl(reference<comm> ref, const otf2::definition::string& name,
                      const otf2::definition::comm_group& group, std::shared_ptr<comm_impl> parent)
            : ref_(ref), name_(name), group_(group), self_group_(nullptr), parent_(parent)
            {
            }

            comm_impl(reference<comm> ref, const otf2::definition::string& name,
                      const otf2::definition::comm_group& group)
            : ref_(ref), name_(name), group_(group), self_group_(nullptr), parent_(nullptr)
            {
            }

            comm_impl(reference<comm> ref, const otf2::definition::string& name,
                      const otf2::definition::comm_self_group& group,
                      std::shared_ptr<comm_impl> parent)
            : ref_(ref), name_(name), group_(nullptr), self_group_(group), parent_(parent)
            {
            }

            comm_impl(reference<comm> ref, const otf2::definition::string& name,
                      const otf2::definition::comm_self_group& group)
            : ref_(ref), name_(name), group_(nullptr), self_group_(group), parent_(nullptr)
            {
            }

            // no implicit copy allowed, see duplicate()
            comm_impl(const comm_impl&) = delete;
            comm_impl& operator=(const comm_impl&) = delete;

            comm_impl(comm_impl&&) = default;
            comm_impl& operator=(comm_impl&&) = default;

            static std::shared_ptr<comm_impl> undefined()
            {
                static std::shared_ptr<comm_impl> undef(std::make_shared<comm_impl>(
                    reference<comm>::undefined(), string::undefined(), comm_group::undefined()));
                return undef;
            }

            reference<comm> ref() const
            {
                return ref_;
            }

            const otf2::definition::string& name() const
            {
                return name_;
            }

            const otf2::definition::comm_group& group() const
            {
                if (has_self_group())
                    make_exception("The comm with id ", ref_.get(),
                                   " hasn't got a group. It has a self group.");

                return group_;
            }

            const otf2::definition::comm_self_group& self_group() const
            {
                if (!has_self_group())
                    make_exception("The comm with id ", ref_.get(), " hasn't got a self group");

                return self_group_;
            }

            bool has_self_group() const
            {
                return self_group_.is_valid();
            }

            bool has_parent() const
            {
                return bool(parent_);
            }

            std::shared_ptr<comm_impl> parent() const
            {
                if (!has_parent())
                {
                    std::stringstream msg;
                    msg << "The comm '" << name_.str() << "' hasn't got a parent.";
                    throw exception(msg.str());
                }

                return parent_;
            }

        private:
            reference<comm> ref_;
            otf2::definition::string name_;
            otf2::definition::comm_group group_;
            otf2::definition::comm_self_group self_group_;
            std::shared_ptr<comm_impl> parent_;
        };
    }
}
} // namespace otf2::definition::detail

#endif // INCLUDE_OTF2XX_DEFINITIONS_DETAIL_COMM_HPP
