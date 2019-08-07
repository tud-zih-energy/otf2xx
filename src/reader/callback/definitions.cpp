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
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::attribute>(
                        self, registry.get<otf2::definition::string>(name),
                        registry.get<otf2::definition::string>(description),
                        static_cast<otf2::definition::attribute::attribute_type>(type));

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode call_path(void* userData, OTF2_CallpathRef self,
                                            OTF2_CallpathRef parent, OTF2_RegionRef region)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    // This *should* even work for UNDEFINED parents
                    const auto& def = registry.create<otf2::definition::call_path>(
                        self, registry.get<otf2::definition::region>(region),
                        registry.get<otf2::definition::call_path>(parent));

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode call_path_parameter(void* userData, OTF2_CallpathRef callpath,
                                                      OTF2_ParameterRef parameter, OTF2_Type type,
                                                      OTF2_AttributeValue value)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::call_path_parameter>(
                        registry.get<otf2::definition::call_path>(callpath),
                        registry.get<otf2::definition::parameter>(parameter),
                        static_cast<otf2::definition::call_path_parameter::type_type>(type),
                        static_cast<otf2::definition::call_path_parameter::value_type>(value));

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode call_site(void* userData, OTF2_CallsiteRef self,
                                            OTF2_StringRef sourceFile, uint32_t lineNumber,
                                            OTF2_RegionRef enteredRegion, OTF2_RegionRef leftRegion)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::call_site>(
                        self, registry.get<otf2::definition::string>(sourceFile), lineNumber,
                        registry.get<otf2::definition::region>(enteredRegion),
                        registry.get<otf2::definition::region>(leftRegion));

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

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
                    auto& registry = reader->registry();

                    // Fuck it yeah, C++14 lambda paramter auto deduction for da prezz
                    auto process = [&](const auto& group) {
                        const auto& c = registry.create<otf2::definition::comm>(
                            self, registry.get<otf2::definition::string>(name), group,
                            registry.get<otf2::definition::comm>(parent));

                        reader->callback().definition(c);
                    };

                    if (registry.has<otf2::definition::comm_group>(group))
                    {
                        process(registry.get<otf2::definition::comm_group>(group));
                    }
                    else
                    {
                        process(registry.get<otf2::definition::comm_self_group>(group));
                    }

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode group(void* userData, OTF2_GroupRef self, OTF2_StringRef name,
                                        OTF2_GroupType groupType, OTF2_Paradigm paradigm,
                                        OTF2_GroupFlag groupFlags, uint32_t numberOfMembers,
                                        const uint64_t* members)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    switch (groupType)
                    {
                    case OTF2_GROUP_TYPE_LOCATIONS:
                    {
                        auto& lsg = registry.create<otf2::definition::locations_group>(
                            self, registry.get<otf2::definition::string>(name),
                            static_cast<otf2::common::paradigm_type>(paradigm),
                            static_cast<otf2::common::group_flag_type>(groupFlags));

                        for (std::uint32_t i = 0; i < numberOfMembers; ++i)
                        {
                            lsg.add_member(registry.get<otf2::definition::location>(members[i]));
                        }

                        reader->callback().definition(lsg);
                    }
                    break;

                    case OTF2_GROUP_TYPE_REGIONS:
                    {
                        auto& rg = registry.create<otf2::definition::regions_group>(
                            self, registry.get<otf2::definition::string>(name),
                            static_cast<otf2::common::paradigm_type>(paradigm),
                            static_cast<otf2::common::group_flag_type>(groupFlags));

                        for (std::uint32_t i = 0; i < numberOfMembers; ++i)
                        {
                            rg.add_member(registry.get<otf2::definition::region>(members[i]));
                        }

                        reader->callback().definition(rg);
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
                        auto& cg = registry.create<otf2::definition::comm_group>(
                            self, registry.get<otf2::definition::string>(name),
                            static_cast<otf2::common::paradigm_type>(paradigm),
                            static_cast<otf2::common::group_flag_type>(groupFlags));

                        // search the correspondig group :O
                        bool found = false;

                        // TODO use some key sorting searchy thingy of the registry? maybe?
                        for (const auto& group :
                             registry.all<otf2::definition::comm_locations_group>())
                        {
                            if (group.paradigm() ==
                                static_cast<otf2::common::paradigm_type>(paradigm))
                            {
                                assert(!found);
                                found = true;

                                for (std::uint32_t i = 0; i < numberOfMembers; ++i)
                                {
                                    cg.add_member(group[members[i]]);
                                }
                            }
                        }

                        // check that there was actually a comm locations group found
                        assert(found);

                        reader->callback().definition(cg);
                    }
                    break;

                    case OTF2_GROUP_TYPE_COMM_LOCATIONS:
                    {
                        auto& clg = registry.create<otf2::definition::comm_locations_group>(
                            self, registry.get<otf2::definition::string>(name),
                            static_cast<otf2::common::paradigm_type>(paradigm),
                            static_cast<otf2::common::group_flag_type>(groupFlags));

                        for (std::uint32_t i = 0; i < numberOfMembers; ++i)
                        {
                            clg.add_member(registry.get<otf2::definition::location>(members[i]));
                        }

                        reader->callback().definition(clg);
                    }
                    break;

                    case OTF2_GROUP_TYPE_COMM_SELF:
                    {
                        const auto& csg = registry.create<otf2::definition::comm_self_group>(
                            self, registry.get<otf2::definition::string>(name),
                            static_cast<otf2::common::paradigm_type>(paradigm),
                            static_cast<otf2::common::group_flag_type>(groupFlags));

                        reader->callback().definition(csg);
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
                    auto& registry = reader->registry();

                    const auto& loc = registry.create<otf2::definition::location>(
                        self, registry.get<otf2::definition::string>(name),
                        registry.get<otf2::definition::location_group>(locationGroup),
                        static_cast<otf2::definition::location::location_type>(locationType),
                        numberOfEvents);

                    reader->callback().definition(loc);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode location_group(void* userData, OTF2_LocationGroupRef self,
                                                 OTF2_StringRef name,
                                                 OTF2_LocationGroupType locationGroupType,
                                                 OTF2_SystemTreeNodeRef systemTreeParent)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& lg = registry.create<otf2::definition::location_group>(
                        self, registry.get<otf2::definition::string>(name),
                        static_cast<otf2::definition::location_group::location_group_type>(
                            locationGroupType),
                        registry.get<otf2::definition::system_tree_node>(systemTreeParent));

                    reader->callback().definition(lg);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode metric_class(void* userData, OTF2_MetricRef self,
                                               uint8_t numberOfMetrics,
                                               const OTF2_MetricMemberRef* metricMembers,
                                               OTF2_MetricOccurrence metricOccurrence,
                                               OTF2_RecorderKind recorderKind)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    auto& mc = registry.create<otf2::definition::metric_class>(
                        self, static_cast<otf2::common::metric_occurence>(metricOccurrence),
                        static_cast<otf2::common::recorder_kind>(recorderKind));

                    for (std::size_t i = 0; i < numberOfMetrics; i++)
                    {
                        mc.add_member(
                            registry.get<otf2::definition::metric_member>(metricMembers[i]));
                    }

                    reader->callback().definition(mc);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode metric_class_recorder(void* userData, OTF2_MetricRef metric,
                                                        OTF2_LocationRef recorder)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    if (registry.has<otf2::definition::metric_class>(metric))
                    {
                        const auto& def = registry.create<otf2::definition::metric_class_recorder>(
                            registry.get<otf2::definition::metric_class>(metric),
                            registry.get<otf2::definition::location>(recorder));

                        reader->callback().definition(def);
                    }
                    else
                    {
                        const auto& def = registry.create<otf2::definition::metric_class_recorder>(
                            registry.get<otf2::definition::metric_instance>(metric),
                            registry.get<otf2::definition::location>(recorder));

                        reader->callback().definition(def);
                    }

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode metric_instance(void* userData, OTF2_MetricRef self,
                                                  OTF2_MetricRef metricClass,
                                                  OTF2_LocationRef recorder,
                                                  OTF2_MetricScope metricScope, uint64_t scope)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    // I love C++14 auto lambda paramters <3 <3 <3
                    auto process = [&](const auto& scope) {
                        const auto& mi = registry.create<otf2::definition::metric_instance>(
                            self, registry.get<otf2::definition::metric_class>(metricClass),
                            registry.get<otf2::definition::location>(recorder), scope);
                        reader->callback().definition(mi);
                    };

                    switch (metricScope)
                    {
                    case OTF2_SCOPE_LOCATION:
                        process(registry.get<otf2::definition::location>(scope));
                        break;

                    case OTF2_SCOPE_LOCATION_GROUP:
                        process(registry.get<otf2::definition::location_group>(scope));
                        break;

                    case OTF2_SCOPE_SYSTEM_TREE_NODE:
                        process(registry.get<otf2::definition::system_tree_node>(scope));
                        break;

                    case OTF2_SCOPE_GROUP:
                        process(registry.get<otf2::definition::locations_group>(scope));
                        break;

                    default:
                        make_exception("Unknown scope type for metric instance given");
                    }

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
                    auto& registry = reader->registry();

                    const auto& mm = registry.create<otf2::definition::metric_member>(
                        self, registry.get<otf2::definition::string>(name),
                        registry.get<otf2::definition::string>(description),
                        static_cast<otf2::common::metric_type>(metricType),
                        static_cast<otf2::common::metric_mode>(metricMode),
                        static_cast<otf2::common::type>(valueType),
                        static_cast<otf2::common::base_type>(metricBase),
                        static_cast<std::int64_t>(exponent),
                        registry.get<otf2::definition::string>(unit));

                    reader->callback().definition(mm);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode parameter(void* userData, OTF2_ParameterRef self,
                                            OTF2_StringRef name, OTF2_ParameterType parameterType)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& para = registry.create<otf2::definition::parameter>(
                        self, registry.get<otf2::definition::string>(name),
                        static_cast<otf2::definition::parameter::parameter_type>(parameterType));

                    reader->callback().definition(para);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode region(void* userData, OTF2_RegionRef self, OTF2_StringRef name,
                                         OTF2_StringRef canonicalName, OTF2_StringRef description,
                                         OTF2_RegionRole regionRole, OTF2_Paradigm paradigm,
                                         OTF2_RegionFlag regionFlags, OTF2_StringRef sourceFile,
                                         uint32_t beginLineNumber, uint32_t endLineNumber)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& r = registry.create<otf2::definition::region>(
                        self, registry.get<otf2::definition::string>(name),
                        registry.get<otf2::definition::string>(canonicalName),
                        registry.get<otf2::definition::string>(description),
                        static_cast<otf2::definition::region::role_type>(regionRole),
                        static_cast<otf2::definition::region::paradigm_type>(paradigm),
                        static_cast<otf2::definition::region::flags_type>(regionFlags),
                        registry.get<otf2::definition::string>(sourceFile), beginLineNumber,
                        endLineNumber);

                    reader->callback().definition(r);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode rma_win(void* userData, OTF2_RmaWinRef self, OTF2_StringRef name,
                                          OTF2_CommRef comm)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& rw = registry.create<otf2::definition::rma_win>(
                        self, registry.get<otf2::definition::string>(name),
                        registry.get<otf2::definition::comm>(comm));

                    reader->callback().definition(rw);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode string(void* userData, OTF2_StringRef self, const char* string)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& str = registry.create<otf2::definition::string>(self, string);

                    reader->callback().definition(str);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode system_tree_node(void* userData, OTF2_SystemTreeNodeRef self,
                                                   OTF2_StringRef name, OTF2_StringRef className,
                                                   OTF2_SystemTreeNodeRef parent)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    // This *should* even work for UNDEFINED parents
                    const auto& stn = registry.create<otf2::definition::system_tree_node>(
                        self, registry.get<otf2::definition::string>(name),
                        registry.get<otf2::definition::string>(className),
                        registry.get<otf2::definition::system_tree_node>(parent));

                    reader->callback().definition(stn);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode system_tree_node_domain(void* userData,
                                                          OTF2_SystemTreeNodeRef systemTreeNode,
                                                          OTF2_SystemTreeDomain systemTreeDomain)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& stnd = registry.create<otf2::definition::system_tree_node_domain>(
                        registry.get<otf2::definition::system_tree_node>(systemTreeNode),
                        static_cast<otf2::definition::system_tree_node_domain::
                                        system_tree_node_domain_type>(systemTreeDomain));

                    reader->callback().definition(stnd);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode system_tree_node_property(void* userData,
                                                            OTF2_SystemTreeNodeRef systemTreeNode,
                                                            OTF2_StringRef name, OTF2_Type type,
                                                            OTF2_AttributeValue value)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::system_tree_node_property>(
                        registry.get<otf2::definition::system_tree_node>(systemTreeNode),
                        registry.get<otf2::definition::string>(name),
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
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::location_property>(
                        registry.get<otf2::definition::location>(location),
                        registry.get<otf2::definition::string>(name),
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
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::location_group_property>(
                        registry.get<otf2::definition::location_group>(locationGroup),
                        registry.get<otf2::definition::string>(name),
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
                    auto& registry = reader->registry();

                    const auto& scl = registry.create<otf2::definition::source_code_location>(
                        self, registry.get<otf2::definition::string>(file), lineNumber);

                    reader->callback().definition(scl);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode calling_context(void* userData, OTF2_CallingContextRef self,
                                                  OTF2_RegionRef region,
                                                  OTF2_SourceCodeLocationRef sourceCodeLocation,
                                                  OTF2_CallingContextRef parent)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& cc = registry.create<otf2::definition::calling_context>(
                        self, registry.get<otf2::definition::region>(region),
                        registry.get<otf2::definition::source_code_location>(sourceCodeLocation),
                        registry.get<otf2::definition::calling_context>(parent));

                    reader->callback().definition(cc);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode calling_context_property(void* userData,
                                                           OTF2_CallingContextRef callingContext,
                                                           OTF2_StringRef name, OTF2_Type type,
                                                           OTF2_AttributeValue value)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::calling_context_property>(
                        registry.get<otf2::definition::calling_context>(callingContext),
                        registry.get<otf2::definition::string>(name),
                        static_cast<otf2::definition::calling_context_property::type_type>(type),
                        static_cast<otf2::definition::calling_context_property::value_type>(value));

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode
                interrupt_generator(void* userData, OTF2_InterruptGeneratorRef self,
                                    OTF2_StringRef name,
                                    OTF2_InterruptGeneratorMode interruptGeneratorMode,
                                    OTF2_Base base, int64_t exponent, uint64_t period)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& ig = registry.create<otf2::definition::interrupt_generator>(
                        self, registry.get<otf2::definition::string>(name),
                        static_cast<
                            otf2::definition::interrupt_generator::interrupt_generator_mode_type>(
                            interruptGeneratorMode),
                        static_cast<otf2::definition::interrupt_generator::base_type>(base),
                        exponent, period);

                    reader->callback().definition(ig);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode io_regular_file(void* userData, OTF2_IoFileRef self,
                                                  OTF2_StringRef file, OTF2_SystemTreeNodeRef scope)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::io_regular_file>(
                        self, registry.get<otf2::definition::string>(file),
                        registry.get<otf2::definition::system_tree_node>(scope));

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode io_directory(void* userData, OTF2_IoFileRef self,
                                               OTF2_StringRef file, OTF2_SystemTreeNodeRef scope)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::io_directory>(
                        self, registry.get<otf2::definition::string>(file),
                        registry.get<otf2::definition::system_tree_node>(scope));

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
                    auto& registry = reader->registry();

                    auto process = [&](const auto& file) {
                        const auto& ioh = registry.create<otf2::definition::io_handle>(
                            self, registry.get<otf2::definition::string>(name), file,
                            registry.get<otf2::definition::io_paradigm>(ioParadigm),
                            static_cast<otf2::definition::io_handle::io_handle_flag_type>(
                                ioHandleFlags),
                            registry.get<otf2::definition::comm>(comm),
                            registry.get<otf2::definition::io_handle>(parent));

                        reader->callback().definition(ioh);
                    };

                    if (registry.has<otf2::definition::io_regular_file>(file))
                    {
                        process(registry.get<otf2::definition::io_regular_file>(file));
                    }
                    else
                    {
                        process(registry.get<otf2::definition::io_directory>(file));
                    }

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
                    auto& registry = reader->registry();

                    std::vector<otf2::common::io_paradigm_property_type> props;
                    std::vector<otf2::attribute_value> vals;

                    for (uint8_t i = 0; i < numberOfProperties; ++i)
                    {
                        props.push_back(
                            static_cast<otf2::common::io_paradigm_property_type>(properties[i]));
                        vals.emplace_back(static_cast<otf2::common::type>(types[i]), values[i]);
                    }

                    const auto& iop = registry.create<otf2::definition::io_paradigm>(
                        self, registry.get<otf2::definition::string>(identification),
                        registry.get<otf2::definition::string>(name),
                        static_cast<otf2::definition::io_paradigm::paradigm_class_type>(
                            ioParadigmClass),
                        static_cast<otf2::definition::io_paradigm::paradigm_flag_type>(
                            ioParadigmFlags),
                        props, vals);

                    reader->callback().definition(iop);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode io_file_property(void* userData, OTF2_IoFileRef ioFile,
                                                   OTF2_StringRef name, OTF2_Type type,
                                                   OTF2_AttributeValue value)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    auto process = [&](const auto& file) {
                        const auto& def = registry.create<otf2::definition::io_file_property>(
                            file, registry.get<otf2::definition::string>(name),
                            static_cast<otf2::definition::io_file_property::type_type>(type),
                            static_cast<otf2::definition::io_file_property::value_type>(value));

                        reader->callback().definition(def);
                    };

                    if (registry.has<otf2::definition::io_regular_file>(ioFile))
                    {
                        process(registry.get<otf2::definition::io_regular_file>(ioFile));
                    }
                    else
                    {
                        process(registry.get<otf2::definition::io_directory>(ioFile));
                    }

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode io_pre_created_handle_state(void* userData,
                                                              OTF2_IoHandleRef ioHandle,
                                                              OTF2_IoAccessMode mode,
                                                              OTF2_IoStatusFlag statusFlags)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& self =
                        registry.create<otf2::definition::io_pre_created_handle_state>(
                            registry.get<otf2::definition::io_handle>(ioHandle),
                            static_cast<
                                otf2::definition::io_pre_created_handle_state::access_mode_type>(
                                mode),
                            static_cast<
                                otf2::definition::io_pre_created_handle_state::status_flag_type>(
                                statusFlags));

                    reader->callback().definition(self);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode cart_dimension(void* userData, OTF2_CartDimensionRef self,
                                                 OTF2_StringRef name, uint32_t size,
                                                 OTF2_CartPeriodicity cartPeriodicity)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::cart_dimension>(
                        self, registry.get<otf2::definition::string>(name), size,
                        static_cast<bool>(cartPeriodicity));

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode cart_topology(void* userData, OTF2_CartTopologyRef self,
                                                OTF2_StringRef name, OTF2_CommRef communicator,
                                                uint8_t numberOfDimensions,
                                                const OTF2_CartDimensionRef* cartDimensions)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    auto& def = registry.create<otf2::definition::cart_topology>(
                        self, registry.get<otf2::definition::string>(name),
                        registry.get<otf2::definition::comm>(communicator));

                    for (uint8_t i = 0; i < numberOfDimensions; ++i)
                    {
                        def.add_dimension(
                            registry.get<otf2::definition::cart_dimension>(cartDimensions[i]));
                    }

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode cart_coordinate(void* userData, OTF2_CartTopologyRef cartTopology,
                                                  uint32_t rank, uint8_t numberOfDimensions,
                                                  const uint32_t* coordinates)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);
                    auto& registry = reader->registry();

                    const auto& def = registry.create<otf2::definition::cart_coordinate>(
                        registry.get<otf2::definition::cart_topology>(cartTopology), rank,
                        std::vector<std::uint32_t>{ coordinates,
                                                    coordinates + numberOfDimensions });

                    reader->callback().definition(def);

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }

                OTF2_CallbackCode unknown(void* userData)
                {
                    otf2::reader::reader* reader = static_cast<otf2::reader::reader*>(userData);

                    reader->callback().definition(otf2::definition::unknown());

                    return static_cast<OTF2_CallbackCode>(OTF2_SUCCESS);
                }
            } // namespace global
        }     // namespace definition
    }         // namespace detail
} // namespace reader
} // namespace otf2
