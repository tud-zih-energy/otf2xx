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

#ifndef INCLUDE_OTF2XX_WRITER_FWD_HPP
#define INCLUDE_OTF2XX_WRITER_FWD_HPP

#include <otf2/OTF2_Callbacks.h>

#include <otf2xx/definition/fwd.hpp>

namespace otf2
{
namespace writer
{

    namespace detail
    {

        OTF2_FlushType pre_flush(void* userData, OTF2_FileType fileType, OTF2_LocationRef location,
                                 void* callerData, bool final);

        OTF2_TimeStamp post_flush(void* userData, OTF2_FileType fileType,
                                  OTF2_LocationRef location);

        namespace callbacks
        {
            namespace collective
            {

                OTF2_CallbackCode barrier(void* userData, OTF2_CollectiveContext* commContext);
                OTF2_CallbackCode broadcast(void* userData, OTF2_CollectiveContext* commContext,
                                            void* data, uint32_t numberElements, OTF2_Type type,
                                            uint32_t root);
                OTF2_CallbackCode gather(void* userData, OTF2_CollectiveContext* commContext,
                                         const void* inData, void* outData, uint32_t numberElements,
                                         OTF2_Type type, uint32_t root);
                OTF2_CallbackCode gatherv(void* userData, OTF2_CollectiveContext* commContext,
                                          const void* inData, uint32_t inElements, void* outData,
                                          const uint32_t* outElements, OTF2_Type type,
                                          uint32_t root);
                OTF2_CallbackCode get_rank(void* userData, OTF2_CollectiveContext* commContext,
                                           uint32_t* rank);
                OTF2_CallbackCode get_size(void* userData, OTF2_CollectiveContext* commContext,
                                           uint32_t* size);
                OTF2_CallbackCode scatter(void* userData, OTF2_CollectiveContext* commContext,
                                          const void* inData, void* outData,
                                          uint32_t numberElements, OTF2_Type type, uint32_t root);
                OTF2_CallbackCode scatterv(void* userData, OTF2_CollectiveContext* commContext,
                                           const void* inData, const uint32_t* inElements,
                                           void* outData, uint32_t outElements, OTF2_Type type,
                                           uint32_t root);
            }
        } // namespace callbacks::collective
    }     // namespace detail

    class archive;
    class global;
    class local;

    template <typename Record>
    local& operator<<(local& wrt, Record evt);

    template <typename Definition>
    local& operator<<(local& wrt, const otf2::definition::container<Definition>& c);

    template <typename Definition>
    global& operator<<(global& wrt, Definition&& def);

    template <typename Anything, typename>
    global& operator<<(archive& ar, Anything&& any);
}
} // namespace otf2::writer

#endif // INCLUDE_OTF2XX_WRITER_FWD_HPP
