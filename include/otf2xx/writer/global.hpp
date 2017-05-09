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

#ifndef INCLUDE_OTF2XX_WRITER_GLOBAL_HPP
#define INCLUDE_OTF2XX_WRITER_GLOBAL_HPP

#include <otf2/OTF2_GlobalDefWriter.h>

#include <otf2xx/definition/definitions.hpp>
#include <otf2xx/event/marker.hpp>
#include <otf2xx/exception.hpp>
#include <otf2xx/traits/tuple_meta.hpp>

#include <algorithm>
#include <limits>

namespace otf2
{
namespace writer
{

    class global
    {
    public:
        global(OTF2_GlobalDefWriter* wrt, OTF2_MarkerWriter* marker_wrt)
        : wrt(wrt), marker_wrt_(marker_wrt)
        {
        }

        global(global&) = default;
        global& operator=(global&) = default;

        global(global&&) = default;
        global& operator=(global&&) = default;

    public:
        std::uint64_t num_definitions() const
        {
            std::uint64_t result;

            check(OTF2_GlobalDefWriter_GetNumberOfDefinitions(wrt, &result),
                  "Couldn't get the number of definitions");

            return result;
        }

        std::uint64_t num_locations() const
        {
            std::uint64_t result;

            check(OTF2_GlobalDefWriter_GetNumberOfLocations(wrt, &result),
                  "Couldn't get the number of locations");

            return result;
        }

    private:
        void store(const otf2::definition::attribute& data)
        {
            check(OTF2_GlobalDefWriter_WriteAttribute(wrt, data.ref(), data.name().ref(),
                                                      data.description().ref(),
                                                      static_cast<OTF2_Type>(data.type())),
                  "Couldn't write attribute to global definitions writer");
        }

        // TODO: implement this
        //         void store(const otf2::definition::callpath& data)
        //         {
        //             check(OTF2_GlobalDefWriter_WriteCallpath(wrt, data.ref(),
        // data.parent().ref(), data.region().ref()), "Couldn't write callpath
        // to global definitions writer");
        //         }
        //
        //         void store(const otf2::definition::callsite& data)
        //         {
        //             check(OTF2_GlobalDefWriter_WriteCallsite(wrt, data.ref(),
        // data.source_file().ref(), data.line(), data.entered().ref(),
        // data.leaved().ref()), "Couldn't write callsite to global definitions
        // writer");
        //         }

        void store(const otf2::definition::comm& data)
        {
            otf2::reference<otf2::definition::detail::group_base>::ref_type group_ref;

            if (data.has_self_group())
                group_ref = data.self_group().ref();
            else
                group_ref = data.group().ref();

            if (data.has_parent())
                check(OTF2_GlobalDefWriter_WriteComm(wrt, data.ref(), data.name().ref(), group_ref,
                                                     data.parent().ref()),
                      "Couldn't write comm to global definitions writer");
            else
                check(OTF2_GlobalDefWriter_WriteComm(
                          wrt, data.ref(), data.name().ref(), group_ref,
                          otf2::reference<otf2::definition::comm>::undefined()),
                      "Couldn't write comm to global definitions writer");
        }

        template <typename T, otf2::common::group_type GroupType>
        void store(const otf2::definition::group<T, GroupType>& data)
        {
            auto members = data.members();
            check(OTF2_GlobalDefWriter_WriteGroup(
                      wrt, data.ref(), data.name().ref(), static_cast<OTF2_GroupType>(data.type()),
                      static_cast<OTF2_Paradigm>(data.paradigm()),
                      static_cast<OTF2_GroupFlag>(data.group_flag()), data.size(), members.data()),
                  "Couldn't write group to global definitions writer");
        }

        void store(const otf2::definition::comm_group& data)
        {
            auto members = data.members();

            // find corresponding group
            otf2::definition::comm_locations_group cgroup;
            for (auto group : comm_locations_groups_)
            {
                if (group.paradigm() == data.paradigm())
                {
                    cgroup = group;
                }
            }

            assert(cgroup.is_valid());

            auto cmembers = cgroup.members();

            // FIXME O(n^2) doesn't scale to millions of cores
            // translate members relative to cgroup
            for (std::size_t i = 0; i < members.size(); ++i)
            {
                for (std::size_t j = 0; j < cmembers.size(); ++j)
                {
                    if (members[i] == cmembers[j])
                    {
                        members[i] = j;
                        break;
                    }
                }
            }

            check(OTF2_GlobalDefWriter_WriteGroup(
                      wrt, data.ref(), data.name().ref(), static_cast<OTF2_GroupType>(data.type()),
                      static_cast<OTF2_Paradigm>(data.paradigm()),
                      static_cast<OTF2_GroupFlag>(data.group_flag()), data.size(), members.data()),
                  "Couldn't write group to global definitions writer");
        }

