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

#ifndef INCLUDE_OTF2XX_WRITER_LOCAL_HPP
#define INCLUDE_OTF2XX_WRITER_LOCAL_HPP

#include <otf2/OTF2_DefWriter.h>
#include <otf2/OTF2_EvtWriter.h>

#include <otf2xx/definition/definitions.hpp>
#include <otf2xx/event/events.hpp>
#include <otf2xx/exception.hpp>

#include <otf2xx/chrono/chrono.hpp>

namespace otf2
{
namespace writer
{

    class local
    {
    public:
        local(OTF2_Archive* ar, const otf2::definition::location& location)
        : location_(location), ar_(ar), evt_wrt_(OTF2_Archive_GetEvtWriter(ar, location.ref())),
          def_wrt_(OTF2_Archive_GetDefWriter(ar, location.ref()))
        {
            if (evt_wrt_ == nullptr)
            {
                make_exception("Couldn't open local event writer for '", location, "'");
            }
            if (def_wrt_ == nullptr)
            {
                make_exception("Couldn't open local definition writer for '", location, "'");
            }
        }

        local(const local&) = delete;
        local& operator=(const local&) = delete;

        local(local&&) = default;
        local& operator=(local&&) = default;

        ~local()
        {
            check(OTF2_Archive_CloseDefWriter(ar_, def_wrt_));
            check(OTF2_Archive_CloseEvtWriter(ar_, evt_wrt_));
        }

    public:
        const otf2::definition::location& location()
        {
            return location_;
        }

        std::uint64_t num_events() const
        {
            std::uint64_t tmp;

            check(OTF2_EvtWriter_GetNumberOfEvents(evt_wrt_, &tmp),
                  "Couldn't get number of events");

            return tmp;
        }

