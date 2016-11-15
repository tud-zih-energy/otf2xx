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

#ifndef INCLUDE_OTF2XX_DEFINITIONS_DETAIL_PARAMETER_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_DETAIL_PARAMETER_HPP

#include <otf2xx/common.hpp>
#include <otf2xx/fwd.hpp>
#include <otf2xx/reference.hpp>

#include <otf2xx/definition/string.hpp>

#include <memory>

namespace otf2
{
namespace definition
{
    namespace detail
    {

        class parameter_impl
        {
        public:
            typedef otf2::common::parameter_type parameter_type;

            parameter_impl(otf2::reference<parameter> ref, const otf2::definition::string& name,
                           parameter_type type)
            : ref_(ref), name_(name), type_(type)
            {
                //             std::cout << "Created Paramter_impl @" << this << " name: '" << &name
                //             << "' id: " << ref << std::endl;
            }

            ~parameter_impl()
            {
                //             std::cout << "Deleted Paramter_impl @" << this << " name: '" <<
                //             &name_ << "' id: " << ref_ << std::endl;
            }

            // no implicit copy allowed, see duplicate()
            parameter_impl(const parameter_impl&) = delete;
            parameter_impl& operator=(const parameter_impl&) = delete;

            parameter_impl(parameter_impl&&) = delete;
            parameter_impl& operator=(parameter_impl&&) = delete;

            static std::shared_ptr<parameter_impl> undefined()
            {
                static std::shared_ptr<parameter_impl> undef(
                    std::make_shared<parameter_impl>(otf2::reference<parameter>::undefined(),
                                                     string::undefined(), parameter_type::int64));
                return undef;
            }

            const otf2::definition::string& name() const
            {
                return name_;
            }

            parameter_type type() const
            {
                return type_;
            }

            otf2::reference<parameter> ref() const
            {
                return ref_;
            }

        private:
            otf2::reference<parameter> ref_;
            otf2::definition::string name_;
            parameter_type type_;
        };
    }
}
} // namespace otf2::definition::detail

#endif // INCLUDE_OTF2XX_DEFINITIONS_DETAIL_PARAMETER_HPP