        void store(const otf2::definition::location& data)
        {
            check(OTF2_GlobalDefWriter_WriteLocation(wrt, data.ref(), data.name().ref(),
                                                     static_cast<OTF2_LocationType>(data.type()),
                                                     data.num_events(),
                                                     data.location_group().ref()),
                  "Couldn't write location to global definitions writer");
        }

        void store(const otf2::definition::location_group& data)
        {
            check(OTF2_GlobalDefWriter_WriteLocationGroup(
                      wrt, data.ref(), data.name().ref(),
                      static_cast<OTF2_LocationGroupType>(data.type()), data.parent().ref()),
                  "Couldn't write location group to global definitions writer");
        }

        void store(const otf2::definition::metric_class& data)
        {
            std::vector<otf2::reference<otf2::definition::metric_member>::ref_type> members;

            members.reserve(data.size());

            for (std::size_t i = 0; i < data.size(); ++i)
            {
                members.push_back(data[i].ref());
            }

            check(OTF2_GlobalDefWriter_WriteMetricClass(
                      wrt, data.ref(), data.size(), members.data(),
                      static_cast<OTF2_MetricOccurrence>(data.occurence()),
                      static_cast<OTF2_RecorderKind>(data.recorder_kind())),
                  "Couldn't write metric class to global definitions writer");
        }

        void store(const otf2::definition::metric_instance& data)
        {
            uint64_t scope;

            typedef otf2::definition::metric_instance::metric_scope scope_type;

            switch (data.scope())
            {
            case scope_type::location:
                scope = data.location_scope().ref();
                break;

            case scope_type::location_group:
                scope = data.location_group_scope().ref();
                break;

            case scope_type::system_tree_node:
                scope = data.system_tree_node_scope().ref();
                break;

            case scope_type::group:
                scope = data.group_scope().ref();
                break;
            }

            check(OTF2_GlobalDefWriter_WriteMetricInstance(
                      wrt, data.ref(), data.metric_class().ref(), data.recorder().ref(),
                      static_cast<OTF2_MetricScope>(data.scope()), scope),
                  "Couldn't write metric instance to global definitions writer");
        }

        void store(const otf2::definition::metric_member& data)
        {
            check(OTF2_GlobalDefWriter_WriteMetricMember(
                      wrt, data.ref(), data.name().ref(), data.description().ref(),
                      static_cast<OTF2_MetricType>(data.type()),
                      static_cast<OTF2_MetricMode>(data.mode()),
                      static_cast<OTF2_Type>(data.value_type()),
                      static_cast<OTF2_Base>(data.value_base()), data.value_exponent(),
                      data.value_unit().ref()),
                  "Couldn't write metric member to global definitions writer");
        }

        void store(const otf2::definition::parameter& data)
        {
            check(OTF2_GlobalDefWriter_WriteParameter(wrt, data.ref(), data.name().ref(),
                                                      static_cast<OTF2_ParameterType>(data.type())),
                  "Couldn't write paramter to global definitions writer");
        }

        void store(const otf2::definition::region& data)
        {
            check(OTF2_GlobalDefWriter_WriteRegion(
                      wrt, data.ref(), data.name().ref(), data.canonical_name().ref(),
                      data.description().ref(), static_cast<OTF2_RegionRole>(data.role()),
                      static_cast<OTF2_Paradigm>(data.paradigm()),
                      static_cast<OTF2_RegionFlag>(data.flags()), data.source_file().ref(),
                      data.begin_line(), data.end_line()),
                  "Couldn't write region to global definitions writer");
        }

        //         void store(const otf2::definition::rma_win& data)
        //         {
        //             check(OTF2_GlobalDefWriter_WriteRmaWin(wrt, data.ref(),
        // data.name().ref(), data.comm().ref()), "Couldn't write RMA window to
        // global definitions writer");
        //         }

