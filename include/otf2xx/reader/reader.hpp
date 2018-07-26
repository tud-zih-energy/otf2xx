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

#ifndef INCLUDE_OTF2XX_READER_READER_HPP
#define INCLUDE_OTF2XX_READER_READER_HPP

#include <otf2xx/definition/definitions.hpp>
#include <otf2xx/event/buffer.hpp>
#include <otf2xx/exception.hpp>
#include <otf2xx/reader/callback.hpp>
#include <otf2xx/reader/fwd.hpp>

#include <otf2/OTF2_GlobalDefReader.h>
#include <otf2/OTF2_GlobalEvtReader.h>
#include <otf2/OTF2_Reader.h>

#include <map>
#include <memory>
#include <string>

namespace otf2
{
namespace reader
{

    /**
     * \brief the class for reading in trace files
     *
     */
    class reader
    {
        template <typename Definition>
        using map_type = otf2::definition::container<Definition>;

    public:
        /**
         * \brief constructor
         * Triggers initialization done callback
         * \param name the path to the otf2 anchor file. Usually traces.otf2 in a score-p trace
         * \throws if it can't open the trace file
         */
        reader(const std::string& name) : rdr(OTF2_Reader_Open(name.c_str())), callback_(nullptr)
        {
            if (rdr == nullptr)
                throw make_exception("Couldn't open the trace file: ", name);
        }

        /**
         * \brief triggers the read of all definition records
         *
         * For each definition the callback is called.
         *
         * After all definitions are read, the method \ref
         *otf2::reader::callback::definitions_done() is called.
         */
        void read_definitions()
        {
            def_rdr = OTF2_Reader_GetGlobalDefReader(rdr);

            register_definition_callbacks();

            uint64_t definitions_read = 0;
            OTF2_Reader_ReadAllGlobalDefinitions(rdr, def_rdr, &definitions_read);

            callback().definitions_done(*this);
        }

        /**
         * \brief tells the reader, that it should read the events of the given location
         *
         * Call this method for every location you want to have the events read.
         *
         * Defaults to no locations.
         *
         * \param [in] location the location, for which the events should be read
         */
        void register_location(otf2::definition::location location) const
        {
            has_locations = true;

            OTF2_Reader_GetEvtReader(rdr, location.ref());

            OTF2_DefReader* def_reader = OTF2_Reader_GetDefReader(rdr, location.ref());

            uint64_t definitions_read = 0;
            check(OTF2_Reader_ReadAllLocalDefinitions(rdr, def_reader, &definitions_read),
                  "Couldn't read local definitions for location ", location, " from trace file");
        }

        /**
         * \brief triggers the read of all event records
         *
         * For each event the callback is called.
         *
         * After all events are read, the method \ref otf2::reader::callback::events_done() is
         *called.
         */
        void read_events()
        {
            if (has_locations)
            {
                evt_rdr = OTF2_Reader_GetGlobalEvtReader(rdr);
                register_event_callbacks();

                uint64_t events_read = 0;
                check(OTF2_Reader_ReadAllGlobalEvents(rdr, evt_rdr, &events_read),
                      "Couldn't read events from trace file");
            }
            callback().events_done(*this);
        }

