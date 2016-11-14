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

#ifndef INCLUDE_OTF2XX_DEFINITIONS_DETAIL_PROPERTY_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_DETAIL_PROPERTY_HPP

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

        template <typename Definition>
        class property_impl
        {
        public:
            using type_type = otf2::common::type;
            using value_type = OTF2_AttributeValue;

            property_impl(Definition def, string name, type_type type, value_type value)
            : def_(def), name_(name), type_(type), value_(value)
            {
            }

            // no implicit copy allowed, see duplicate()
            property_impl(const property_impl&) = delete;
            property_impl& operator=(const property_impl&) = delete;

            property_impl(property_impl&&) = delete;
            property_impl& operator=(property_impl&&) = delete;

            static std::shared_ptr<property_impl> undefined()
            {
                static std::shared_ptr<property_impl> undef(std::make_shared<property_impl>(
                    Definition::undefined(), string::undefined(), string::undefined()));
                return undef;
            }

            string name() const
            {
                return name_;
            }

            type_type type() const
            {
                return type_;
            }

            value_type value() const
            {
                return value_;
            }

            Definition def() const
            {
                return def_;
            }

            otf2::reference<property<Definition>> ref() const
            {
                return otf2::reference<property<Definition>>::undefined();
            }

        private:
            Definition def_;
            string name_;
            type_type type_;
            value_type value_;
        };
    }
}
} // namespace otf2::definition::detail

#endif // INCLUDE_OTF2XX_DEFINITIONS_DETAIL_PROPERTY_HPP