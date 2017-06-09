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

#include <otf2xx/reader/callback_funcs.hpp>

#include <otf2xx/reader/callback.hpp>
#include <otf2xx/reader/reader.hpp>

#include <otf2xx/definition/definitions.hpp>
#include <otf2xx/event/events.hpp>

#include <otf2xx/chrono/chrono.hpp>

#include <otf2/OTF2_Definitions.h>
#include <otf2/OTF2_GeneralDefinitions.h>

#include <memory>
#include <vector>

namespace otf2
{
namespace reader
{
    namespace detail
    {
        namespace event
        {

            OTF2_CallbackCode buffer_flush(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                           void* userData, OTF2_AttributeList* attributeList,
                                           OTF2_TimeStamp stopTime)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::buffer_flush(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            stopTime - reader->clock_properties().start_time().count()))));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode enter(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                    void* userData, OTF2_AttributeList* attributeList,
                                    OTF2_RegionRef regionID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::enter(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->regions()[regionID]));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode leave(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                    void* userData, OTF2_AttributeList* attributeList,
                                    OTF2_RegionRef regionID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::leave(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->regions()[regionID]));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode measurement(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                          void* userData, OTF2_AttributeList* attributeList,
                                          OTF2_MeasurementMode measurementMode)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::measurement(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        static_cast<otf2::event::measurement::mode_type>(measurementMode)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode metric(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                     void* userData, OTF2_AttributeList* attributeList,
                                     OTF2_MetricRef metric, uint8_t numberOfMetrics,
                                     __attribute__((unused)) const OTF2_Type* typeIDs,
                                     const OTF2_MetricValue* metricValues)
            {
                // typeID parameter is ignored, as it's redundant with metric_member

                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                // WORKAROUND for broken Score-P traces
                if (time < reader->clock_properties().start_time().count())
                {
                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                typedef otf2::event::metric::value_container value_container;

                std::vector<value_container> values(numberOfMetrics);

                // assumes a valid trace file
                if (reader->metric_classes().count(metric))
                {
                    const auto& mc = reader->metric_classes()[metric];

                    for (std::size_t i = 0; i < numberOfMetrics; i++)
                    {
                        values[i].metric = mc[i];
                        values[i].value = metricValues[i];
                    }

                    reader->callback().event(
                        reader->locations()[locationID],
                        otf2::event::metric(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                time - reader->clock_properties().start_time().count())),
                            mc, std::move(values)));
                }
                else
                {
                    assert(reader->metric_instances().count(metric));

                    const auto& mc = reader->metric_instances()[metric].metric_class();

                    for (std::size_t i = 0; i < numberOfMetrics; i++)
                    {
                        values[i].metric = mc[i];
                        values[i].value = metricValues[i];
                    }

                    reader->callback().event(
                        reader->locations()[locationID],
                        otf2::event::metric(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                time - reader->clock_properties().start_time().count())),
                            reader->metric_instances()[metric], std::move(values)));
                }

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_collective_begin(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                   void* userData,
                                                   OTF2_AttributeList* attributeList)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::mpi_collective_begin(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count()))));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_collective_end(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                 void* userData, OTF2_AttributeList* attributeList,
                                                 OTF2_CollectiveOp collectiveOp,
                                                 OTF2_CommRef communicator, uint32_t root,
                                                 uint64_t sizeSent, uint64_t sizeReceived)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::mpi_collective_end(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        static_cast<otf2::event::mpi_collective_end::collective_type>(collectiveOp),
                        reader->comms()[communicator], root, sizeSent, sizeReceived));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_irecv(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                        void* userData, OTF2_AttributeList* attributeList,
                                        uint32_t sender, OTF2_CommRef communicator, uint32_t msgTag,
                                        uint64_t msgLength, uint64_t requestID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::mpi_ireceive(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        sender, reader->comms()[communicator], msgTag, msgLength, requestID));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_irecv_request(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                void* userData, OTF2_AttributeList* attributeList,
                                                uint64_t requestID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::mpi_ireceive_request(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        requestID));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_isend(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                        void* userData, OTF2_AttributeList* attributeList,
                                        uint32_t receiver, OTF2_CommRef communicator,
                                        uint32_t msgTag, uint64_t msgLength, uint64_t requestID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::mpi_isend(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        receiver, reader->comms()[communicator], msgTag, msgLength, requestID));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_isend_complete(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                 void* userData, OTF2_AttributeList* attributeList,
                                                 uint64_t requestID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::mpi_isend_complete(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        requestID));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_recv(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                       void* userData, OTF2_AttributeList* attributeList,
                                       uint32_t sender, OTF2_CommRef communicator, uint32_t msgTag,
                                       uint64_t msgLength)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::mpi_receive(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        sender, reader->comms()[communicator], msgTag, msgLength));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_request_cancelled(OTF2_LocationRef locationID,
                                                    OTF2_TimeStamp time, void* userData,
                                                    OTF2_AttributeList* attributeList,
                                                    uint64_t requestID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::mpi_request_cancelled(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        requestID));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_request_test(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                               void* userData, OTF2_AttributeList* attributeList,
                                               uint64_t requestID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::mpi_request_test(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        requestID));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_send(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                       void* userData, OTF2_AttributeList* attributeList,
                                       uint32_t receiver, OTF2_CommRef communicator,
                                       uint32_t msgTag, uint64_t msgLength)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::mpi_send(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        receiver, reader->comms()[communicator], msgTag, msgLength));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode parameter_int(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                            void* userData, OTF2_AttributeList* attributeList,
                                            OTF2_ParameterRef parameter, int64_t value)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::parameter_int(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->parameters()[parameter], value));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode parameter_string(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                               void* userData, OTF2_AttributeList* attributeList,
                                               OTF2_ParameterRef parameter, OTF2_StringRef string)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::parameter_string(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->parameters()[parameter], reader->strings()[string]));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode parameter_unsigned_int(OTF2_LocationRef locationID,
                                                     OTF2_TimeStamp time, void* userData,
                                                     OTF2_AttributeList* attributeList,
                                                     OTF2_ParameterRef parameter, uint64_t value)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::parameter_unsigned_int(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->parameters()[parameter], value));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode calling_context_sample(OTF2_LocationRef locationID,
                                                     OTF2_TimeStamp time, void* userData,
                                                     OTF2_AttributeList* attributeList,
                                                     OTF2_CallingContextRef callingContext,
                                                     uint32_t unwindDistance,
                                                     OTF2_InterruptGeneratorRef interruptGenerator)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::calling_context_sample(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->calling_contexts()[callingContext], unwindDistance,
                        reader->interrupt_generators()[interruptGenerator]));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            // clang-format off

    //              OTF2_CallbackCode RmaAcquireLock  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId, OTF2_LockType lockType);
    //              OTF2_CallbackCode RmaAtomic  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, OTF2_RmaAtomicType type, uint64_t bytesSent, uint64_t bytesReceived, uint64_t matchingId);
    //              OTF2_CallbackCode RmaCollectiveBegin  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList);
    //              OTF2_CallbackCode RmaCollectiveEnd  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CollectiveOp collectiveOp, OTF2_RmaSyncLevel syncLevel, OTF2_RmaWinRef win, uint32_t root, uint64_t bytesSent, uint64_t bytesReceived);
    //              OTF2_CallbackCode RmaGet  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t bytes, uint64_t matchingId);
    //              OTF2_CallbackCode RmaGroupSync  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaSyncLevel syncLevel, OTF2_RmaWinRef win, OTF2_GroupRef group);
    //              OTF2_CallbackCode RmaOpCompleteBlocking  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint64_t matchingId);
    //              OTF2_CallbackCode RmaOpCompleteNonBlocking  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint64_t matchingId);
    //              OTF2_CallbackCode RmaOpCompleteRemote  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint64_t matchingId);
    //              OTF2_CallbackCode RmaOpTest  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint64_t matchingId);
    //              OTF2_CallbackCode RmaPut  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t bytes, uint64_t matchingId);
    //              OTF2_CallbackCode RmaReleaseLock  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId);
    //              OTF2_CallbackCode RmaRequestLock  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId, OTF2_LockType lockType);
    //              OTF2_CallbackCode RmaSync  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, OTF2_RmaSyncType syncType);
    //              OTF2_CallbackCode RmaTryLock  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId, OTF2_LockType lockType);
    //              OTF2_CallbackCode RmaWaitChange  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win);
    //              OTF2_CallbackCode RmaWinCreate  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win);
    //              OTF2_CallbackCode RmaWinDestroy  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win);

            // clang-format on

            OTF2_CallbackCode thread_acquire_lock(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                  void* userData, OTF2_AttributeList* attributeList,
                                                  OTF2_Paradigm model, uint32_t lockID,
                                                  uint32_t acquisitionOrder)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_acquire_lock(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        static_cast<otf2::common::paradigm_type>(model), lockID, acquisitionOrder));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_fork(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                          void* userData, OTF2_AttributeList* attributeList,
                                          OTF2_Paradigm model, uint32_t numberOfRequestedThreads)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_fork(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        static_cast<otf2::common::paradigm_type>(model), numberOfRequestedThreads));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_join(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                          void* userData, OTF2_AttributeList* attributeList,
                                          OTF2_Paradigm model)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_join(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        static_cast<otf2::common::paradigm_type>(model)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_release_lock(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                  void* userData, OTF2_AttributeList* attributeList,
                                                  OTF2_Paradigm model, uint32_t lockID,
                                                  uint32_t acquisitionOrder)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_release_lock(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        static_cast<otf2::common::paradigm_type>(model), lockID, acquisitionOrder));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_task_complete(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                   void* userData,
                                                   OTF2_AttributeList* attributeList,
                                                   OTF2_CommRef threadTeam, uint32_t creatingThread,
                                                   uint32_t generationNumber)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_task_complete(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->comms()[threadTeam], creatingThread, generationNumber));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_task_create(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                 void* userData, OTF2_AttributeList* attributeList,
                                                 OTF2_CommRef threadTeam, uint32_t creatingThread,
                                                 uint32_t generationNumber)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_task_create(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->comms()[threadTeam], creatingThread, generationNumber));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_task_switch(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                 void* userData, OTF2_AttributeList* attributeList,
                                                 OTF2_CommRef threadTeam, uint32_t creatingThread,
                                                 uint32_t generationNumber)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_task_switch(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->comms()[threadTeam], creatingThread, generationNumber));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_team_begin(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                void* userData, OTF2_AttributeList* attributeList,
                                                OTF2_CommRef threadTeam)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_team_begin(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->comms()[threadTeam]));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_team_end(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                              void* userData, OTF2_AttributeList* attributeList,
                                              OTF2_CommRef threadTeam)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_team_end(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->comms()[threadTeam]));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_create(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                            void* userData, OTF2_AttributeList* attributeList,
                                            OTF2_CommRef threadContingent,
                                            std::uint64_t sequenceCount)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_create(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->comms()[threadContingent], sequenceCount));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_begin(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                           void* userData, OTF2_AttributeList* attributeList,
                                           OTF2_CommRef threadContingent,
                                           std::uint64_t sequenceCount)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_begin(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->comms()[threadContingent], sequenceCount));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_wait(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                          void* userData, OTF2_AttributeList* attributeList,
                                          OTF2_CommRef threadContingent,
                                          std::uint64_t sequenceCount)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_wait(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->comms()[threadContingent], sequenceCount));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_end(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                         void* userData, OTF2_AttributeList* attributeList,
                                         OTF2_CommRef threadContingent, std::uint64_t sequenceCount)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::thread_end(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        reader->comms()[threadContingent], sequenceCount));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_create_handle(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                               void* userData, OTF2_AttributeList* attributeList,
                                               OTF2_IoHandleRef handle, OTF2_IoAccessMode mode,
                                               OTF2_IoCreationFlag creationFlags, OTF2_IoStatusFlag statusFlags)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                        reader->locations()[locationID],
                        otf2::event::io_create_handle(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                    time - reader->clock_properties().start_time().count())),
                            reader->io_handles()[handle],
                            static_cast<otf2::common::io_access_mode_type>(mode),
                            static_cast<otf2::common::io_creation_flag_type>(creationFlags),
                            static_cast<otf2::common::io_status_flag_type>(statusFlags)
                ));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_destroy_handle(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                void* userData, OTF2_AttributeList* attributeList,
                                                OTF2_IoHandleRef handle)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                        reader->locations()[locationID],
                        otf2::event::io_destroy_handle(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                    time - reader->clock_properties().start_time().count())),
                            reader->io_handles()[handle]
                ));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_duplicate_handle(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                  void* userData, OTF2_AttributeList* attributeList,
                                                  OTF2_IoHandleRef oldHandle, OTF2_IoHandleRef newHandle,
                                                  OTF2_IoStatusFlag statusFlags)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                        reader->locations()[locationID],
                        otf2::event::io_duplicate_handle(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                    time - reader->clock_properties().start_time().count())),
                            reader->io_handles()[oldHandle],
                            reader->io_handles()[newHandle],
                            static_cast<otf2::common::io_status_flag_type>(statusFlags)
                ));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_seek(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                      void* userData, OTF2_AttributeList* attributeList,
                                      OTF2_IoHandleRef handle, int64_t offsetRequest,
                                      OTF2_IoSeekOption whence, uint64_t offsetResult)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                        reader->locations()[locationID],
                        otf2::event::io_seek(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                    time - reader->clock_properties().start_time().count())),
                            reader->io_handles()[handle],
                            offsetRequest,
                            static_cast<otf2::common::io_seek_option_type>(whence),
                            offsetResult
                ));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_change_status_flag(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                    void* userData, OTF2_AttributeList* attributeList,
                                                    OTF2_IoHandleRef handle, OTF2_IoStatusFlag statusFlags)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                        reader->locations()[locationID],
                        otf2::event::io_change_status_flag(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                    time - reader->clock_properties().start_time().count())),
                            reader->io_handles()[handle],
                            static_cast<otf2::common::io_status_flag_type>(statusFlags)
                ));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_delete_file(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                             void* userData, OTF2_AttributeList* attributeList,
                                             OTF2_IoParadigmRef ioParadigm, OTF2_IoFileRef file)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                        reader->locations()[locationID],
                        otf2::event::io_delete_file(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                    time - reader->clock_properties().start_time().count())),
                            //TODO ADD IO_PARADIGM
                            reader->io_files()[file]
                ));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_operation_begin(OTF2_LocationRef locationId, OTF2_TimeStamp time,
                                                 void* userData, OTF2_AttributeList* attributeList,
                                                 OTF2_IoHandleRef handle, OTF2_IoOperationMode mode,
                                                 OTF2_IoOperationFlag operationFlags, uint64_t bytesRequest,
                                                 uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationId],
                    otf2::event::io_operation_begin(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                time - reader->clock_properties().start_time().count())),
                        reader->io_handles()[handle],
                        static_cast<otf2::common::io_operation_mode_type>(mode),
                        static_cast<otf2::common::io_operation_flag_type>(operationFlags),
                        bytesRequest, matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_operation_test(OTF2_LocationRef locationID OTF2_TimeStamp time,
                                                void* userData, OTF2_AttributeList* attributeList,
                                                OTF2_IoHandleRef handle, uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                        reader->locations()[locationID],
                        otf2::event::io_operation_test(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                    time - reader->clock_properties().start_time().count())),
                            reader->io_handles()[handle], matchingId
                ));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_operation_complete(OTF2_LocationRef locationId, OTF2_TimeStamp time,
                                                    void* userData, OTF2_AttributeList* attributeList,
                                                    OTF2_IoHandleRef handle, uint64_t bytesRequest,
                                                    uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationId],
                    otf2::event::io_operation_complete(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                time - reader->clock_properties().start_time().count())),
                        reader->io_handles()[handle], bytesRequest, matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode unknown(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                      void* userData, OTF2_AttributeList*)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                reader->callback().event(
                    reader->locations()[locationID],
                    otf2::event::unknown(
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count()))));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }
        }
    }
}
} // namespace otf2::reader::detail::event
