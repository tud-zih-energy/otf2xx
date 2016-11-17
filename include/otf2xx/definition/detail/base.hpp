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

#ifndef INCLUDE_OTF2XX_DEFINITIONS_DETAIL_BASE_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_DETAIL_BASE_HPP

#include <otf2xx/definition/fwd.hpp>
#include <otf2xx/traits/traits.hpp>

#include <cassert>
#include <memory>

namespace otf2
{
namespace definition
{
    namespace detail
    {

        /**
         * \brief CRTP base class for definition references
         *
         * This class is used to implement some common methods, contructors and
         * member for the definition record reference types.
         *
         * This class is implemented using CRTP.
         *
         * This class holds the shared_ptr and some common methods.
         *
         * \tparam Def type of definition record reference type
         * \tparam Impl type of definiotn record implementation type
         */
        template <typename Def, typename Impl>
        class base
        {
        public:
            typedef otf2::reference<typename otf2::traits::reference_param_type<Def>::type>
                reference_type;

            base(const std::shared_ptr<Impl>& data) : data_(data)
            {
            }

            base(std::shared_ptr<Impl>&& data) : data_(std::move(data))
            {
            }

            base() = default;

            base(const base& other) = default;
            base(base&& other) = default;

            base& operator=(const base&) = default;
            base& operator=(base&&) = default;

        public:
            /**
             * \brief Returns the reference number of the definition
             *
             * This number is used by libotf2 to identify a definition record.
             *
             * \returns a reference number
             */
            reference_type ref() const
            {
                assert(is_valid());
                return data_->ref();
            }

            /**
             * \brief Returns a reference to an undefined definition.
             *
             * In most cases undefined means, that the ref() of this definition is -1.
             *
             * \return a definiton object
             */
            static const Def& undefined()
            {
                static Def undef(Impl::undefined());
                return undef;
            }

            /**
             * \brief Returns if the definition object is valid
             *
             * \warning { If this returns false, it's not allowed to call any other
             *          functions on this object }
             *
             * For now, a definition is valid, if the internal pointer isn't
             * set to nullptr.
             *
             * \returns true or false
             */
            bool is_valid() const
            {
                return bool(data_);
            }

            /**
             * \brief operator bool, alias for is_valid()
             */
            explicit operator bool() const
            {
                return is_valid();
            }

            /**
             * \brief Returns the internal pointer
             *
             * \warning { This method isn't part of the public interface of definition
             *          objects. You're disencouraged to relie on it. }
             *
             * \return std::shared_ptr<Impl> to the referenced object
             */
            const std::shared_ptr<Impl>& get() const
            {
                return data_;
            }

            /**
             * \brief Returns the impl object
             *
             * \warning { This method isn't part of the public interface of definition
             *          objects. You're disencouraged to relie on it. }
             *
             * \return const Impl& to the referenced object
             */
            const Impl& data() const
            {
                return *(data_.get());
            }

        protected:
            std::shared_ptr<Impl> data_;
        };

        template <typename Def, typename Impl>
        inline bool operator==(const base<Def, Impl>& a, const base<Def, Impl>& b)
        {
            if (!a.is_valid() || !b.is_valid())
                return false;

            return a.ref() == b.ref();
        }
    }
}
} // namespace otf2::definition::detail

#endif // INCLUDE_OTF2XX_DEFINITIONS_DETAIL_BASE_HPP