        void store(const otf2::definition::string& data)
        {
            check(OTF2_GlobalDefWriter_WriteString(wrt, data.ref(), data.str().c_str()),
                  "Couldn't write string to global definitions writer");
        }

        void store(const otf2::definition::system_tree_node& data)
        {
            if (data.has_parent())
                check(OTF2_GlobalDefWriter_WriteSystemTreeNode(wrt, data.ref(), data.name().ref(),
                                                               data.class_name().ref(),
                                                               data.parent().ref()),
                      "Couldn't write system tree node to global definitions "
                      "writer");
            else
                check(OTF2_GlobalDefWriter_WriteSystemTreeNode(
                          wrt, data.ref(), data.name().ref(), data.class_name().ref(),
                          otf2::reference<otf2::definition::system_tree_node>::undefined()),
                      "Couldn't write system tree node to global definitions "
                      "writer");
        }

        void store(const otf2::definition::clock_properties& data)
        {
            check(OTF2_GlobalDefWriter_WriteClockProperties(wrt, data.ticks_per_second().count(),
                                                            data.start_time().count(),
                                                            data.length().count()),
                  "Couldn't write clock properties to global definitions writer");
        }

        // TODO: implement this
        //         void store(const otf2::definition::system_tree_node_domain&
        // data)
        //         {
        //             check(OTF2_GlobalDefWriter_WriteSystemTreeNodeDomain(wrt,
        // data.ref()), "Couldn't write to global definitions writer");
        //         }

        void store(const otf2::definition::system_tree_node_property& data)
        {
            check(OTF2_GlobalDefWriter_WriteSystemTreeNodeProperty(
                      wrt, data.def().ref(), data.name().ref(), static_cast<OTF2_Type>(data.type()),
                      static_cast<OTF2_AttributeValue>(data.value())),
                  "Couldn't write to global definitions writer");
        }

        void store(const otf2::definition::location_property& data)
        {
            check(OTF2_GlobalDefWriter_WriteLocationProperty(
                      wrt, data.def().ref(), data.name().ref(), static_cast<OTF2_Type>(data.type()),
                      static_cast<OTF2_AttributeValue>(data.value())),
                  "Couldn't write to global definitions writer");
        }

        void store(const otf2::definition::location_group_property& data)
        {
            check(OTF2_GlobalDefWriter_WriteLocationGroupProperty(
                      wrt, data.def().ref(), data.name().ref(), static_cast<OTF2_Type>(data.type()),
                      static_cast<OTF2_AttributeValue>(data.value())),
                  "Couldn't write to global definitions writer");
        }

        void store(const otf2::definition::source_code_location& data)
        {
            check(OTF2_GlobalDefWriter_WriteSourceCodeLocation(wrt, data.ref(), data.file().ref(),
                                                               data.line_number()),
                  "Couldn't write to global definitions writer");
        }

        void store(const otf2::definition::calling_context& data)
        {
            if (data.has_parent())
            {
                check(OTF2_GlobalDefWriter_WriteCallingContext(wrt, data.ref(), data.region().ref(),
                                                               data.source_code_location().ref(),
                                                               data.parent().ref()),
                      "Couldn't write to global definitions writer");
            }
            else
            {
                check(OTF2_GlobalDefWriter_WriteCallingContext(
                          wrt, data.ref(), data.region().ref(), data.source_code_location().ref(),
                          otf2::reference<otf2::definition::calling_context>::undefined()),
                      "Couldn't write to global definitions writer");
            }
        }

        void store(const otf2::definition::calling_context_property& data)
        {
            check(OTF2_GlobalDefWriter_WriteCallingContextProperty(
                      wrt, data.def().ref(), data.name().ref(), static_cast<OTF2_Type>(data.type()),
                      static_cast<OTF2_AttributeValue>(data.value())),
                  "Couldn't write to global definitions writer");
        }

        void store(const otf2::definition::interrupt_generator& data)
        {
            check(OTF2_GlobalDefWriter_WriteInterruptGenerator(
                      wrt, data.ref(), data.name().ref(),
                      static_cast<OTF2_InterruptGeneratorMode>(data.interrupt_generator_mode()),
                      static_cast<OTF2_Base>(data.period_base()), data.period_exponent(),
                      data.period()),
                  "Couldn't write to global definitions writer");
        }

