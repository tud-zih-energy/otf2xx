/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2019, Technische Universität Dresden, Germany
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

#ifndef INCLUDE_OTF2XX_DEFINITIONS_CALL_SITE_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_CALL_SITE_HPP

#include <otf2xx/common.hpp>
#include <otf2xx/fwd.hpp>
#include <otf2xx/reference.hpp>

#include <otf2xx/definition/region.hpp>
#include <otf2xx/definition/string.hpp>

#include <otf2xx/definition/detail/call_site_impl.hpp>
#include <otf2xx/definition/detail/referable_base.hpp>

#include <memory>

namespace otf2
{
namespace definition
{

    /**
     * \brief class for representing a call site definition
     */
    class call_site : public detail::referable_base<call_site, detail::call_site_impl>
    {
        using base = detail::referable_base<call_site, detail::call_site_impl>;
        using base::base;

    public:
        call_site(reference_type ref, const otf2::definition::string& source_file,
                  uint32_t line_number, const otf2::definition::region& entered_region,
                  const otf2::definition::region& left_region)
        : base(ref, new impl_type(source_file, line_number, entered_region, left_region))
        {
        }

        call_site() = default;

        /**
         * \brief returns the region which was called.
         * \returns otf2::definition::region
         */
        const otf2::definition::region& entered_region() const
        {
            assert(this->is_valid());
            return data_->entered_region();
        }

        /**
         * \brief returns the region which made the call.
         * \returns otf2::definition::region
         */
        const otf2::definition::region& left_region() const
        {
            assert(this->is_valid());
            return data_->left_region();
        }

        /**
         * \brief returns the source file where this call was made
         * \returns otf2::definition::source_code_location
         */
        const otf2::definition::string& source_file() const
        {
            assert(this->is_valid());
            return data_->source_file();
        }

        /**
         * \brief returns the line number in the source file where this call was made
         * \returns uint32_t
         */
        std::uint32_t line_number() const
        {
            assert(this->is_valid());
            return data_->line_number();
        }
    };
} // namespace definition
} // namespace otf2

#endif // INCLUDE_OTF2XX_DEFINITIONS_CALL_SITE_HPP
