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

#ifndef INCLUDE_OTF2XX_DEFINITIONS_COMM_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_COMM_HPP

#include <otf2xx/exception.hpp>

#include <otf2xx/fwd.hpp>
#include <otf2xx/reference.hpp>

#include <otf2xx/definition/group.hpp>
#include <otf2xx/definition/string.hpp>

#include <otf2xx/definition/detail/base.hpp>
#include <otf2xx/definition/detail/comm_impl.hpp>

#include <sstream>

namespace otf2
{
namespace definition
{

    /**
     * \brief class for representing a comm definition
     */
    class comm : public detail::base<comm>
    {
        typedef typename otf2::traits::definition_impl_type<comm>::type impl_type;
        typedef detail::base<comm> base;

        using base::base;

    public:
        comm(reference<comm> ref, const otf2::definition::string& name,
             const otf2::definition::comm_group& group, const otf2::definition::comm& parent)
        : base(new impl_type(ref, name, group, parent.get()))
        {
        }

        comm(reference<comm> ref, const otf2::definition::string& name,
             const otf2::definition::comm_group& group)
        : base(new impl_type(ref, name, group))
        {
        }

        comm(reference<comm> ref, const otf2::definition::string& name,
             const otf2::definition::comm_self_group& group, comm parent)
        : base(new impl_type(ref, name, group, parent.get()))
        {
        }

        comm(reference<comm> ref, const otf2::definition::string& name,
             const otf2::definition::comm_self_group& group)
        : base(new impl_type(ref, name, group))
        {
        }

        comm() = default;

        /**
         * \brief returns the name of the comm definition as a string definition
         *
         * \returns a string definition containing the name
         *
         */
        const otf2::definition::string& name() const
        {
            assert(this->is_valid());
            return data_->name();
        }

        /**
         * \brief sets the name of the comm definition
         *
         * \param a string definition containing the new name
         *
         */
        void name(const otf2::definition::string& name)
        {
            assert(this->is_valid());
            data_->name() = name;
        }

        /**
         * \brief returns the comm group of this comm
         *
         * \returns a comm group
         * \attention before call, check that there is a comm group
         * \throws otf::exception if thee is no comm group
         */
        const otf2::definition::comm_group& group() const
        {
            assert(this->is_valid());
            return data_->group();
        }

        /**
         * \brief returns the comm self group of this comm
         *
         * \returns a comm self group
         * \attention before call, check that there is a comm self group
         * \throws otf::exception if thee is no comm self group
         */
        const otf2::definition::comm_self_group& self_group() const
        {
            assert(this->is_valid());
            return data_->self_group();
        }

        /**
         * \brief returns if there is a comm self group
         * If return is true, then it has got a comm self group
         * Otherwise it has got a comm group
         * \return bool
         */
        bool has_self_group() const
        {
            assert(this->is_valid());
            return data_->has_self_group();
        }

        /**
         * \brief returns the comm has got a parent
         * \return bool
         */
        bool has_parent() const
        {
            assert(this->is_valid());
            return data_->has_parent();
        }

        /**
         * \brief returns the parent of this comm
         * \return returns a otf::definition::comm, which might not be valid, if the comm hasn't got
         * a parent!
         */
        otf2::definition::comm parent() const
        {
            assert(this->is_valid());
            return data_->parent();
        }
    };

} // namespace otf2::definition

} // namespace otf2

#endif // INCLUDE_OTF2XX_DEFINITIONS_COMM_HPP
