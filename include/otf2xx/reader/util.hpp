/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2020, Technische Universität Dresden, Germany
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

#pragma once

#include <otf2xx/exception.hpp>

#include <otf2/OTF2_GlobalDefReader.h>
#include <otf2/OTF2_GlobalEvtReader.h>
#include <otf2/OTF2_Reader.h>

#include <memory>

namespace otf2
{
namespace reader
{
    class definition_files
    {
        OTF2_Reader* rdr_;
        bool are_open_ = false;

    public:
        definition_files(OTF2_Reader* rdr, bool open_now = false) : rdr_(rdr)
        {
            if (open_now)
            {
                open();
            }
        }

        void open()
        {
            if (!are_open())
            {
                are_open_ = OTF2_Reader_OpenDefFiles(rdr_);
            }
        }

        void close()
        {
            check(OTF2_Reader_CloseDefFiles(rdr_), "Couldn't close definition files of the trace.");
            are_open_ = false;
        }

        bool are_open() const
        {
            return are_open_;
        }

        ~definition_files()
        {
            if (are_open())
            {
                close();
            }
        }
    };

    class event_files
    {
        OTF2_Reader* rdr_;
        bool are_open_ = false;

    public:
        event_files(OTF2_Reader* rdr, bool open_now = false) : rdr_(rdr)
        {
            if (open_now)
            {
                open();
            }
        }

        void open()
        {
            if (!are_open())
            {
                check(OTF2_Reader_OpenEvtFiles(rdr_), "Couldn't open event files of the trace.");
            }
        }

        void close()
        {
            check(OTF2_Reader_CloseEvtFiles(rdr_), "Couldn't close event files of the trace.");
            are_open_ = false;
        }

        bool are_open() const
        {
            return are_open_;
        }

        ~event_files()
        {
            if (are_open())
            {
                close();
            }
        }
    };
} // namespace reader
} // namespace otf2
