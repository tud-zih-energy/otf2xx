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

#ifndef INCLUDE_OTF2XX_DEFINITIONS_DETAIL_IO_HANDLE_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_DETAIL_IO_HANDLE_HPP

#include <otf2xx/exception.hpp>

#include <otf2xx/common.hpp>
#include <otf2xx/fwd.hpp>
#include <otf2xx/reference.hpp>

#include <otf2xx/definition/detail/impl_base.hpp>
#include <otf2xx/definition/detail/owning_ptr.hpp>

#include <otf2xx/definition/string.hpp>
#include <otf2xx/definition/io_file.hpp>
#include <otf2xx/definition/comm.hpp>

namespace otf2
{
namespace definition
{
    namespace detail
    {

        class io_handle_impl : public impl_base<io_handle_impl>
        {
        public:
            using io_handle_flag_type = otf2::common::io_handle_flag_type;

            io_handle_impl(otf2::reference<otf2::definition::io_handle> ref,
                           const otf2::definition::string& name,
                           const otf2::definition::io_file& file,
                           const otf2::definition::io_paradigm& paradigm,
                           io_handle_flag_type handle_flag,
                           const otf2::definition::comm& comm,
                           io_handle_impl* parent,
                           int retain_count = 0)
            : impl_base(retain_count), ref_(ref), name_(name), file_(file), paradigm_(paradigm),
              io_handle_flag_(handle_flag), comm_(comm), parent_(parent)
            {
            }

            io_handle_impl(otf2::reference<otf2::definition::io_handle> ref,
                           const otf2::definition::string& name,
                           const otf2::definition::io_file& file,
                           const otf2::definition::io_paradigm& paradigm,
                           io_handle_flag_type handle_flag,
                           const otf2::definition::comm& comm,
                           int retain_count = 0)
            : impl_base(retain_count), ref_(ref), name_(name), file_(file), paradigm_(paradigm),
              io_handle_flag_(handle_flag), comm_(comm), parent_(nullptr)
            {
            }

            // no implicit copy, see duplicate()
            io_handle_impl(const io_handle_impl&) = delete;
            io_handle_impl& operator=(const io_file_impl&) = delete;

            io_handle_impl(io_handle_impl&&) = default;
            io_handle_impl& operator=(io_handle_impl&&) = default;

            static io_handle_impl* undefined()
            {
                static io_handle_impl undef(
                    otf2::reference<io_handle>::undefined(),
                    otf2::definition::string::undefined(),
                    otf2::definition::io_file::undefined(),
                    otf2::definition::io_paradigm::undefined(),
                    io_handle_flag_type::none,
                    otf2::definition::comm::undefined());

                return &undef;
            }

            otf2::reference<io_handle> ref() const
            {
                return ref_;
            }

            const otf2::definition::string& name() const
            {
                return name_;
            }

            const otf2::definition::io_file& file() const
            {
                return file_;
            }

            const otf2::definition::io_paradigm& paradigm() const
            {
                return paradigm_;
            }

            io_handle_flag_type io_handle_flag() const
            {
                return io_handle_flag_;
            }

            const otf2::definition::comm& comm() const
            {
                return comm_;
            }

            bool has_parent() const
            {
                return parent_.get() != nullptr;
            }

            io_handle_impl* parent() const
            {
                if (!has_parent())
                {
                    make_exception("The io handle '", name().str(),
                                   "' hasn't got a parent.");
                }

                return parent_.get();
            }

        private:
            otf2::reference<io_handle> ref_;
            otf2::definition::string name_;
            otf2::definition::io_file file_;
            otf2::definition::io_paradigm paradigm_;
            io_handle_flag_type io_handle_flag_;
            otf2::definition::comm comm_;
            owning_ptr<io_handle_impl> parent_;
        };
    }
}
} // naespace otf2::definition::detail

#endif // INCLUDE_OTF2XX_DEFINITIONS_DETAIL_IO_HANDLE_HPP
