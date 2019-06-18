/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2018, Technische Universität Dresden, Germany
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::enter(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::region>(regionID)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode leave(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                    void* userData, OTF2_AttributeList* attributeList,
                                    OTF2_RegionRef regionID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::leave(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::region>(regionID)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode measurement(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                          void* userData, OTF2_AttributeList* attributeList,
                                          OTF2_MeasurementMode measurementMode)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                                     const OTF2_Type* typeIDs, const OTF2_MetricValue* metricValues)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                // WORKAROUND for broken Score-P traces
                if (time < reader->clock_properties().start_time().count())
                {
                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                otf2::chrono::time_point timestamp =
                    otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                        time - reader->clock_properties().start_time().count()));

                otf2::event::metric::values metric_values{
                    std::vector<OTF2_Type>{ typeIDs, typeIDs + numberOfMetrics },
                    std::vector<OTF2_MetricValue>{ metricValues, metricValues + numberOfMetrics }
                };

                otf2::event::metric metric_event{ attributeList, timestamp,
                                                  std::move(metric_values) };

                // assumes a valid trace file
                if (registry.has<otf2::definition::metric_class>(metric))
                {
                    // create metric_event that references a metric_class
                    metric_event.metric_class(registry.get<otf2::definition::metric_class>(metric));
                }
                else
                {
                    // create metric_event that references a metric_instance
                    metric_event.metric_instance(
                        registry.get<otf2::definition::metric_instance>(metric));
                }

                reader->callback().event(registry.get<otf2::definition::location>(locationID),
                                         metric_event);
                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_collective_begin(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                   void* userData,
                                                   OTF2_AttributeList* attributeList)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::mpi_collective_end(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        static_cast<otf2::event::mpi_collective_end::collective_type>(collectiveOp),
                        registry.get<otf2::definition::comm>(communicator), root, sizeSent,
                        sizeReceived));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_irecv(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                        void* userData, OTF2_AttributeList* attributeList,
                                        uint32_t sender, OTF2_CommRef communicator, uint32_t msgTag,
                                        uint64_t msgLength, uint64_t requestID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::mpi_ireceive(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        sender, registry.get<otf2::definition::comm>(communicator), msgTag,
                        msgLength, requestID));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_irecv_request(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                void* userData, OTF2_AttributeList* attributeList,
                                                uint64_t requestID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::mpi_isend(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        receiver, registry.get<otf2::definition::comm>(communicator), msgTag,
                        msgLength, requestID));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_isend_complete(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                 void* userData, OTF2_AttributeList* attributeList,
                                                 uint64_t requestID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::mpi_receive(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        sender, registry.get<otf2::definition::comm>(communicator), msgTag,
                        msgLength));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode mpi_request_cancelled(OTF2_LocationRef locationID,
                                                    OTF2_TimeStamp time, void* userData,
                                                    OTF2_AttributeList* attributeList,
                                                    uint64_t requestID)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::mpi_send(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        receiver, registry.get<otf2::definition::comm>(communicator), msgTag,
                        msgLength));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode parameter_int(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                            void* userData, OTF2_AttributeList* attributeList,
                                            OTF2_ParameterRef parameter, int64_t value)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::parameter_int(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::parameter>(parameter), value));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode parameter_string(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                               void* userData, OTF2_AttributeList* attributeList,
                                               OTF2_ParameterRef parameter, OTF2_StringRef string)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::parameter_string(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::parameter>(parameter),
                        registry.get<otf2::definition::string>(string)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode parameter_unsigned_int(OTF2_LocationRef locationID,
                                                     OTF2_TimeStamp time, void* userData,
                                                     OTF2_AttributeList* attributeList,
                                                     OTF2_ParameterRef parameter, uint64_t value)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::parameter_unsigned_int(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::parameter>(parameter), value));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode calling_context_enter(OTF2_LocationRef locationID,
                                                    OTF2_TimeStamp time, void* userData,
                                                    OTF2_AttributeList* attributeList,
                                                    OTF2_CallingContextRef callingContext,
                                                    uint32_t unwindDistance)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::calling_context_enter(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::calling_context>(callingContext),
                        unwindDistance));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode calling_context_leave(OTF2_LocationRef locationID,
                                                    OTF2_TimeStamp time, void* userData,
                                                    OTF2_AttributeList* attributeList,
                                                    OTF2_CallingContextRef callingContext)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::calling_context_leave(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::calling_context>(callingContext)));

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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::calling_context_sample(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::calling_context>(callingContext),
                        unwindDistance,
                        registry.get<otf2::definition::interrupt_generator>(interruptGenerator)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_acquire_lock(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                             void* userData, OTF2_AttributeList* attributeList,
                                             OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId,
                                             OTF2_LockType lockType)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_acquire_lock(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), remote, lockId,
                        static_cast<otf2::event::rma_acquire_lock::lock_type_type>(lockType)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_atomic(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                        void* userData, OTF2_AttributeList* attributeList,
                                        OTF2_RmaWinRef win, uint32_t remote,
                                        OTF2_RmaAtomicType type, uint64_t bytesSent,
                                        uint64_t bytesReceived, uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_atomic(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), remote,
                        static_cast<otf2::event::rma_atomic::atomic_type>(type), bytesSent,
                        bytesReceived, matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_collective_begin(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                 void* userData, OTF2_AttributeList* attributeList)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_collective_begin(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count()))));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_collective_end(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                               void* userData, OTF2_AttributeList* attributeList,
                                               OTF2_CollectiveOp collectiveOp,
                                               OTF2_RmaSyncLevel syncLevel, OTF2_RmaWinRef win,
                                               uint32_t root, uint64_t bytesSent,
                                               uint64_t bytesReceived)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_collective_end(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        static_cast<otf2::event::rma_collective_end::collective_type>(collectiveOp),
                        static_cast<otf2::event::rma_collective_end::sync_level_type>(syncLevel),
                        registry.get<otf2::definition::rma_win>(win), root, bytesSent,
                        bytesReceived));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_get(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                     void* userData, OTF2_AttributeList* attributeList,
                                     OTF2_RmaWinRef win, uint32_t remote, uint64_t bytes,
                                     uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_get(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), remote, bytes, matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_group_sync(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                           void* userData, OTF2_AttributeList* attributeList,
                                           OTF2_RmaSyncLevel syncLevel, OTF2_RmaWinRef win,
                                           OTF2_GroupRef group)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_group_sync(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        static_cast<otf2::event::rma_group_sync::sync_level_type>(syncLevel),
                        registry.get<otf2::definition::rma_win>(win),
                        registry.get<otf2::definition::comm_group>(group)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_op_complete_blocking(OTF2_LocationRef locationID,
                                                    OTF2_TimeStamp time, void* userData,
                                                    OTF2_AttributeList* attributeList,
                                                    OTF2_RmaWinRef win, uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_op_complete_blocking(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_op_complete_non_blocking(OTF2_LocationRef locationID,
                                                       OTF2_TimeStamp time, void* userData,
                                                       OTF2_AttributeList* attributeList,
                                                       OTF2_RmaWinRef win, uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_op_complete_non_blocking(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_op_complete_remote(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                  void* userData, OTF2_AttributeList* attributeList,
                                                  OTF2_RmaWinRef win, uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_op_complete_remote(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_op_test(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                        void* userData, OTF2_AttributeList* attributeList,
                                        OTF2_RmaWinRef win, uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_op_test(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_put(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                     void* userData, OTF2_AttributeList* attributeList,
                                     OTF2_RmaWinRef win, uint32_t remote, uint64_t bytes,
                                     uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_put(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), remote, bytes, matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_release_lock(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                             void* userData, OTF2_AttributeList* attributeList,
                                             OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_release_lock(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), remote, lockId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_request_lock(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                             void* userData, OTF2_AttributeList* attributeList,
                                             OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId,
                                             OTF2_LockType lockType)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_request_lock(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), remote, lockId,
                        static_cast<otf2::event::rma_request_lock::lock_type_type>(lockType)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_sync(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                      void* userData, OTF2_AttributeList* attributeList,
                                      OTF2_RmaWinRef win, uint32_t remote,
                                      OTF2_RmaSyncType syncType)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_sync(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), remote,
                        static_cast<otf2::event::rma_sync::sync_type_type>(syncType)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_try_lock(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                         void* userData, OTF2_AttributeList* attributeList,
                                         OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId,
                                         OTF2_LockType lockType)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_try_lock(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win), remote, lockId,
                        static_cast<otf2::event::rma_try_lock::lock_type_type>(lockType)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_wait_change(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                            void* userData, OTF2_AttributeList* attributeList,
                                            OTF2_RmaWinRef win)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_wait_change(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_win_create(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                           void* userData, OTF2_AttributeList* attributeList,
                                           OTF2_RmaWinRef win)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_win_create(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode rma_win_destroy(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                              void* userData, OTF2_AttributeList* attributeList,
                                              OTF2_RmaWinRef win)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::rma_win_destroy(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::rma_win>(win)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_acquire_lock(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                  void* userData, OTF2_AttributeList* attributeList,
                                                  OTF2_Paradigm model, uint32_t lockID,
                                                  uint32_t acquisitionOrder)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
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
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::thread_task_complete(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::comm>(threadTeam), creatingThread,
                        generationNumber));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_task_create(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                 void* userData, OTF2_AttributeList* attributeList,
                                                 OTF2_CommRef threadTeam, uint32_t creatingThread,
                                                 uint32_t generationNumber)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::thread_task_create(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::comm>(threadTeam), creatingThread,
                        generationNumber));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_task_switch(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                 void* userData, OTF2_AttributeList* attributeList,
                                                 OTF2_CommRef threadTeam, uint32_t creatingThread,
                                                 uint32_t generationNumber)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::thread_task_switch(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::comm>(threadTeam), creatingThread,
                        generationNumber));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_team_begin(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                void* userData, OTF2_AttributeList* attributeList,
                                                OTF2_CommRef threadTeam)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::thread_team_begin(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::comm>(threadTeam)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_team_end(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                              void* userData, OTF2_AttributeList* attributeList,
                                              OTF2_CommRef threadTeam)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::thread_team_end(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::comm>(threadTeam)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_create(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                            void* userData, OTF2_AttributeList* attributeList,
                                            OTF2_CommRef threadContingent,
                                            std::uint64_t sequenceCount)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::thread_create(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::comm>(threadContingent), sequenceCount));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_begin(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                           void* userData, OTF2_AttributeList* attributeList,
                                           OTF2_CommRef threadContingent,
                                           std::uint64_t sequenceCount)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::thread_begin(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::comm>(threadContingent), sequenceCount));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_wait(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                          void* userData, OTF2_AttributeList* attributeList,
                                          OTF2_CommRef threadContingent,
                                          std::uint64_t sequenceCount)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::thread_wait(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::comm>(threadContingent), sequenceCount));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode thread_end(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                         void* userData, OTF2_AttributeList* attributeList,
                                         OTF2_CommRef threadContingent, std::uint64_t sequenceCount)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::thread_end(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::comm>(threadContingent), sequenceCount));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_create_handle(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                               void* userData, OTF2_AttributeList* attributeList,
                                               OTF2_IoHandleRef handle, OTF2_IoAccessMode mode,
                                               OTF2_IoCreationFlag creationFlags,
                                               OTF2_IoStatusFlag statusFlags)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_create_handle(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle),
                        static_cast<otf2::common::io_access_mode_type>(mode),
                        static_cast<otf2::common::io_creation_flag_type>(creationFlags),
                        static_cast<otf2::common::io_status_flag_type>(statusFlags)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_destroy_handle(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                void* userData, OTF2_AttributeList* attributeList,
                                                OTF2_IoHandleRef handle)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_destroy_handle(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_duplicate_handle(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                  void* userData, OTF2_AttributeList* attributeList,
                                                  OTF2_IoHandleRef oldHandle,
                                                  OTF2_IoHandleRef newHandle,
                                                  OTF2_IoStatusFlag statusFlags)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_duplicate_handle(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(oldHandle),
                        registry.get<otf2::definition::io_handle>(newHandle),
                        static_cast<otf2::common::io_status_flag_type>(statusFlags)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_seek(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                      void* userData, OTF2_AttributeList* attributeList,
                                      OTF2_IoHandleRef handle, int64_t offsetRequest,
                                      OTF2_IoSeekOption whence, uint64_t offsetResult)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_seek(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle), offsetRequest,
                        static_cast<otf2::common::io_seek_option_type>(whence), offsetResult));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_change_status_flag(OTF2_LocationRef locationID,
                                                    OTF2_TimeStamp time, void* userData,
                                                    OTF2_AttributeList* attributeList,
                                                    OTF2_IoHandleRef handle,
                                                    OTF2_IoStatusFlag statusFlags)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_change_status_flag(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle),
                        static_cast<otf2::common::io_status_flag_type>(statusFlags)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_delete_file(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                             void* userData, OTF2_AttributeList* attributeList,
                                             OTF2_IoParadigmRef ioParadigm, OTF2_IoFileRef file)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                if (registry.has<otf2::definition::io_handle>(file))
                {
                    reader->callback().event(
                        registry.get<otf2::definition::location>(locationID),
                        otf2::event::io_delete_file(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                time - reader->clock_properties().start_time().count())),
                            registry.get<otf2::definition::io_paradigm>(ioParadigm),
                            registry.get<otf2::definition::io_regular_file>(file)));
                }
                else
                {
                    reader->callback().event(
                        registry.get<otf2::definition::location>(locationID),
                        otf2::event::io_delete_file(
                            attributeList,
                            otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                                time - reader->clock_properties().start_time().count())),
                            registry.get<otf2::definition::io_paradigm>(ioParadigm),
                            registry.get<otf2::definition::io_directory>(file)));
                }

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_operation_begin(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                 void* userData, OTF2_AttributeList* attributeList,
                                                 OTF2_IoHandleRef handle, OTF2_IoOperationMode mode,
                                                 OTF2_IoOperationFlag operationFlags,
                                                 uint64_t bytesRequest, uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_operation_begin(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle),
                        static_cast<otf2::common::io_operation_mode_type>(mode),
                        static_cast<otf2::common::io_operation_flag_type>(operationFlags),
                        bytesRequest, matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_operation_test(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                void* userData, OTF2_AttributeList* attributeList,
                                                OTF2_IoHandleRef handle, uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_operation_test(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle), matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_operation_issued(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                                  void* userData, OTF2_AttributeList* attributeList,
                                                  OTF2_IoHandleRef handle, uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_operation_issued(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle), matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_operation_cancelled(OTF2_LocationRef locationID,
                                                     OTF2_TimeStamp time, void* userData,
                                                     OTF2_AttributeList* attributeList,
                                                     OTF2_IoHandleRef handle, uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_operation_cancelled(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle), matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_operation_complete(OTF2_LocationRef locationID,
                                                    OTF2_TimeStamp time, void* userData,
                                                    OTF2_AttributeList* attributeList,
                                                    OTF2_IoHandleRef handle, uint64_t bytesRequest,
                                                    uint64_t matchingId)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_operation_complete(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle), bytesRequest,
                        matchingId));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_acquire_lock(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                              void* userData, OTF2_AttributeList* attributeList,
                                              OTF2_IoHandleRef handle, OTF2_LockType lockType)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_acquire_lock(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle),
                        static_cast<otf2::common::lock_type>(lockType)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_release_lock(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                              void* userData, OTF2_AttributeList* attributeList,
                                              OTF2_IoHandleRef handle, OTF2_LockType lockType)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_release_lock(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle),
                        static_cast<otf2::common::lock_type>(lockType)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode io_try_lock(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                          void* userData, OTF2_AttributeList* attributeList,
                                          OTF2_IoHandleRef handle, OTF2_LockType lockType)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::io_try_lock(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::io_handle>(handle),
                        static_cast<otf2::common::lock_type>(lockType)));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode program_begin(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                            void* userData, OTF2_AttributeList* attributeList,
                                            OTF2_StringRef programName, uint32_t numberOfArguments,
                                            const OTF2_StringRef* programArguments)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                std::vector<otf2::definition::detail::weak_ref<otf2::definition::string>> args;

                for (uint32_t i = 0; i < numberOfArguments; i++)
                {
                    args.emplace_back(registry.get<otf2::definition::string>(programArguments[i]));
                }

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::program_begin(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        registry.get<otf2::definition::string>(programName), args));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode program_end(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                          void* userData, OTF2_AttributeList* attributeList,
                                          int64_t exitStatus)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::program_end(
                        attributeList,
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count())),
                        exitStatus));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }

            OTF2_CallbackCode unknown(OTF2_LocationRef locationID, OTF2_TimeStamp time,
                                      void* userData, OTF2_AttributeList*)
            {
                otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                auto& registry = reader->registry();

                reader->callback().event(
                    registry.get<otf2::definition::location>(locationID),
                    otf2::event::unknown(
                        otf2::chrono::convert(reader->ticks_per_second())(otf2::chrono::ticks(
                            time - reader->clock_properties().start_time().count()))));

                return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
            }
        } // namespace event
    }     // namespace detail
} // namespace reader
} // namespace otf2