        void store(const otf2::definition::marker& data)
        {
            check(OTF2_MarkerWriter_WriteDefMarker(
                      marker_wrt_, data.ref(), data.group().c_str(), data.category().c_str(),
                      static_cast<OTF2_MarkerSeverity>(data.severity())),
                  "Couldn't write to marker writer");
        }

        template <typename Definiton>
        void store(otf2::definition::container<Definiton>& c)
        {
            for (const auto& def : c)
                store(def);
        }

        template <typename... Definitions>
        void store(otf2::definition::container<Definitions>&... cs)
        {
            auto iters = std::make_tuple(cs.begin()...);

            using common_ref_type = typename otf2::traits::reference_type_var<Definitions...>::type;
            constexpr auto max_ref = std::numeric_limits<common_ref_type>::max();

            while (otf2::traits::reduce_tuple([](auto it) { return static_cast<bool>(it); },
                                              [](bool a, bool b) { return a || b; }, iters))
            {
                otf2::traits::apply_to_tuple_min(
                    [max_ref](auto it) {
                        return static_cast<bool>(it) ? it->ref().get() : max_ref;
                    },
                    [this](auto it) {
                        store(*it);
                        ++it;
                    },
                    iters);
            }
        }

    public:
        void write(otf2::definition::attribute data)
        {
            attributes_.add_definition(std::move(data));
        }

        // TODO: implement this
        //         void write(const otf2::definition::callpath& data)
        //         {
        //             callpaths_..add_definition(std::move(data));
        //         }
        //
        //         void write(const otf2::definition::callsite& data)
        //         {
        //             callsites_.add_definition(std::move(data));
        //         }

        void write(otf2::definition::comm data)
        {
            comms_.add_definition(std::move(data));
        }

        void write(otf2::definition::location data)
        {
            locations_.add_definition(std::move(data));
        }

        void write(otf2::definition::location_group data)
        {
            location_groups_.add_definition(std::move(data));
        }

        void write(otf2::definition::comm_group data)
        {
            comm_groups_.add_definition(std::move(data));
        }

        void write(otf2::definition::comm_locations_group data)
        {
            comm_locations_groups_.add_definition(std::move(data));
        }

        void write(otf2::definition::comm_self_group data)
        {
            comm_self_groups_.add_definition(std::move(data));
        }

        void write(otf2::definition::regions_group data)
        {
            regions_groups_.add_definition(std::move(data));
        }

        void write(otf2::definition::locations_group data)
        {
            locations_groups_.add_definition(std::move(data));
        }

        //         void write(otf2::definition::metric_group data)
        //         {
        //             metric_groups_.add_definition(std::move(data));
        //         }

        void write(otf2::definition::metric_class data)
        {
            metric_classes_.add_definition(std::move(data));
        }

        //         void write(const otf2::definition::metric_class_definition&
        // data)
        //         {
        //             metric_definitions_.add_definition(std::move(data));
        //         }

        void write(otf2::definition::metric_instance data)
        {
            metric_instances_.add_definition(std::move(data));
        }

        void write(otf2::definition::metric_member data)
        {
            metric_members_.add_definition(std::move(data));
        }

        void write(otf2::definition::parameter data)
        {
            parameters_.add_definition(std::move(data));
        }

        void write(otf2::definition::region data)
        {
            regions_.add_definition(std::move(data));
        }

        //         void write(const otf2::definition::rma_win& data)
        //         {
        //             rma_wins.add_definition(std::move(data));
        //         }

        void write(otf2::definition::string data)
        {
            strings_.add_definition(std::move(data));
        }

        void write(otf2::definition::system_tree_node data)
        {
            system_tree_nodes_.add_definition(std::move(data));
        }

        void write(otf2::definition::clock_properties data)
        {
            clock_properties_ = data;
        }

        // TODO: implement this
        //         void write(const otf2::definition::system_tree_node_domain&
        // data)
        //         {
        //             system_tree_node_domains.add_definition(std::move(data));
        //         }

        void write(otf2::definition::system_tree_node_property data)
        {
            system_tree_node_properties_.add_definition(std::move(data));
        }

        void write(otf2::definition::location_property data)
        {
            location_properties_.add_definition(std::move(data));
        }

        void write(otf2::definition::location_group_property data)
        {
            location_group_properties_.add_definition(std::move(data));
        }