    private:
        /**
         * \internal
         *
         * \brief prepares the otf2 callback struct for definition callbacks
         */
        void register_definition_callbacks()
        {
            OTF2_GlobalDefReaderCallbacks* global_def_callbacks =
                OTF2_GlobalDefReaderCallbacks_New();

            // clang-format off

            check(OTF2_GlobalDefReaderCallbacks_SetAttributeCallback(global_def_callbacks, detail::definition::global::attribute), "Couldn't set attribute callback handler");
            //OTF2_GlobalDefReaderCallbacks_SetCallpathCallback (global_def_callbacks, detail::definition::global::), "Couldn't set attribute callback handler");
            //OTF2_GlobalDefReaderCallbacks_SetCallsiteCallback (global_def_callbacks, detail::definition::global::), "Couldn't set attribute callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetClockPropertiesCallback(global_def_callbacks, detail::definition::global::clock_properties), "Couldn't set clock_properties callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetCommCallback(global_def_callbacks, detail::definition::global::comm), "Couldn't set comm callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetGroupCallback(global_def_callbacks, detail::definition::global::group), "Couldn't set group callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetLocationCallback(global_def_callbacks, otf2::reader::detail::definition::global::location), "Couldn't set location callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetLocationGroupCallback(global_def_callbacks, detail::definition::global::location_group), "Couldn't set location_group callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetMetricClassCallback (global_def_callbacks, detail::definition::global::metric_class), "Couldn't set metric class callback handler");
            //check(OTF2_GlobalDefReaderCallbacks_SetMetricClassRecorderCallback (global_def_callbacks, detail::definition::global::), "Couldn't set attribute callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetMetricInstanceCallback (global_def_callbacks, detail::definition::global::metric_instance), "Couldn't set metric instance callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetMetricMemberCallback (global_def_callbacks, detail::definition::global::metric_member), "Couldn't set metric member callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetParameterCallback(global_def_callbacks, detail::definition::global::parameter), "Couldn't set parameter callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetRegionCallback(global_def_callbacks, detail::definition::global::region), "Couldn't set region callback handler");
            //check(OTF2_GlobalDefReaderCallbacks_SetRmaWinCallback (global_def_callbacks, detail::definition::global::), "Couldn't set attribute callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetStringCallback(global_def_callbacks, detail::definition::global::string), "Couldn't set string callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeCallback(global_def_callbacks, detail::definition::global::system_tree_node), "Couldn't set system_tree_node callback handler");
            //check(OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeDomainCallback (global_def_callbacks, detail::definition::global::), "Couldn't set attribute callback handler");

            check(OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodePropertyCallback (global_def_callbacks, detail::definition::global::system_tree_node_property), "Couldn't set attribute callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetLocationPropertyCallback (global_def_callbacks, detail::definition::global::location_property), "Couldn't set attribute callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetLocationGroupPropertyCallback (global_def_callbacks, detail::definition::global::location_group_property), "Couldn't set attribute callback handler");

            check(OTF2_GlobalDefReaderCallbacks_SetSourceCodeLocationCallback (global_def_callbacks, detail::definition::global::source_code_location), "Couldn't set source code location callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetCallingContextCallback (global_def_callbacks, detail::definition::global::calling_context), "Couldn't set calling context callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetCallingContextPropertyCallback (global_def_callbacks, detail::definition::global::calling_context_property), "Couldn't set calling context property callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetInterruptGeneratorCallback (global_def_callbacks, detail::definition::global::interrupt_generator), "Couldn't set interrupt generator callback handler");

            check(OTF2_GlobalDefReaderCallbacks_SetIoRegularFileCallback (global_def_callbacks, detail::definition::global::io_regular_file), "Couldn't set io file callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetIoDirectoryCallback (global_def_callbacks, detail::definition::global::io_directory), "Couldn't set io directory callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetIoHandleCallback (global_def_callbacks, detail::definition::global::io_handle), "Couldn't set io handle callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetIoParadigmCallback (global_def_callbacks, detail::definition::global::io_paradigm), "Couldn't set io paradigm callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetIoFilePropertyCallback (global_def_callbacks, detail::definition::global::io_file_property), "Couldn't set io file properties callback handler");
            check(OTF2_GlobalDefReaderCallbacks_SetIoPreCreatedHandleStateCallback (global_def_callbacks, detail::definition::global::io_pre_created_handle_state), "Couldn't set io pre created handle state callback handler");

            check(OTF2_GlobalDefReaderCallbacks_SetUnknownCallback(global_def_callbacks, detail::definition::global::unknown), "Couldn't set unknown callback handler");

            // clang-format on

            check(OTF2_Reader_RegisterGlobalDefCallbacks(rdr, def_rdr, global_def_callbacks,
                                                         static_cast<void*>(this)),
                  "Couldn't register callbacks in reader");
            OTF2_GlobalDefReaderCallbacks_Delete(global_def_callbacks);
        }

        /**
         * \internal
         *
         * \brief prepares the otf2 callback struct for event callbacks
         */
        void register_event_callbacks()
        {
            OTF2_GlobalEvtReaderCallbacks* event_callbacks = OTF2_GlobalEvtReaderCallbacks_New();

            // clang-format off
            check(OTF2_GlobalEvtReaderCallbacks_SetBufferFlushCallback(event_callbacks, detail::event::buffer_flush), "Couldn't set buffer_flush event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetEnterCallback(event_callbacks, detail::event::enter), "Couldn't set enter event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetLeaveCallback(event_callbacks, detail::event::leave), "Couldn't set leave event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMeasurementOnOffCallback(event_callbacks, detail::event::measurement), "Couldn't set measurement event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMetricCallback (event_callbacks, detail::event::metric), "Couldn't set metric event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMpiCollectiveBeginCallback(event_callbacks, detail::event::mpi_collective_begin), "Couldn't set mpi collective begin event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMpiCollectiveEndCallback(event_callbacks, detail::event::mpi_collective_end), "Couldn't set mpi collective end event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMpiIrecvCallback(event_callbacks, detail::event::mpi_irecv), "Couldn't set mpi_irecv event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMpiIrecvRequestCallback(event_callbacks, detail::event::mpi_irecv_request), "Couldn't set mpi_irecv_request event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMpiIsendCallback(event_callbacks, detail::event::mpi_isend), "Couldn't set mpi_isend event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMpiIsendCompleteCallback(event_callbacks, detail::event::mpi_isend_complete), "Couldn't set mpi_isend_complete event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMpiRecvCallback(event_callbacks, detail::event::mpi_recv), "Couldn't set mpi_recv event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMpiRequestCancelledCallback(event_callbacks, detail::event::mpi_request_cancelled), "Couldn't set mpi_request_cancelled event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMpiRequestTestCallback(event_callbacks, detail::event::mpi_request_test), "Couldn't set mpi_request_test event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetMpiSendCallback(event_callbacks, detail::event::mpi_send), "Couldn't set mpi_send event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetParameterIntCallback(event_callbacks, detail::event::parameter_int), "Couldn't set parameter_int event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetParameterStringCallback(event_callbacks, detail::event::parameter_string), "Couldn't set parameter_string event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetParameterUnsignedIntCallback(event_callbacks, detail::event::parameter_unsigned_int), "Couldn't set parameter_unsigned_int event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetCallingContextSampleCallback(event_callbacks, detail::event::calling_context_sample), "Couldn't set calling_context_sample event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaAcquireLockCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaAtomicCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaCollectiveBeginCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaCollectiveEndCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaGetCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaGroupSyncCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaOpCompleteBlockingCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaOpCompleteNonBlockingCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaOpCompleteRemoteCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaOpTestCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaPutCallback (event_callbacks, detail::event::);, "Couldn't set buffer_flush event callback")
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaReleaseLockCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaRequestLockCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaSyncCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaTryLockCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaWaitChangeCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaWinCreateCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            //             check(OTF2_GlobalEvtReaderCallbacks_SetRmaWinDestroyCallback (event_callbacks, detail::event::), "Couldn't set buffer_flush event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadAcquireLockCallback(event_callbacks, detail::event::thread_acquire_lock), "Couldn't set thread_acquire_lock event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadForkCallback(event_callbacks, detail::event::thread_fork), "Couldn't set thread_fork event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadJoinCallback(event_callbacks, detail::event::thread_join), "Couldn't set thread_join event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadReleaseLockCallback(event_callbacks, detail::event::thread_release_lock), "Couldn't set thread_release_lock event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadTaskCompleteCallback(event_callbacks, detail::event::thread_task_complete), "Couldn't set thread_task_complete event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadTaskCreateCallback(event_callbacks, detail::event::thread_task_create), "Couldn't set thread_task_create event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadTaskSwitchCallback(event_callbacks, detail::event::thread_task_switch), "Couldn't set thread_task_switch event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadTeamBeginCallback(event_callbacks, detail::event::thread_team_begin), "Couldn't set thread_team_begin event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadTeamEndCallback(event_callbacks, detail::event::thread_team_end), "Couldn't set thread_team_end event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadCreateCallback(event_callbacks, detail::event::thread_create), "Couldn't set thread_create event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadBeginCallback(event_callbacks, detail::event::thread_begin), "Couldn't set thread_begin event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadWaitCallback(event_callbacks, detail::event::thread_wait), "Couldn't set thread_wait event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetThreadEndCallback(event_callbacks, detail::event::thread_end), "Couldn't set thread_end event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoCreateHandleCallback(event_callbacks, detail::event::io_create_handle), "Couldn't set io_create_handle event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoDestroyHandleCallback(event_callbacks, detail::event::io_destroy_handle), "Couldn't set io_destroy_handle event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoDuplicateHandleCallback(event_callbacks, detail::event::io_duplicate_handle), "Couldn't set io_duplicate_handle event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoSeekCallback(event_callbacks, detail::event::io_seek), "Couldn't set io_seek event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoChangeStatusFlagsCallback(event_callbacks, detail::event::io_change_status_flag), "Couldn't set io_change_status_flag event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoDeleteFileCallback(event_callbacks, detail::event::io_delete_file), "Couldn't set io_delete_file event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoOperationBeginCallback(event_callbacks, detail::event::io_operation_begin), "Couldn't set io_operation_begin event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoOperationTestCallback(event_callbacks, detail::event::io_operation_test), "Couldn't set io_operation_test event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoOperationIssuedCallback(event_callbacks, detail::event::io_operation_issued), "Couldn't set io_operation_issued event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoOperationCancelledCallback(event_callbacks, detail::event::io_operation_cancelled), "Couldn't set io_operation_cancelled event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoOperationCompleteCallback(event_callbacks, detail::event::io_operation_complete), "Couldn't set io_operation_complete event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoAcquireLockCallback(event_callbacks, detail::event::io_acquire_lock), "Couldn't set io_acquire_lock event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoReleaseLockCallback(event_callbacks, detail::event::io_release_lock), "Couldn't set io_release_lock event callback");
            check(OTF2_GlobalEvtReaderCallbacks_SetIoTryLockCallback(event_callbacks, detail::event::io_try_lock), "Couldn't set io_try_lock event callback");

            check(OTF2_GlobalEvtReaderCallbacks_SetUnknownCallback(event_callbacks, detail::event::unknown), "Couldn't set unknown event callback");

            // clang-format on

            check(OTF2_Reader_RegisterGlobalEvtCallbacks(rdr, evt_rdr, event_callbacks,
                                                         static_cast<void*>(this)),
                  "Couldn't register event callbacks");
            OTF2_GlobalEvtReaderCallbacks_Delete(event_callbacks);
        }

    public:
        /**
         * \brief returns the number of locations
         */
        std::uint64_t num_locations() const
        {
            std::uint64_t result;

            check(OTF2_Reader_GetNumberOfLocations(rdr, &result),
                  "Couldn't get the number of locations from archive");

            return result;
        }

    public:
        /**
         * \brief returns the callback instance
         *
         * \return the instance of the callback
         */
        otf2::reader::callback& callback()
        {
            assert(has_callback());

            return *callback_;
        }

        /**
         * \brief returns true, if a callback was set
         *
         * \return whether a callback was set
         */
        bool has_callback() const
        {
            return callback_ != nullptr;
        }

        /**
         * \brief set the given callback as callback for the reader
         * \param callback an otf2::reader::callback instance
         * \param buffered if it's set to true, when internally use otf2::event::buffer
         */
        void set_callback(otf2::reader::callback& callback, bool buffered = false)
        {
            if (buffered)
            {
                buffer_.reset(new otf2::event::buffer(callback));
                callback_ = buffer_.get();
            }
            else
            {
                callback_ = &callback;
            }
        }

        reader(reader&) = delete;
        reader& operator=(reader&) = delete;

        /**
         * \brief destructor
         *
         * \internal
         *
         * Closes the OTF2_Reader
         */
        ~reader()
        {
            OTF2_Reader_Close(rdr);
        }

    public:
        /**
         * \brief returns all attributes
         *
         * This function returns every attribute definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::attribute>& attributes()
        {
            return attributes_;
        }

        /**
         * \brief returns all attributes in a const context
         *
         * This function returns every attribute definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::attribute>& attributes() const
        {
            return attributes_;
        }

        /**
         * \brief returns all comms
         *
         * This function returns every comm definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::comm>& comms()
        {
            return comms_;
        }

        /**
         * \brief returns all comms in a const context
         *
         * This function returns every comm definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::comm>& comms() const
        {
            return comms_;
        }


        /**
         * \brief returns all locations
         *
         * This function returns every location definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::location>& locations()
        {
            return locations_;
        }

        /**
         * \brief returns all locations in a const context
         *
         * This function returns every locations definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::location>& locations() const
        {
            return locations_;
        }

        /**
         * \brief returns all location_groups
         *
         * This function returns every location group definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::location_group>& location_groups()
        {
            return location_groups_;
        }

        /**
         * \brief returns all location_groups in a const context
         *
         * This function returns every location group definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::location_group>& location_groups() const
        {
            return location_groups_;
        }

        /**
         * \brief returns all parameters
         *
         * This function returns every parameter definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::parameter>& parameters()
        {
            return parameters_;
        }

        /**
         * \brief returns all parameters in a const context
         *
         * This function returns every parameter definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::parameter>& parameters() const
        {
            return parameters_;
        }

        /**
         * \brief returns all regions
         *
         * This function returns every regions definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::region>& regions()
        {
            return regions_;
        }

        /**
         * \brief returns all regions in a const context
         *
         * This function returns every regions definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::region>& regions() const
        {
            return regions_;
        }

        /**
         * \brief returns all strings
         *
         * This function returns every string definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::string>& strings()
        {
            return strings_;
        }

        /**
         * \brief returns all strings in a const context
         *
         * This function returns every string definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::string>& strings() const
        {
            return strings_;
        }

        /**
         * \brief returns all system tree nodes
         *
         * This function returns every system tree node definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::system_tree_node>& system_tree_nodes()
        {
            return system_tree_nodes_;
        }

        /**
         * \brief returns all system tree nodes in a const context
         *
         * This function returns every system tree node definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::system_tree_node>& system_tree_nodes() const
        {
            return system_tree_nodes_;
        }

        /**
         * \brief returns all source code locations
         *
         * This function returns every system tree node definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::source_code_location>& source_code_locations()
        {
            return source_code_locations_;
        }

        /**
         * \brief returns all source code locations in a const context
         *
         * This function returns every system tree node definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::source_code_location>& source_code_locations() const
        {
            return source_code_locations_;
        }

        /**
         * \brief returns all calling contexts
         *
         * This function returns every system tree node definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::calling_context>& calling_contexts()
        {
            return calling_contexts_;
        }

        /**
         * \brief returns all calling contexts in a const context
         *
         * This function returns every system tree node definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::calling_context>& calling_contexts() const
        {
            return calling_contexts_;
        }

        /**
         * \brief returns all interrupt generators
         *
         * This function returns every system tree node definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::interrupt_generator>& interrupt_generators()
        {
            return interrupt_generators_;
        }

        /**
         * \brief returns all interrupt generators in const context
         *
         * This function returns every system tree node definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::interrupt_generator>& interrupt_generators() const
        {
            return interrupt_generators_;
        }

        /**
         * \brief returns all io files, which could be either io regular files or io directories
         *
         * This function returns every io file definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::io_file>& io_files()
        {
            return io_files_;
        }

        /**
         * \brief returns all io files, which could be either io regular files or io directories in a const context
         *
         * This function returns every io file definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::io_file>& io_files() const
        {
            return io_files_;
        }

        /**
         * \brief returns all io regular files
         *
         * This function returns every io regular file definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::io_regular_file>& io_regular_files()
        {
            return io_regular_files_;
        }

        /**
         * \brief returns all io regular files in a const context
         *
         * This function returns every io regular file definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::io_regular_file>& io_regular_files() const
        {
            return io_regular_files_;
        }

        /**
         * \brief returns all io directories
         *
         * This function returns every io directory definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::io_directory>& io_directories()
        {
            return io_directories_;
        }

        /**
         * \brief returns all io directories in a const context
         *
         * This function returns every io directory definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::io_directory>& io_directories() const
        {
            return io_directories_;
        }

        /**
         * \brief returns all io handles
         *
         * This function returns every io handle definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::io_handle>& io_handles()
        {
            return io_handles_;
        }

        /**
         * \brief returns all io handles in a const context
         *
         * This function returns every io handle definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::io_handle>& io_handles() const
        {
            return io_handles_;
        }

        /**
         * \brief returns all io paradigms
         *
         * This function returns every io paradigm definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::io_paradigm>& io_paradigms()
        {
            return io_paradigms_;
        }

        /**
         * \brief returns all io paradigms in a const context
         *
         * This function returns every io paradigm definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::io_paradigm>& io_paradigms() const
        {
            return io_paradigms_;
        }

        /**
         * \brief returns all io pre created handle states
         *
         * This function returns every io pre created handle state definition, which was read until
         *the call of the function. This means there could be missing some. On the other hand it is
         *garantied, that any referenced definition is already there. So you are safe, when using a
         *returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::io_pre_created_handle_state>& io_pre_created_handle_states()
        {
            return io_pre_created_handle_states_;
        }

        /**
         * \brief returns all io pre created handle states in a const context
         *
         * This function returns every io pre created handle state definition, which was read until
         *the call of the function. This means there could be missing some. On the other hand it is
         *garantied, that any referenced definition is already there. So you are safe, when using a
         *returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::io_pre_created_handle_state>& io_pre_created_handle_states() const
        {
            return io_pre_created_handle_states_;
        }

    public:
        /**
         * \brief returns all groups of locations
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::locations_group>& locations_groups()
        {
            return locations_groups_;
        }

        /**
         * \brief returns all groups of locations in a const context
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::locations_group>& locations_groups() const
        {
            return locations_groups_;
        }

        /**
         * \brief returns all groups of regions
         *
         * This function returns every attribute definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::regions_group>& regions_groups()
        {
            return regions_groups_;
        }

        /**
         * \brief returns all groups of regions in a const context
         *
         * This function returns every attribute definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::regions_group>& regions_groups() const
        {
            return regions_groups_;
        }

        // map_type<otf2::definition::metric_group>& metric_groups_;

        /**
         * \brief returns all group of comm locations
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::comm_locations_group>& comm_locations_groups()
        {
            return comm_locations_groups_;
        }

        /**
         * \brief returns all group of comm locations in a const context
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::comm_locations_group>& comm_locations_groups() const
        {
            return comm_locations_groups_;
        }

        /**
         * \brief returns all group of comms
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */

        map_type<otf2::definition::comm_group>& comm_groups()
        {
            return comm_groups_;
        }

        /**
         * \brief returns all group of comms in a const context
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */

        const map_type<otf2::definition::comm_group>& comm_groups() const
        {
            return comm_groups_;
        }

        /**
         * \brief returns all group of comm selfs
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::comm_self_group>& comm_self_groups()
        {
            return comm_self_groups_;
        }

        /**
         * \brief returns all group of comm selfs in a const context
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::comm_self_group>& comm_self_groups() const
        {
            return comm_self_groups_;
        }

        /**
         * \brief returns all metric members
         *
         * This function returns every metric member definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::metric_member>& metric_members()
        {
            return metric_members_;
        }

        /**
         * \brief returns all metric members in a const context
         *
         * This function returns every metric member definition, which was read until the call of
         *the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::metric_member>& metric_members() const
        {
            return metric_members_;
        }

        /**
         * \brief returns all metric classes
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::metric_class>& metric_classes()
        {
            return metric_classes_;
        }

        /**
         * \brief returns all metric classes in a const context
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::metric_class>& metric_classes() const
        {
            return metric_classes_;
        }

        /**
         * \brief returns all metric instances
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::metric_instance>& metric_instances()
        {
            return metric_instances_;
        }

        /**
         * \brief returns all metric instances in a const context
         *
         * This function returns every group definition, which was read until the call of the
         *function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::metric_instance>& metric_instances() const
        {
            return metric_instances_;
        }

        /**
         * \brief returns all location properties
         *
         * This function returns every location property definition, which was read until the call
         *of the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::location_property>& location_properties()
        {
            return location_properties_;
        }

        /**
         * \brief returns all location properties in a const context
         *
         * This function returns every location property definition, which was read until the call
         *of the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::location_property>& location_properties() const
        {
            return location_properties_;
        }

        /**
         * \brief returns all location group properties
         *
         * This function returns every location property definition, which was read until the call
         *of the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::location_group_property>& location_group_properties()
        {
            return location_group_properties_;
        }

        /**
         * \brief returns all location group properties in a const context
         *
         * This function returns every location property definition, which was read until the call
         *of the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::location_group_property>& location_group_properties() const
        {
            return location_group_properties_;
        }

        /**
         * \brief returns all system tree node properties
         *
         * This function returns every location property definition, which was read until the call
         *of the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::system_tree_node_property>& system_tree_node_properties()
        {
            return system_tree_node_properties_;
        }

        /**
         * \brief returns all system tree node properties in a const context
         *
         * This function returns every location property definition, which was read until the call
         *of the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::system_tree_node_property>& system_tree_node_properties() const
        {
            return system_tree_node_properties_;
        }

        /**
         * \brief returns all calling context properties
         *
         * This function returns every location property definition, which was read until the call
         *of the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::calling_context_property>& calling_context_properties()
        {
            return calling_context_properties_;
        }

        /**
         * \brief returns all calling context properties in a const context
         *
         * This function returns every location property definition, which was read until the call
         *of the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::calling_context_property>& calling_context_properties() const
        {
            return calling_context_properties_;
        }

        /**
         * \brief returns all io file properties
         *
         * This function returns every io file property definition, which was read until the call
         *of the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        map_type<otf2::definition::io_file_property>& io_file_properties()
        {
            return io_file_properties_;
        }

        /**
         * \brief returns all io file properties in a const context
         *
         * This function returns every io file property definition, which was read until the call
         *of the function.
         * This means there could be missing some. On the other hand it is garantied, that any
         *referenced
         * definition is already there. So you are safe, when using a returned definition.
         *
         * \returns a otf2::definition::container which contains all definitions
         */
        const map_type<otf2::definition::io_file_property>& io_file_properties() const
        {
            return io_file_properties_;
        }

    public:
        /**
         * \brief returns the ticks per second
         *
         * You should check with \ref has_clock_properties() if there was a clock properties
         *definition before
         * you rely on this. Otherwise you will get the default of 10^9 ticks per second.
         */
        otf2::chrono::ticks ticks_per_second() const
        {
            if (has_clock_properties())
                return clock_properties_->ticks_per_second();
            else
                return otf2::chrono::ticks(1e9);
        }

        /**
         * \brief returns if clock properties were read from the trace already
         */
        bool has_clock_properties() const
        {
            return static_cast<bool>(clock_properties_);
        }

        /**
         * \brief set the clock properties definition
         * You shouldn't call this function
         * \internal
         * \param cp a unique_ptr to a clock properties definition
         */
        void set_clock_properties(std::unique_ptr<otf2::definition::clock_properties>&& cp)
        {
            clock_properties_.release();
            clock_properties_ = std::move(cp);
        }

        /**
         * \brief returns clock properties definition
         */
        const otf2::definition::clock_properties& clock_properties() const
        {
            assert(has_clock_properties());
            return *clock_properties_;
        }

    private:
        OTF2_Reader* rdr;
        OTF2_GlobalDefReader* def_rdr;
        OTF2_GlobalEvtReader* evt_rdr;

        map_type<otf2::definition::attribute> attributes_;
        map_type<otf2::definition::comm> comms_;
        map_type<otf2::definition::location> locations_;
        map_type<otf2::definition::location_group> location_groups_;
        map_type<otf2::definition::parameter> parameters_;
        map_type<otf2::definition::region> regions_;
        map_type<otf2::definition::string> strings_;
        map_type<otf2::definition::system_tree_node> system_tree_nodes_;

        map_type<otf2::definition::source_code_location> source_code_locations_;
        map_type<otf2::definition::calling_context> calling_contexts_;
        map_type<otf2::definition::interrupt_generator> interrupt_generators_;
        map_type<otf2::definition::io_handle> io_handles_;
        map_type<otf2::definition::io_file> io_files_;
        map_type<otf2::definition::io_regular_file> io_regular_files_;
        map_type<otf2::definition::io_directory> io_directories_;
        map_type<otf2::definition::io_paradigm> io_paradigms_;
        map_type<otf2::definition::io_pre_created_handle_state> io_pre_created_handle_states_;

        map_type<otf2::definition::locations_group> locations_groups_;
        map_type<otf2::definition::regions_group> regions_groups_;
        // map_type<otf2::definition::metric_group> metric_groups_;
        map_type<otf2::definition::comm_locations_group> comm_locations_groups_;
        map_type<otf2::definition::comm_group> comm_groups_;
        map_type<otf2::definition::comm_self_group> comm_self_groups_;

        map_type<otf2::definition::metric_member> metric_members_;
        map_type<otf2::definition::metric_class> metric_classes_;
        map_type<otf2::definition::metric_instance> metric_instances_;

        map_type<otf2::definition::location_property> location_properties_;
        map_type<otf2::definition::location_group_property> location_group_properties_;
        map_type<otf2::definition::system_tree_node_property> system_tree_node_properties_;
        map_type<otf2::definition::calling_context_property> calling_context_properties_;
        map_type<otf2::definition::io_file_property> io_file_properties_;

        std::unique_ptr<otf2::definition::clock_properties> clock_properties_;

        std::unique_ptr<otf2::reader::callback> buffer_;
        otf2::reader::callback* callback_;

        mutable bool has_locations = false;
    };

} // namespace reader
} // namespace otf2

#endif // INCLUDE_OTF2XX_READER_READER_HPP
