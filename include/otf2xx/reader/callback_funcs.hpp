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

#ifndef INCLUDE_OTF2XX_READER_CALLBACK_FUNCS_HPP
#define INCLUDE_OTF2XX_READER_CALLBACK_FUNCS_HPP

#include <otf2/OTF2_Reader.h>

namespace otf2
{
namespace reader
{
    namespace detail
    {
        namespace event
        {

            // clang-format off

            OTF2_CallbackCode buffer_flush  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_TimeStamp stopTime);
            OTF2_CallbackCode enter  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RegionRef region);
            OTF2_CallbackCode leave  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RegionRef region);
            OTF2_CallbackCode measurement  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_MeasurementMode measurementMode);
            OTF2_CallbackCode metric  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_MetricRef metric, uint8_t numberOfMetrics, const OTF2_Type *typeIDs, const OTF2_MetricValue *metricValues);
            OTF2_CallbackCode mpi_collective_begin  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList);
            OTF2_CallbackCode mpi_collective_end  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CollectiveOp collectiveOp, OTF2_CommRef communicator, uint32_t root, uint64_t sizeSent, uint64_t sizeReceived);
            OTF2_CallbackCode mpi_irecv  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, uint32_t sender, OTF2_CommRef communicator, uint32_t msgTag, uint64_t msgLength, uint64_t requestID);
            OTF2_CallbackCode mpi_irecv_request  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, uint64_t requestID);
            OTF2_CallbackCode mpi_isend  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, uint32_t receiver, OTF2_CommRef communicator, uint32_t msgTag, uint64_t msgLength, uint64_t requestID);
            OTF2_CallbackCode mpi_isend_complete  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, uint64_t requestID);
            OTF2_CallbackCode mpi_recv  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, uint32_t sender, OTF2_CommRef communicator, uint32_t msgTag, uint64_t msgLength);
            OTF2_CallbackCode mpi_request_cancelled  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, uint64_t requestID);
            OTF2_CallbackCode mpi_request_test  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, uint64_t requestID);
            OTF2_CallbackCode mpi_send  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, uint32_t receiver, OTF2_CommRef communicator, uint32_t msgTag, uint64_t msgLength);
            OTF2_CallbackCode parameter_int  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_ParameterRef parameter, int64_t value);
            OTF2_CallbackCode parameter_string  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_ParameterRef parameter, OTF2_StringRef string);
            OTF2_CallbackCode parameter_unsigned_int  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_ParameterRef parameter, uint64_t value);
            OTF2_CallbackCode calling_context_sample (OTF2_LocationRef location, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CallingContextRef callingContext, uint32_t unwindDistance, OTF2_InterruptGeneratorRef interruptGenerator);
    //             OTF2_CallbackCode RmaAcquireLock  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId, OTF2_LockType lockType);
    //             OTF2_CallbackCode RmaAtomic  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, OTF2_RmaAtomicType type, uint64_t bytesSent, uint64_t bytesReceived, uint64_t matchingId);
    //             OTF2_CallbackCode RmaCollectiveBegin  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList);
    //             OTF2_CallbackCode RmaCollectiveEnd  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CollectiveOp collectiveOp, OTF2_RmaSyncLevel syncLevel, OTF2_RmaWinRef win, uint32_t root, uint64_t bytesSent, uint64_t bytesReceived);
    //             OTF2_CallbackCode RmaGet  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t bytes, uint64_t matchingId);
    //             OTF2_CallbackCode RmaGroupSync  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaSyncLevel syncLevel, OTF2_RmaWinRef win, OTF2_GroupRef group);
    //             OTF2_CallbackCode RmaOpCompleteBlocking  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint64_t matchingId);
    //             OTF2_CallbackCode RmaOpCompleteNonBlocking  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint64_t matchingId);
    //             OTF2_CallbackCode RmaOpCompleteRemote  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint64_t matchingId);
    //             OTF2_CallbackCode RmaOpTest  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint64_t matchingId);
    //             OTF2_CallbackCode RmaPut  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t bytes, uint64_t matchingId);
    //             OTF2_CallbackCode RmaReleaseLock  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId);
    //             OTF2_CallbackCode RmaRequestLock  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId, OTF2_LockType lockType);
    //             OTF2_CallbackCode RmaSync  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, OTF2_RmaSyncType syncType);
    //             OTF2_CallbackCode RmaTryLock  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win, uint32_t remote, uint64_t lockId, OTF2_LockType lockType);
    //             OTF2_CallbackCode RmaWaitChange  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win);
    //             OTF2_CallbackCode RmaWinCreate  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win);
    //             OTF2_CallbackCode RmaWinDestroy  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_RmaWinRef win);
            OTF2_CallbackCode thread_acquire_lock  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_Paradigm model, uint32_t lockID, uint32_t acquisitionOrder);
            OTF2_CallbackCode thread_fork  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_Paradigm model, uint32_t numberOfRequestedThreads);
            OTF2_CallbackCode thread_join  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_Paradigm model);
            OTF2_CallbackCode thread_release_lock  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_Paradigm model, uint32_t lockID, uint32_t acquisitionOrder);
            OTF2_CallbackCode thread_task_complete  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CommRef threadTeam, uint32_t creatingThread, uint32_t generationNumber);
            OTF2_CallbackCode thread_task_create  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CommRef threadTeam, uint32_t creatingThread, uint32_t generationNumber);
            OTF2_CallbackCode thread_task_switch  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CommRef threadTeam, uint32_t creatingThread, uint32_t generationNumber);
            OTF2_CallbackCode thread_team_begin  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CommRef threadTeam);
            OTF2_CallbackCode thread_team_end  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CommRef threadTeam);
            OTF2_CallbackCode thread_create  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CommRef threadContingent, uint64_t sequenceNumber);
            OTF2_CallbackCode thread_begin  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CommRef threadContingent, uint64_t sequenceNumber);
            OTF2_CallbackCode thread_wait  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CommRef threadContingent, uint64_t sequenceNumber);
            OTF2_CallbackCode thread_end  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_CommRef threadContingent, uint64_t sequenceNumber);

            OTF2_CallbackCode io_create_handle (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_IoHandleRef handle, OTF2_IoAccessMode mode, OTF2_IoCreationFlag creationFlags, OTF2_IoStatusFlag statusFlags);
            OTF2_CallbackCode io_destroy_handle (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_IoHandleRef handle);
            OTF2_CallbackCode io_duplicate_handle (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_IoHandleRef oldHandle, OTF2_IoHandleRef newHandle, OTF2_IoStatusFlag statusFlags);
            OTF2_CallbackCode io_seek (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_IoHandleRef handle, int64_t offsetRequest, OTF2_IoSeekOption whence, uint64_t offsetResult);
            OTF2_CallbackCode io_change_status_flag (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_IoHandleRef handle, OTF2_IoStatusFlag statusFlags);
            OTF2_CallbackCode io_delete_file (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_IoParadigmRef ioParadigm, OTF2_IoFileRef file);
            OTF2_CallbackCode io_operation_begin (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_IoHandleRef handle, OTF2_IoOperationMode mode, OTF2_IoOperationFlag operationFlags, uint64_t bytesRequest, uint64_t matching_id);
            OTF2_CallbackCode io_operation_complete (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList, OTF2_IoHandleRef handle, uint64_t bytesRequest, uint64_t matching_id);

            OTF2_CallbackCode unknown  (OTF2_LocationRef locationID, OTF2_TimeStamp time, void *userData, OTF2_AttributeList *attributeList);

            // clang-format on
        }

        namespace definition
        {
            namespace global
            {
                // clang-format off

                OTF2_CallbackCode attribute  (void *userData, OTF2_AttributeRef self, OTF2_StringRef name, OTF2_StringRef description, OTF2_Type type);
                //OTF2_CallbackCode Callpath  (void *userData, OTF2_CallpathRef self, OTF2_CallpathRef parent, OTF2_RegionRef region);
                //OTF2_CallbackCode Callsite  (void *userData, OTF2_CallsiteRef self, OTF2_StringRef sourceFile, uint32_t lineNumber, OTF2_RegionRef enteredRegion, OTF2_RegionRef leftRegion);
                OTF2_CallbackCode clock_properties  (void *userData, uint64_t timerResolution, uint64_t globalOffset, uint64_t traceLength);
                OTF2_CallbackCode comm  (void *userData, OTF2_CommRef self, OTF2_StringRef name, OTF2_GroupRef group, OTF2_CommRef parent);
                OTF2_CallbackCode group  (void *userData, OTF2_GroupRef self, OTF2_StringRef name, OTF2_GroupType groupType, OTF2_Paradigm paradigm, OTF2_GroupFlag groupFlags, uint32_t numberOfMembers, const uint64_t *members);
                OTF2_CallbackCode location  (void *userData, OTF2_LocationRef self, OTF2_StringRef name, OTF2_LocationType locationType, uint64_t numberOfEvents, OTF2_LocationGroupRef locationGroup);
                OTF2_CallbackCode location_group  (void *userData, OTF2_LocationGroupRef self, OTF2_StringRef name, OTF2_LocationGroupType locationGroupType, OTF2_SystemTreeNodeRef systemTreeParent);
                OTF2_CallbackCode metric_class  (void *userData, OTF2_MetricRef self, uint8_t numberOfMetrics, const OTF2_MetricMemberRef *metricMembers, OTF2_MetricOccurrence metricOccurrence, OTF2_RecorderKind recorderKind);
                //OTF2_CallbackCode MetricClassRecorder  (void *userData, OTF2_MetricRef metricClass, OTF2_LocationRef recorder);
                OTF2_CallbackCode metric_instance  (void *userData, OTF2_MetricRef self, OTF2_MetricRef metricClass, OTF2_LocationRef recorder, OTF2_MetricScope metricScope, uint64_t scope);
                OTF2_CallbackCode metric_member  (void *userData, OTF2_MetricMemberRef self, OTF2_StringRef name, OTF2_StringRef description, OTF2_MetricType metricType, OTF2_MetricMode metricMode, OTF2_Type valueType, OTF2_Base metricBase, int64_t exponent, OTF2_StringRef unit);
                OTF2_CallbackCode parameter  (void *userData, OTF2_ParameterRef self, OTF2_StringRef name, OTF2_ParameterType parameterType);
                OTF2_CallbackCode region  (void *userData, OTF2_RegionRef self, OTF2_StringRef name, OTF2_StringRef canonicalName, OTF2_StringRef description, OTF2_RegionRole regionRole, OTF2_Paradigm paradigm, OTF2_RegionFlag regionFlags, OTF2_StringRef sourceFile, uint32_t beginLineNumber, uint32_t endLineNumber);
                //OTF2_CallbackCode RmaWin  (void *userData, OTF2_RmaWinRef self, OTF2_StringRef name, OTF2_CommRef comm);
                OTF2_CallbackCode string  (void *userData, OTF2_StringRef self, const char *string);
                OTF2_CallbackCode system_tree_node  (void *userData, OTF2_SystemTreeNodeRef self, OTF2_StringRef name, OTF2_StringRef className, OTF2_SystemTreeNodeRef parent);
                //OTF2_CallbackCode SystemTreeNodeDomain  (void *userData, OTF2_SystemTreeNodeRef systemTreeNode, OTF2_SystemTreeDomain systemTreeDomain);
                OTF2_CallbackCode system_tree_node_property  (void *userData, OTF2_SystemTreeNodeRef systemTreeNode, OTF2_StringRef name, OTF2_Type type, OTF2_AttributeValue value);
                OTF2_CallbackCode location_property  (void *userData, OTF2_LocationRef location, OTF2_StringRef name, OTF2_Type type, OTF2_AttributeValue value);
                OTF2_CallbackCode location_group_property  (void *userData, OTF2_LocationGroupRef locationGroup, OTF2_StringRef name, OTF2_Type type, OTF2_AttributeValue value);

                OTF2_CallbackCode source_code_location (void *userData, OTF2_SourceCodeLocationRef self, OTF2_StringRef file, uint32_t lineNumber);
                OTF2_CallbackCode calling_context (void *userData, OTF2_CallingContextRef self, OTF2_RegionRef region, OTF2_SourceCodeLocationRef sourceCodeLocation, OTF2_CallingContextRef parent);
                OTF2_CallbackCode calling_context_property (void *userData, OTF2_CallingContextRef callingContext, OTF2_StringRef name, OTF2_Type type, OTF2_AttributeValue value);
                OTF2_CallbackCode interrupt_generator (void *userData, OTF2_InterruptGeneratorRef self, OTF2_StringRef name, OTF2_InterruptGeneratorMode interruptGeneratorMode, OTF2_Base base, int64_t exponent, uint64_t period);

                OTF2_CallbackCode io_file (void *userData, OTF2_IoFileRef self, OTF2_StringRef file, OTF2_SystemTreeNodeRef scope);
                OTF2_CallbackCode io_handle (void *userData, OTF2_IoHandleRef self, OTF2_IoFileRef file, OTF2_IoParadigmRef ioParadigm, OTF2_IoHandleFlag ioHandleFlags, OTF2_CommRef comm, OTF2_IoHandleRef parent);

                OTF2_CallbackCode unknown  (void *userData);

                // clang-format on
            }
        }
    }
}
}

#endif // INCLUDE_OTF2XX_READER_CALLBACK_FUNCS_HPP