        void write(otf2::definition::source_code_location data)
        {
            source_code_locations_.add_definition(std::move(data));
        }

        void write(otf2::definition::calling_context data)
        {
            calling_contexts_.add_definition(std::move(data));
        }

        void write(otf2::definition::calling_context_property data)
        {
            calling_context_properties_.add_definition(std::move(data));
        }

        void write(otf2::definition::interrupt_generator data)
        {
            interrupt_generators_.add_definition(std::move(data));
        }

        void write(otf2::definition::marker data)
        {
            markers_.add_definition(std::move(data));
        }

    public:
        void write(otf2::event::marker evt)
        {
            static otf2::chrono::convert cvrt(otf2::chrono::clock::period::den);
            static_assert(otf2::chrono::clock::period::num == 1,
                          "Don't mess around with the chrono stuff!");

            check(OTF2_MarkerWriter_WriteMarker(marker_wrt_, cvrt(evt.timestamp()).count(),
                                                evt.duration().count(), evt.def_marker().ref(),
                                                static_cast<OTF2_MarkerScope>(evt.scope()),
                                                evt.scope_ref(), evt.text().c_str()),
                  "Couldn't write marker event to marker writer.");
        }

    public:
        ~global()
        {
            // call real writes in correct order
            store(clock_properties_);
            store(strings_);
            store(attributes_);
            store(system_tree_nodes_);
            store(system_tree_node_properties_);
            store(location_groups_);
            store(location_group_properties_);
            store(locations_);
            store(location_properties_);
            store(regions_);

            store(comm_locations_groups_, comm_self_groups_, comm_groups_, locations_groups_,
                  regions_groups_);
            //             store(metric_groups);

            store(comms_);

            store(parameters_);
            //             store(callpaths_);
            //             store(callsites_);

            store(source_code_locations_);
            store(calling_contexts_);
            store(calling_context_properties_);
            store(interrupt_generators_);

            store(metric_members_);
            store(metric_classes_, metric_instances_);

            store(markers_);
        }

    private:
        OTF2_GlobalDefWriter* wrt;
        OTF2_MarkerWriter* marker_wrt_;

        otf2::definition::container<otf2::definition::attribute> attributes_;
        otf2::definition::container<otf2::definition::comm> comms_;
        otf2::definition::container<otf2::definition::location> locations_;
        otf2::definition::container<otf2::definition::location_group> location_groups_;
        otf2::definition::container<otf2::definition::parameter> parameters_;
        otf2::definition::container<otf2::definition::region> regions_;
        otf2::definition::container<otf2::definition::string> strings_;
        otf2::definition::container<otf2::definition::system_tree_node> system_tree_nodes_;

        otf2::definition::container<otf2::definition::source_code_location> source_code_locations_;
        otf2::definition::container<otf2::definition::calling_context> calling_contexts_;
        otf2::definition::container<otf2::definition::interrupt_generator> interrupt_generators_;

        otf2::definition::container<otf2::definition::metric_class> metric_classes_;
        otf2::definition::container<otf2::definition::metric_instance> metric_instances_;
        otf2::definition::container<otf2::definition::metric_member> metric_members_;

        otf2::definition::container<otf2::definition::locations_group> locations_groups_;
        otf2::definition::container<otf2::definition::regions_group> regions_groups_;
        // otf2::definition::container<otf2::definition::metric_group>
        // metric_groups_;
        otf2::definition::container<otf2::definition::comm_locations_group> comm_locations_groups_;
        otf2::definition::container<otf2::definition::comm_group> comm_groups_;
        otf2::definition::container<otf2::definition::comm_self_group> comm_self_groups_;

        otf2::definition::container<otf2::definition::location_property> location_properties_;
        otf2::definition::container<otf2::definition::location_group_property>
            location_group_properties_;
        otf2::definition::container<otf2::definition::system_tree_node_property>
            system_tree_node_properties_;

        otf2::definition::container<otf2::definition::calling_context_property>
            calling_context_properties_;

        otf2::definition::container<otf2::definition::marker> markers_;

        otf2::definition::clock_properties clock_properties_;
    };

    template <typename Definition>
    global& operator<<(global& wrt, Definition def)
    {
        wrt.write(def);

        return wrt;
    }
}
} // namespace otf2::writer

#endif // INCLUDE_OTF2XX_WRITER_GLOBAL_HPP