    public:
        void write(const otf2::event::buffer_flush& data)
        {
            check(OTF2_EvtWriter_BufferFlush(evt_wrt_, data.attribute_list().get(),
                                             convert(data.timestamp()), convert(data.finish())),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::enter& data)
        {
            check(OTF2_EvtWriter_Enter(evt_wrt_, data.attribute_list().get(),
                                       convert(data.timestamp()), data.region_->ref().get()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::leave& data)
        {
            check(OTF2_EvtWriter_Leave(evt_wrt_, data.attribute_list().get(),
                                       convert(data.timestamp()), data.region_->ref().get()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::measurement& data)
        {
            check(OTF2_EvtWriter_MeasurementOnOff(evt_wrt_, data.attribute_list().get(),
                                                  convert(data.timestamp()),
                                                  static_cast<OTF2_MeasurementMode>(data.mode())),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::metric& data)
        {
            otf2::reference<otf2::definition::detail::metric_base>::ref_type metric;

            if (data.metric_instance_)
            {
                metric = data.metric_instance_->ref();
            }
            else
            {
                metric = data.metric_class_->ref();
            }

            std::size_t num_members = data.values().size();

            type_ids_.resize(num_members);
            values_.resize(num_members);

            for (std::size_t i = 0; i < num_members; i++)
            {
                type_ids_[i] = static_cast<OTF2_Type>(data.values()[i].metric->value_type());
                values_[i] = data.values()[i].value;
            }

            check(OTF2_EvtWriter_Metric(evt_wrt_, data.attribute_list().get(),
                                        convert(data.timestamp()), metric, num_members,
                                        type_ids_.data(), values_.data()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

    public:
        void write(const otf2::event::mpi_ireceive_complete& data)
        {
            check(OTF2_EvtWriter_MpiIrecv(evt_wrt_, data.attribute_list().get(),
                                          convert(data.timestamp()), data.sender(),
                                          data.comm_->ref(), data.msg_tag(), data.msg_length(),
                                          data.request_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::mpi_ireceive_request& data)
        {
            check(OTF2_EvtWriter_MpiIrecvRequest(evt_wrt_, data.attribute_list().get(),
                                                 convert(data.timestamp()), data.request_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::mpi_isend_request& data)
        {
            check(OTF2_EvtWriter_MpiIsend(evt_wrt_, data.attribute_list().get(),
                                          convert(data.timestamp()), data.receiver(),
                                          data.comm_->ref(), data.msg_tag(), data.msg_length(),
                                          data.request_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::mpi_isend_complete& data)
        {
            check(OTF2_EvtWriter_MpiIsendComplete(evt_wrt_, data.attribute_list().get(),
                                                  convert(data.timestamp()), data.request_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::mpi_receive& data)
        {
            check(OTF2_EvtWriter_MpiRecv(evt_wrt_, data.attribute_list().get(),
                                         convert(data.timestamp()), data.sender(),
                                         data.comm_->ref(), data.msg_tag(), data.msg_length()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::mpi_request_test& data)
        {
            check(OTF2_EvtWriter_MpiRequestTest(evt_wrt_, data.attribute_list().get(),
                                                convert(data.timestamp()), data.request_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::mpi_request_cancelled& data)
        {
            check(OTF2_EvtWriter_MpiRequestCancelled(evt_wrt_, data.attribute_list().get(),
                                                     convert(data.timestamp()), data.request_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::mpi_send& data)
        {
            check(OTF2_EvtWriter_MpiSend(evt_wrt_, data.attribute_list().get(),
                                         convert(data.timestamp()), data.receiver(),
                                         data.comm_->ref(), data.msg_tag(), data.msg_length()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::mpi_collective_begin& data)
        {
            check(OTF2_EvtWriter_MpiCollectiveBegin(evt_wrt_, data.attribute_list().get(),
                                                    convert(data.timestamp())),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::mpi_collective_end& data)
        {
            check(OTF2_EvtWriter_MpiCollectiveEnd(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      static_cast<OTF2_CollectiveOp>(data.type()), data.comm_->ref(), data.root(),
                      data.sent(), data.received()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

    public:
        void write(const otf2::event::parameter_int& data)
        {
            check(OTF2_EvtWriter_ParameterInt(evt_wrt_, data.attribute_list().get(),
                                              convert(data.timestamp()), data.parameter_->ref(),
                                              data.value()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::parameter_unsigned_int& data)
        {
            check(OTF2_EvtWriter_ParameterUnsignedInt(evt_wrt_, data.attribute_list().get(),
                                                      convert(data.timestamp()),
                                                      data.parameter_->ref(), data.value()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::parameter_string& data)
        {
            check(OTF2_EvtWriter_ParameterString(evt_wrt_, data.attribute_list().get(),
                                                 convert(data.timestamp()), data.parameter_->ref(),
                                                 data.value_->ref()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::calling_context_sample& data)
        {
            check(OTF2_EvtWriter_CallingContextSample(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      data.calling_context_->ref(), data.unwind_distance(),
                      data.interrupt_generator_->ref()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        // TODO find a better solution to allow high-performance writing of this event without
        // using shread ptrs
        void write_calling_context_sample(otf2::chrono::time_point timestamp,
                                          OTF2_CallingContextRef ref, uint32_t unwind_distance,
                                          OTF2_InterruptGeneratorRef interrupt_generator_ref)
        {
            check(OTF2_EvtWriter_CallingContextSample(evt_wrt_, nullptr, convert(timestamp), ref,
                                                      unwind_distance, interrupt_generator_ref),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write_enter(otf2::chrono::time_point timestamp, OTF2_RegionRef ref)
        {
            check(OTF2_EvtWriter_Enter(evt_wrt_, nullptr, convert(timestamp), ref),
                  "Couldn't write event to local event writer.");
        }

        void write_leave(otf2::chrono::time_point timestamp, OTF2_RegionRef ref)
        {
            check(OTF2_EvtWriter_Leave(evt_wrt_, nullptr, convert(timestamp), ref),
                  "Couldn't write event to local event writer.");
        }

    public:
        void write(const otf2::event::thread_acquire_lock& data)
        {
            check(OTF2_EvtWriter_ThreadAcquireLock(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      static_cast<OTF2_Paradigm>(data.paradigm()), data.lock_id(), data.order()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_fork& data)
        {
            check(OTF2_EvtWriter_ThreadFork(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      static_cast<OTF2_Paradigm>(data.paradigm()), data.num_threads()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_join& data)
        {
            check(OTF2_EvtWriter_ThreadJoin(evt_wrt_, data.attribute_list().get(),
                                            convert(data.timestamp()),
                                            static_cast<OTF2_Paradigm>(data.paradigm())),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_release_lock& data)
        {
            check(OTF2_EvtWriter_ThreadReleaseLock(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      static_cast<OTF2_Paradigm>(data.paradigm()), data.lock_id(), data.order()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_task_complete& data)
        {
            check(OTF2_EvtWriter_ThreadTaskComplete(evt_wrt_, data.attribute_list().get(),
                                                    convert(data.timestamp()), data.team_->ref(),
                                                    data.thread(), data.generation()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_task_create& data)
        {
            check(OTF2_EvtWriter_ThreadTaskCreate(evt_wrt_, data.attribute_list().get(),
                                                  convert(data.timestamp()), data.team_->ref(),
                                                  data.thread(), data.generation()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_task_switch& data)
        {
            check(OTF2_EvtWriter_ThreadTaskSwitch(evt_wrt_, data.attribute_list().get(),
                                                  convert(data.timestamp()), data.team_->ref(),
                                                  data.thread(), data.generation()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_team_begin& data)
        {
            check(OTF2_EvtWriter_ThreadTeamBegin(evt_wrt_, data.attribute_list().get(),
                                                 convert(data.timestamp()), data.comm_->ref()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_team_end& data)
        {
            check(OTF2_EvtWriter_ThreadTeamEnd(evt_wrt_, data.attribute_list().get(),
                                               convert(data.timestamp()), data.comm_->ref()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_create& data)
        {
            check(OTF2_EvtWriter_ThreadCreate(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      data.thread_contingent_->ref(), data.sequence_number()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_begin& data)
        {
            check(OTF2_EvtWriter_ThreadBegin(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      data.thread_contingent_->ref(), data.sequence_number()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_wait& data)
        {
            check(OTF2_EvtWriter_ThreadWait(evt_wrt_, data.attribute_list().get(),
                                            convert(data.timestamp()),
                                            data.thread_contingent_->ref(), data.sequence_number()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::thread_end& data)
        {
            check(OTF2_EvtWriter_ThreadEnd(evt_wrt_, data.attribute_list().get(),
                                           convert(data.timestamp()),
                                           data.thread_contingent_->ref(), data.sequence_number()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_create_handle& data)
        {
            check(OTF2_EvtWriter_IoCreateHandle(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      data.handle().ref(), static_cast<OTF2_IoAccessMode>(data.access_mode()),
                      static_cast<OTF2_IoCreationFlag>(data.creation_flags()),
                      static_cast<OTF2_IoStatusFlag>(data.status_flags())),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_destroy_handle& data)
        {
            check(OTF2_EvtWriter_IoDestroyHandle(evt_wrt_, data.attribute_list().get(),
                                                 convert(data.timestamp()), data.handle().ref()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_duplicate_handle& data)
        {
            check(OTF2_EvtWriter_IoDuplicateHandle(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      data.old_handle().ref(), data.new_handle().ref(),
                      static_cast<OTF2_IoStatusFlag>(data.status_flags())),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_seek& data)
        {
            check(OTF2_EvtWriter_IoSeek(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      data.handle().ref(), data.offset_request(),
                      static_cast<OTF2_IoSeekOption>(data.seek_option()), data.offset_result()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_change_status_flag& data)
        {
            check(OTF2_EvtWriter_IoChangeStatusFlags(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      data.handle().ref(), static_cast<OTF2_IoStatusFlag>(data.status_flags())),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_delete_file& data)
        {
            check(OTF2_EvtWriter_IoDeleteFile(evt_wrt_, data.attribute_list().get(),
                                              convert(data.timestamp()), data.paradigm().ref(),
                                              data.file().ref()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_operation_begin& data)
        {
            check(OTF2_EvtWriter_IoOperationBegin(
                      evt_wrt_, data.attribute_list().get(), convert(data.timestamp()),
                      data.handle().ref(), static_cast<OTF2_IoOperationMode>(data.operation_mode()),
                      static_cast<OTF2_IoOperationFlag>(data.operation_flag()),
                      data.bytes_request(), data.matching_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_operation_test& data)
        {
            check(OTF2_EvtWriter_IoOperationTest(evt_wrt_, data.attribute_list().get(),
                                                 convert(data.timestamp()), data.handle().ref(),
                                                 data.matching_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_operation_issued& data)
        {
            check(OTF2_EvtWriter_IoOperationIssued(evt_wrt_, data.attribute_list().get(),
                                                   convert(data.timestamp()), data.handle().ref(),
                                                   data.matching_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_operation_complete& data)
        {
            check(OTF2_EvtWriter_IoOperationComplete(evt_wrt_, data.attribute_list().get(),
                                                     convert(data.timestamp()), data.handle().ref(),
                                                     data.bytes_request(), data.matching_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_operation_cancelled& data)
        {
            check(OTF2_EvtWriter_IoOperationCancelled(evt_wrt_, data.attribute_list().get(),
                                                      convert(data.timestamp()),
                                                      data.handle().ref(), data.matching_id()),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_acquire_lock& data)
        {
            check(OTF2_EvtWriter_IoAcquireLock(evt_wrt_, data.attribute_list().get(),
                                               convert(data.timestamp()), data.handle().ref(),
                                               static_cast<OTF2_LockType>(data.lock_type())),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_release_lock& data)
        {
            check(OTF2_EvtWriter_IoReleaseLock(evt_wrt_, data.attribute_list().get(),
                                               convert(data.timestamp()), data.handle().ref(),
                                               static_cast<OTF2_LockType>(data.lock_type())),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

        void write(const otf2::event::io_try_lock& data)
        {
            check(OTF2_EvtWriter_IoTryLock(evt_wrt_, data.attribute_list().get(),
                                           convert(data.timestamp()), data.handle().ref(),
                                           static_cast<OTF2_LockType>(data.lock_type())),
                  "Couldn't write event to local event writer.");
            location_.event_written();
        }

    public:
        void write(const otf2::definition::mapping_table& def)
        {
            check(OTF2_DefWriter_WriteMappingTable(
                      def_wrt_, static_cast<OTF2_MappingType>(def.mapping_type()), def.id_map()),
                  "Couldn't write mapping table definition to local writer");
        }

    private:
        static OTF2_TimeStamp convert(otf2::chrono::time_point tp)
        {
            static otf2::chrono::convert cvrt(otf2::chrono::clock::period::den);
            static_assert(otf2::chrono::clock::period::num == 1,
                          "Don't mess around with the chrono stuff!");
            return cvrt(tp).count();
        }

    private:
        otf2::definition::location location_;
        OTF2_Archive* ar_;
        OTF2_EvtWriter* evt_wrt_;
        OTF2_DefWriter* def_wrt_;

        std::vector<OTF2_Type> type_ids_;
        std::vector<OTF2_MetricValue> values_;
    };

    template <typename Record>
    local& operator<<(local& wrt, Record rec)
    {
        wrt.write(rec);
        return wrt;
    }
} // namespace writer
} // namespace otf2

#endif // INCLUDE_OTF2XX_WRITER_LOCAL_HPP
