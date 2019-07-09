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

#ifndef INCLUDE_OTF2XX_WRITER_DETAIL_CALLBACKS_COLLECTIVE_HPP
#define INCLUDE_OTF2XX_WRITER_DETAIL_CALLBACKS_COLLECTIVE_HPP

#include <otf2xx/exception.hpp>

#include <otf2xx/writer/archive.hpp>

#include <otf2/OTF2_Archive.h>
#include <otf2/OTF2_Callbacks.h>
#include <otf2/OTF2_GeneralDefinitions.h>

#include <mpi.h>

#include <cstdlib>

namespace otf2
{
namespace writer
{
    namespace detail
    {
        namespace callbacks
        {
            namespace collective
            {

                inline MPI_Datatype runtime_type_cast(OTF2_Type type)
                {
                    switch (type)
                    {
                    case OTF2_TYPE_FLOAT:
                        return MPI_FLOAT;
                        break;

                    case OTF2_TYPE_DOUBLE:
                        return MPI_DOUBLE;
                        break;

                    case OTF2_TYPE_INT8:
                        return MPI_INT8_T;
                        break;

                    case OTF2_TYPE_INT16:
                        return MPI_INT16_T;
                        break;

                    case OTF2_TYPE_INT32:
                        return MPI_INT32_T;
                        break;

                    case OTF2_TYPE_INT64:
                        return MPI_INT64_T;
                        break;

                    case OTF2_TYPE_UINT8:
                        return MPI_UINT8_T;
                        break;

                    case OTF2_TYPE_UINT16:
                        return MPI_UINT16_T;
                        break;

                    case OTF2_TYPE_UINT32:
                        return MPI_UINT32_T;
                        break;

                    case OTF2_TYPE_UINT64:
                        return MPI_UINT64_T;
                        break;

                    default:
                        throw exception("Unexpected or unknown type given to otf2 collective "
                                        "broadcast callback");
                    }
                }

                inline OTF2_CallbackCode
                barrier(void* userData, __attribute__((unused)) OTF2_CollectiveContext* commContext)
                {
                    otf2::writer::archive& ar = *static_cast<otf2::writer::archive*>(userData);

                    ar.comm().barrier();

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                inline OTF2_CallbackCode broadcast(void* userData,
                                                   __attribute__((unused))
                                                   OTF2_CollectiveContext* commContext,
                                                   void* data, uint32_t numberElements,
                                                   OTF2_Type type, uint32_t root)
                {

                    otf2::writer::archive& ar = *static_cast<otf2::writer::archive*>(userData);

                    MPI_Bcast(data, numberElements, runtime_type_cast(type), root, ar.comm());

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                inline OTF2_CallbackCode
                gather(void* userData, __attribute__((unused)) OTF2_CollectiveContext* commContext,
                       const void* inData, void* outData, uint32_t numberElements, OTF2_Type type,
                       uint32_t root)
                {
                    otf2::writer::archive& ar = *static_cast<otf2::writer::archive*>(userData);
                    MPI_Gather(const_cast<void*>(inData), numberElements, runtime_type_cast(type),
                               outData, numberElements, runtime_type_cast(type), root, ar.comm());

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                inline OTF2_CallbackCode
                gatherv(void* userData, __attribute__((unused)) OTF2_CollectiveContext* commContext,
                        const void* inData, uint32_t inElements, void* outData,
                        const uint32_t* outElements, OTF2_Type type, uint32_t root)
                {
                    otf2::writer::archive& ar = *static_cast<otf2::writer::archive*>(userData);

                    int* displs = NULL;
                    int size = ar.comm().size();
                    int rank = ar.comm().rank();

                    if (rank == static_cast<int>(root))
                    {
                        displs = static_cast<int*>(calloc(size, sizeof(*displs)));
                        int displ = 0;
                        for (int i = 0; i < size; i++)
                        {
                            displs[i] = displ;
                            displ += outElements[i];
                        }
                    }

                    MPI_Gatherv(const_cast<void*>(inData), inElements, runtime_type_cast(type),
                                outData,
                                const_cast<int*>(reinterpret_cast<const int*>(outElements)), displs,
                                runtime_type_cast(type), root, ar.comm());

                    free(displs);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                inline OTF2_CallbackCode get_rank(void* userData,
                                                  __attribute__((unused))
                                                  OTF2_CollectiveContext* commContext,
                                                  std::uint32_t* rank)
                {
                    otf2::writer::archive& ar = *static_cast<otf2::writer::archive*>(userData);

                    int mpi_rank = ar.comm().rank();

                    *rank = static_cast<std::uint32_t>(mpi_rank);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                inline OTF2_CallbackCode get_size(void* userData,
                                                  __attribute__((unused))
                                                  OTF2_CollectiveContext* commContext,
                                                  std::uint32_t* size)
                {
                    otf2::writer::archive& ar = *static_cast<otf2::writer::archive*>(userData);

                    int mpi_size = ar.comm().size();

                    *size = static_cast<std::uint32_t>(mpi_size);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                inline OTF2_CallbackCode
                scatter(void* userData, __attribute__((unused)) OTF2_CollectiveContext* commContext,
                        const void* inData, void* outData, uint32_t numberElements, OTF2_Type type,
                        uint32_t root)
                {
                    otf2::writer::archive& ar = *static_cast<otf2::writer::archive*>(userData);

                    MPI_Scatter(const_cast<void*>(inData), numberElements, runtime_type_cast(type),
                                outData, numberElements, runtime_type_cast(type), root, ar.comm());

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                inline OTF2_CallbackCode scatterv(void* userData,
                                                  __attribute__((unused))
                                                  OTF2_CollectiveContext* commContext,
                                                  const void* inData, const uint32_t* inElements,
                                                  void* outData, uint32_t outElements,
                                                  OTF2_Type type, uint32_t root)
                {
                    otf2::writer::archive& ar = *static_cast<otf2::writer::archive*>(userData);

                    int* displs = NULL;

                    int size = ar.comm().size();
                    int rank = ar.comm().rank();

                    if (rank == static_cast<int>(root))
                    {
                        displs = static_cast<int*>(calloc(size, sizeof(*displs)));
                        int displ = 0;
                        for (int i = 0; i < size; i++)
                        {
                            displs[i] = displ;
                            displ += inElements[i];
                        }
                    }

                    MPI_Scatterv(const_cast<void*>(inData),
                                 const_cast<int*>(reinterpret_cast<const int*>(inElements)), displs,
                                 runtime_type_cast(type), outData, outElements,
                                 runtime_type_cast(type), root, ar.comm());

                    free(displs);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }
            } // namespace collective
        }     // namespace callbacks
    }         // namespace detail
} // namespace writer
} // namespace otf2

#endif // INCLUDE_OTF2XX_WRITER_DETAIL_CALLBACKS_COLLECTIVE_HPP
