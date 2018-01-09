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
        namespace definition
        {
            namespace global
            {

                OTF2_CallbackCode attribute(void* userData, OTF2_AttributeRef self,
                                            OTF2_StringRef name, OTF2_StringRef description,
                                            OTF2_Type type)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->attributes().add_definition(
                        { self, reader->strings()[name], reader->strings()[description],
                          static_cast<otf2::definition::attribute::attribute_type>(type) });

                    reader->callback().definition(reader->attributes()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                //  OTF2_CallbackCode Callpath  (void *userData, OTF2_CallpathRef self,
                // OTF2_CallpathRef parent, OTF2_RegionRef region);
                //  OTF2_CallbackCode Callsite  (void *userData, OTF2_CallsiteRef self,
                // OTF2_StringRef sourceFile, uint32_t lineNumber, OTF2_RegionRef enteredRegion,
                // OTF2_RegionRef leftRegion);

                OTF2_CallbackCode clock_properties(void* userData, uint64_t timerResolution,
                                                   uint64_t globalOffset, uint64_t traceLength)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    std::unique_ptr<otf2::definition::clock_properties> cp(
                        new otf2::definition::clock_properties(otf2::chrono::ticks(timerResolution),
                                                               otf2::chrono::ticks(globalOffset),
                                                               otf2::chrono::ticks(traceLength)));

                    reader->set_clock_properties(std::move(cp));

                    reader->callback().definition(reader->clock_properties());

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode comm(void* userData, OTF2_CommRef self, OTF2_StringRef name,
                                       OTF2_GroupRef group, OTF2_CommRef parent)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    if (parent != OTF2_UNDEFINED_COMM)
                    {
                        if (reader->comm_groups().count(group))
                        {
                            reader->comms().add_definition({ self, reader->strings()[name],
                                                             reader->comm_groups()[group],
                                                             reader->comms()[parent] });
                        }
                        else
                        {
                            reader->comms().add_definition({ self, reader->strings()[name],
                                                             reader->comm_self_groups()[group],
                                                             reader->comms()[parent] });
                        }
                    }
                    else
                    {
                        if (reader->comm_groups().count(group))
                        {
                            reader->comms().add_definition(
                                { self, reader->strings()[name], reader->comm_groups()[group] });
                        }
                        else
                        {
                            reader->comms().add_definition({ self, reader->strings()[name],
                                                             reader->comm_self_groups()[group] });
                        }
                    }

                    reader->callback().definition(reader->comms()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode group(void* userData, OTF2_GroupRef self, OTF2_StringRef name,
                                        OTF2_GroupType groupType, OTF2_Paradigm paradigm,
                                        OTF2_GroupFlag groupFlags, uint32_t numberOfMembers,
                                        const uint64_t* members)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    switch (groupType)
                    {
                    case OTF2_GROUP_TYPE_LOCATIONS:
                    {
                        otf2::definition::locations_group lsg(
                            self, reader->strings()[name],
                            static_cast<otf2::common::paradigm_type>(paradigm),
                            static_cast<otf2::common::group_flag_type>(groupFlags));

                        for (std::uint32_t i = 0; i < numberOfMembers; ++i)
                            lsg.add_member(reader->locations()[members[i]]);

                        reader->locations_groups().add_definition(lsg);

                        reader->callback().definition(reader->location_groups()[self]);
                    }
                    break;

                    case OTF2_GROUP_TYPE_REGIONS:
                    {
                        otf2::definition::regions_group gr(
                            self, reader->strings()[name],
                            static_cast<otf2::common::paradigm_type>(paradigm),
                            static_cast<otf2::common::group_flag_type>(groupFlags));

                        for (std::uint32_t i = 0; i < numberOfMembers; ++i)
                            gr.add_member(reader->regions()[members[i]]);

                        reader->regions_groups().add_definition(gr);

                        reader->callback().definition(reader->regions_groups()[self]);
                    }
                    break;

                    // TODO: implement Metrics
                    //             case OTF2_GROUP_TYPE_METRIC:
                    //             {
                    //                 otf2::definition::metric_group gr(self,
                    //                                                       reader->strings()[name],
                    //                                                       static_cast<otf2::common::paradigm_type>(paradigm),
                    //                                                       static_cast<otf2::common::group_flag_type>(groupFlags)
                    //                 );
                    //
                    //                 for(std::uint32_t i=0; i< numberOfMembers; ++i)
                    //                     gr.add_member(reader->metrics()[members[i]]);
                    //
                    //                 reader->metric_groups().add_definition(std::move(gr));
                    //             }
                    //             break;

                    case OTF2_GROUP_TYPE_COMM_GROUP:
                    {
                        otf2::definition::comm_group lsg(
                            self, reader->strings()[name],
                            static_cast<otf2::common::paradigm_type>(paradigm),
                            static_cast<otf2::common::group_flag_type>(groupFlags));

                        // find correspondig group :O
                        otf2::definition::comm_locations_group cgroup;
                        for (auto group : reader->comm_locations_groups())
                        {
                            if (group.paradigm() ==
                                static_cast<otf2::common::paradigm_type>(paradigm))
                            {
                                cgroup = group;
                            }
                        }

                        assert(cgroup.is_valid());

                        for (std::uint32_t i = 0; i < numberOfMembers; ++i)
                            lsg.add_member(cgroup[members[i]]);

                        reader->comm_groups().add_definition(lsg);

                        reader->callback().definition(reader->comm_groups()[self]);
                    }
                    break;

                    case OTF2_GROUP_TYPE_COMM_LOCATIONS:
                    {
                        otf2::definition::comm_locations_group lsg(
                            self, reader->strings()[name],
                            static_cast<otf2::common::paradigm_type>(paradigm),
                            static_cast<otf2::common::group_flag_type>(groupFlags));

                        for (std::uint32_t i = 0; i < numberOfMembers; ++i)
                            lsg.add_member(reader->locations()[members[i]]);

                        reader->comm_locations_groups().add_definition(lsg);

                        reader->callback().definition(reader->comm_locations_groups()[self]);
                    }
                    break;

                    case OTF2_GROUP_TYPE_COMM_SELF:
                    {
                        otf2::definition::comm_self_group lsg(
                            self, reader->strings()[name],
                            static_cast<otf2::common::paradigm_type>(paradigm),
                            static_cast<otf2::common::group_flag_type>(groupFlags));

                        reader->comm_self_groups().add_definition(lsg);

                        reader->callback().definition(reader->comm_self_groups()[self]);
                    }
                    break;

                    case OTF2_GROUP_TYPE_UNKNOWN:
                    default:
                        make_exception("Unknown group type isn't supported");
                        break;
                    }

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode location(void* userData, OTF2_LocationRef self,
                                           OTF2_StringRef name, OTF2_LocationType locationType,
                                           uint64_t numberOfEvents,
                                           OTF2_LocationGroupRef locationGroup)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->locations().add_definition(
                        { self, reader->strings()[name], reader->location_groups()[locationGroup],
                          static_cast<otf2::definition::location::location_type>(locationType),
                          numberOfEvents });

                    reader->callback().definition(reader->locations()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode location_group(void* userData, OTF2_LocationGroupRef self,
                                                 OTF2_StringRef name,
                                                 OTF2_LocationGroupType locationGroupType,
                                                 OTF2_SystemTreeNodeRef systemTreeParent)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->location_groups().add_definition(
                        { self, reader->strings()[name],
                          static_cast<otf2::definition::location_group::location_group_type>(
                              locationGroupType),
                          reader->system_tree_nodes()[systemTreeParent] });

                    reader->callback().definition(reader->location_groups()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode metric_class(void* userData, OTF2_MetricRef self,
                                               uint8_t numberOfMetrics,
                                               const OTF2_MetricMemberRef* metricMembers,
                                               OTF2_MetricOccurrence metricOccurrence,
                                               OTF2_RecorderKind recorderKind)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->metric_classes().add_definition(
                        { self, static_cast<otf2::common::metric_occurence>(metricOccurrence),
                          static_cast<otf2::common::recorder_kind>(recorderKind) });

                    auto mc = reader->metric_classes()[self];

                    for (std::size_t i = 0; i < numberOfMetrics; i++)
                    {
                        mc.add_member(reader->metric_members()[metricMembers[i]]);
                    }

                    reader->callback().definition(std::move(mc));

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }
                //  OTF2_CallbackCode MetricClassRecorder  (void *userData, OTF2_MetricRef
                // metricClass, OTF2_LocationRef recorder);

                OTF2_CallbackCode metric_instance(void* userData, OTF2_MetricRef self,
                                                  OTF2_MetricRef metricClass,
                                                  OTF2_LocationRef recorder,
                                                  OTF2_MetricScope metricScope, uint64_t scope)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    switch (metricScope)
                    {
                    case OTF2_SCOPE_LOCATION:
                        reader->metric_instances().add_definition(
                            { self, reader->metric_classes()[metricClass],
                              reader->locations()[recorder], reader->locations()[scope] });
                        break;

                    case OTF2_SCOPE_LOCATION_GROUP:
                        reader->metric_instances().add_definition(
                            { self, reader->metric_classes()[metricClass],
                              reader->locations()[recorder], reader->location_groups()[scope] });
                        break;

                    case OTF2_SCOPE_SYSTEM_TREE_NODE:
                        reader->metric_instances().add_definition(
                            { self, reader->metric_classes()[metricClass],
                              reader->locations()[recorder], reader->system_tree_nodes()[scope] });
                        break;

                    case OTF2_SCOPE_GROUP:
                        reader->metric_instances().add_definition(
                            { self, reader->metric_classes()[metricClass],
                              reader->locations()[recorder], reader->locations_groups()[scope] });
                        break;

                    default:
                        make_exception("Unknown scope type for metric instance given");
                    }

                    reader->callback().definition(reader->metric_instances()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode metric_member(void* userData, OTF2_MetricMemberRef self,
                                                OTF2_StringRef name, OTF2_StringRef description,
                                                OTF2_MetricType metricType,
                                                OTF2_MetricMode metricMode, OTF2_Type valueType,
                                                OTF2_Base metricBase, int64_t exponent,
                                                OTF2_StringRef unit)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->metric_members().add_definition(
                        { self, reader->strings()[name], reader->strings()[description],
                          static_cast<otf2::common::metric_type>(metricType),
                          static_cast<otf2::common::metric_mode>(metricMode),
                          static_cast<otf2::common::type>(valueType),
                          static_cast<otf2::common::base_type>(metricBase),
                          static_cast<std::int64_t>(exponent), reader->strings()[unit] });

                    reader->callback().definition(reader->metric_members()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode parameter(void* userData, OTF2_ParameterRef self,
                                            OTF2_StringRef name, OTF2_ParameterType parameterType)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->parameters().add_definition(
                        { self, reader->strings()[name],
                          static_cast<otf2::definition::parameter::parameter_type>(
                              parameterType) });

                    reader->callback().definition(reader->parameters()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode region(void* userData, OTF2_RegionRef self, OTF2_StringRef name,
                                         OTF2_StringRef canonicalName, OTF2_StringRef description,
                                         OTF2_RegionRole regionRole, OTF2_Paradigm paradigm,
                                         OTF2_RegionFlag regionFlags, OTF2_StringRef sourceFile,
                                         uint32_t beginLineNumber, uint32_t endLineNumber)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->regions().add_definition(
                        { self, reader->strings()[name], reader->strings()[canonicalName],
                          reader->strings()[description],
                          static_cast<otf2::definition::region::role_type>(regionRole),
                          static_cast<otf2::definition::region::paradigm_type>(paradigm),
                          static_cast<otf2::definition::region::flags_type>(regionFlags),
                          reader->strings()[sourceFile], beginLineNumber, endLineNumber });

                    reader->callback().definition(reader->regions()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                //  OTF2_CallbackCode RmaWin  (void *userData, OTF2_RmaWinRef self,
                // OTF2_StringRef name, OTF2_CommRef comm){ return
                // static_cast<OTF2_CallbackCode>(OTF2_SUCCESS); }

                OTF2_CallbackCode string(void* userData, OTF2_StringRef self, const char* string)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->strings().add_definition({ self, string });

                    reader->callback().definition(reader->strings()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode system_tree_node(void* userData, OTF2_SystemTreeNodeRef self,
                                                   OTF2_StringRef name, OTF2_StringRef className,
                                                   OTF2_SystemTreeNodeRef parent)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    if (parent != OTF2_UNDEFINED_SYSTEM_TREE_NODE)
                    {
                        reader->system_tree_nodes().add_definition(
                            { self, reader->strings()[name], reader->strings()[className],
                              reader->system_tree_nodes()[parent] });
                    }
                    else
                    {
                        reader->system_tree_nodes().add_definition(
                            { self, reader->strings()[name], reader->strings()[className] });
                    }

                    reader->callback().definition(reader->system_tree_nodes()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                //  OTF2_CallbackCode SystemTreeNodeDomain  (void *userData,
                // OTF2_SystemTreeNodeRef systemTreeNode, OTF2_SystemTreeDomain systemTreeDomain){
                // return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS); }

                OTF2_CallbackCode system_tree_node_property(void* userData,
                                                            OTF2_SystemTreeNodeRef systemTreeNode,
                                                            OTF2_StringRef name, OTF2_Type type,
                                                            OTF2_AttributeValue value)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    const auto& def = reader->system_tree_node_properties().emplace(
                        reader->system_tree_nodes()[systemTreeNode], reader->strings()[name],
                        static_cast<otf2::definition::system_tree_node_property::type_type>(type),
                        static_cast<otf2::definition::system_tree_node_property::value_type>(
                            value));

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode location_property(void* userData, OTF2_LocationRef location,
                                                    OTF2_StringRef name, OTF2_Type type,
                                                    OTF2_AttributeValue value)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    const auto& def = reader->location_properties().emplace(
                        reader->locations()[location], reader->strings()[name],
                        static_cast<otf2::definition::location_property::type_type>(type),
                        static_cast<otf2::definition::location_property::value_type>(value));

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode location_group_property(void* userData,
                                                          OTF2_LocationGroupRef locationGroup,
                                                          OTF2_StringRef name, OTF2_Type type,
                                                          OTF2_AttributeValue value)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    const auto& def = reader->location_group_properties().emplace(
                        reader->location_groups()[locationGroup], reader->strings()[name],
                        static_cast<otf2::definition::location_group_property::type_type>(type),
                        static_cast<otf2::definition::location_group_property::value_type>(value));

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode source_code_location(void* userData,
                                                       OTF2_SourceCodeLocationRef self,
                                                       OTF2_StringRef file, uint32_t lineNumber)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->source_code_locations().emplace(self, reader->strings()[file],
                                                            lineNumber);

                    reader->callback().definition(reader->source_code_locations()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode calling_context(void* userData, OTF2_CallingContextRef self,
                                                  OTF2_RegionRef region,
                                                  OTF2_SourceCodeLocationRef sourceCodeLocation,
                                                  OTF2_CallingContextRef parent)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    if (parent != OTF2_UNDEFINED_CALLING_CONTEXT)
                    {
                        reader->calling_contexts().add_definition(
                            { self, reader->regions()[region],
                              reader->source_code_locations()[sourceCodeLocation],
                              reader->calling_contexts()[parent] });
                    }
                    else
                    {
                        reader->calling_contexts().add_definition(
                            { self, reader->regions()[region],
                              reader->source_code_locations()[sourceCodeLocation] });
                    }

                    reader->callback().definition(reader->calling_contexts()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode calling_context_property(void* userData,
                                                           OTF2_CallingContextRef callingContext,
                                                           OTF2_StringRef name, OTF2_Type type,
                                                           OTF2_AttributeValue value)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    const auto& self = reader->calling_context_properties().emplace(
                        reader->calling_contexts()[callingContext], reader->strings()[name],
                        static_cast<otf2::definition::calling_context_property::type_type>(type),
                        static_cast<otf2::definition::calling_context_property::value_type>(value));

                    reader->callback().definition(self);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode
                interrupt_generator(void* userData, OTF2_InterruptGeneratorRef self,
                                    OTF2_StringRef name,
                                    OTF2_InterruptGeneratorMode interruptGeneratorMode,
                                    OTF2_Base base, int64_t exponent, uint64_t period)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->interrupt_generators().add_definition(
                        { self, reader->strings()[name],
                          static_cast<
                              otf2::definition::interrupt_generator::interrupt_generator_mode_type>(
                              interruptGeneratorMode),
                          static_cast<otf2::definition::interrupt_generator::base_type>(base),
                          exponent, period });

                    reader->callback().definition(reader->interrupt_generators()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode io_regular_file(void* userData, OTF2_IoFileRef self,
                                                  OTF2_StringRef file, OTF2_SystemTreeNodeRef scope)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    const auto& def = reader->io_regular_files().emplace(
                        self, reader->strings()[file], reader->system_tree_nodes()[scope]);

                    reader->io_files().add_definition(def);

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode io_directory(void* userData, OTF2_IoFileRef self,
                                               OTF2_StringRef file, OTF2_SystemTreeNodeRef scope)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    const auto& def = reader->io_directories().emplace(
                        self, reader->strings()[file], reader->system_tree_nodes()[scope]);

                    reader->io_files().add_definition(def);

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode io_handle(void* userData, OTF2_IoHandleRef self,
                                            OTF2_StringRef name, OTF2_IoFileRef file,
                                            OTF2_IoParadigmRef ioParadigm,
                                            OTF2_IoHandleFlag ioHandleFlags, OTF2_CommRef comm,
                                            OTF2_IoHandleRef parent)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    if (parent != OTF2_UNDEFINED_IO_HANDLE)
                    {
                        reader->io_handles().add_definition(
                            { self, reader->strings()[name], reader->io_files()[file],
                              reader->io_paradigms()[ioParadigm],
                              static_cast<otf2::definition::io_handle::io_handle_flag_type>(
                                  ioHandleFlags),
                              reader->comms()[comm], reader->io_handles()[parent] });
                    }
                    else
                    {
                        reader->io_handles().add_definition(
                            { self, reader->strings()[name], reader->io_files()[file],
                              reader->io_paradigms()[ioParadigm],
                              static_cast<otf2::definition::io_handle::io_handle_flag_type>(
                                  ioHandleFlags),
                              reader->comms()[comm] });
                    }

                    reader->callback().definition(reader->io_handles()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode
                io_paradigm(void* userData, OTF2_IoParadigmRef self, OTF2_StringRef identification,
                            OTF2_StringRef name, OTF2_IoParadigmClass ioParadigmClass,
                            OTF2_IoParadigmFlag ioParadigmFlags, uint8_t numberOfProperties,
                            const OTF2_IoParadigmProperty* properties, const OTF2_Type* types,
                            const OTF2_AttributeValue* values)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    std::vector<otf2::common::io_paradigm_property_type> props;
                    std::vector<otf2::attribute_value> vals;

                    for (uint8_t i = 0; i < numberOfProperties; ++i)
                    {
                        props.push_back(
                            static_cast<otf2::common::io_paradigm_property_type>(properties[i]));
                        vals.emplace_back(static_cast<otf2::common::type>(types[i]), values[i]);
                    }

                    reader->io_paradigms().add_definition(
                        { self, reader->strings()[identification], reader->strings()[name],
                          static_cast<otf2::definition::io_paradigm::paradigm_class_type>(
                              ioParadigmClass),
                          static_cast<otf2::definition::io_paradigm::paradigm_flag_type>(
                              ioParadigmFlags),
                          props, vals });

                    reader->callback().definition(reader->io_paradigms()[self]);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode io_file_property(void* userData, OTF2_IoFileRef ioFile,
                                                   OTF2_StringRef name, OTF2_Type type,
                                                   OTF2_AttributeValue value)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    const auto& self = reader->io_file_properties().emplace(
                        reader->io_files()[ioFile], reader->strings()[name],
                        static_cast<otf2::definition::io_file_property::type_type>(type),
                        static_cast<otf2::definition::io_file_property::value_type>(value));

                    reader->callback().definition(self);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode io_pre_created_handle_state(void* userData,
                                                              OTF2_IoHandleRef ioHandle,
                                                              OTF2_IoAccessMode mode,
                                                              OTF2_IoStatusFlag statusFlags)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    const auto& self = reader->io_pre_created_handle_states().emplace(
                        reader->io_handles()[ioHandle],
                        static_cast<
                            otf2::definition::io_pre_created_handle_state::access_mode_type>(mode),
                        static_cast<
                            otf2::definition::io_pre_created_handle_state::status_flag_type>(
                            statusFlags));

                    reader->callback().definition(self);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode unknown(void* userData)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->callback().definition(otf2::definition::unknown());

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }
            }
        }
    }
}
} // namespace otf2::reader::detail
