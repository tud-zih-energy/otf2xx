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

#pragma once

#include <otf2xx/definition/definitions.hpp>
#include <otf2xx/reference.hpp>
#include <otf2xx/reference_generator.hpp>

#include <type_traits>
#include <utility>

namespace otf2
{
template <typename Definition>
class DefinitionHolder
{
public:
    DefinitionHolder(otf2::trace_reference_generator& refs) : refs_(refs)
    {
    }

public:
    const Definition& operator[](otf2::reference<Definition> ref) const
    {
        return definitions_[ref];
    }

    void operator()(const Definition& def)
    {
        auto res = definitions_.add_definition(def);
        refs_.register_definition(res);
    }

    void operator()(otf2::definition::detail::weak_ref<Definition> ref)
    {
        auto res = definitions_.add_definition(ref.lock());
        refs_.register_definition(res);
    }

    template <typename... Args>
    const Definition& create(Args&&... args)
    {
        return definitions_.emplace(refs_.next<Definition>(), std::forward<Args>(args)...);
    }

    template <typename RefType, typename... Args>
    std::enable_if_t<std::is_convertible<RefType, otf2::reference<Definition>>::value,
                     const Definition&>
    create(RefType ref, Args&&... args)
    {
        // TODO I fucking bet that some day there will be a definition, where this is well-formed in
        // the case you wanted to omit the ref FeelsBadMan
        const auto& def = definitions_.emplace(ref, std::forward<Args>(args)...);
        refs_.register_definition(def);
        return def;
    }

    const otf2::definition::container<Definition>& data() const
    {
        return definitions_;
    }

private:
    otf2::definition::container<Definition> definitions_;
    otf2::trace_reference_generator& refs_;
};

template <typename Property>
class PropertyHolder
{
public:
    void operator()(const Property& def)
    {
        properties_.emplace(def);
    }

    template <typename... Args>
    const Property& create(Args&&... args)
    {
        return properties_.emplace(std::forward<Args>(args)...);
    }

    const otf2::definition::container<Property>& data() const
    {
        return properties_;
    }

private:
    otf2::definition::container<Property> properties_;
};

class Registry : public trace_reference_generator
{
    template <typename Definition>
    using map_type = otf2::DefinitionHolder<Definition>;

public:
    Registry()
    : attributes_(*this), comms_(*this), locations_(*this), location_groups_(*this),
      parameters_(*this), regions_(*this), strings_(*this), system_tree_nodes_(*this),
      source_code_locations_(*this), calling_contexts_(*this), interrupt_generators_(*this),
      io_handles_(*this), io_files_(*this), io_regular_files_(*this), io_directories_(*this),
      io_paradigms_(*this), io_pre_created_handle_states_(*this), locations_groups_(*this),
      regions_groups_(*this),
      // metric_groups_(*this),
      comm_locations_groups_(*this), comm_groups_(*this), comm_self_groups_(*this),
      metric_members_(*this), metric_classes_(*this), metric_instances_(*this)
    {
    }

public:
    DefinitionHolder<otf2::definition::attribute>& attributes()
    {
        return attributes_;
    }

    DefinitionHolder<otf2::definition::comm>& comms()
    {
        return comms_;
    }

    DefinitionHolder<otf2::definition::location>& locations()
    {
        return locations_;
    }

    DefinitionHolder<otf2::definition::location_group>& location_groups()
    {
        return location_groups_;
    }

    DefinitionHolder<otf2::definition::parameter>& parameters()
    {
        return parameters_;
    }

    DefinitionHolder<otf2::definition::region>& regions()
    {
        return regions_;
    }

    DefinitionHolder<otf2::definition::string>& strings()
    {
        return strings_;
    }

    DefinitionHolder<otf2::definition::system_tree_node>& system_tree_nodes()
    {
        return system_tree_nodes_;
    }

    DefinitionHolder<otf2::definition::source_code_location>& source_code_locations()
    {
        return source_code_locations_;
    }

    DefinitionHolder<otf2::definition::calling_context>& calling_contexts()
    {
        return calling_contexts_;
    }

    DefinitionHolder<otf2::definition::interrupt_generator>& interrupt_generators()
    {
        return interrupt_generators_;
    }

    DefinitionHolder<otf2::definition::io_file>& io_files()
    {
        return io_files_;
    }

    DefinitionHolder<otf2::definition::io_regular_file>& io_regular_files()
    {
        return io_regular_files_;
    }

    DefinitionHolder<otf2::definition::io_directory>& io_directories()
    {
        return io_directories_;
    }

    DefinitionHolder<otf2::definition::io_handle>& io_handles()
    {
        return io_handles_;
    }

    DefinitionHolder<otf2::definition::io_paradigm>& io_paradigms()
    {
        return io_paradigms_;
    }

    DefinitionHolder<otf2::definition::io_pre_created_handle_state>& io_pre_created_handle_states()
    {
        return io_pre_created_handle_states_;
    }

    DefinitionHolder<otf2::definition::locations_group>& locations_groups()
    {
        return locations_groups_;
    }

    DefinitionHolder<otf2::definition::regions_group>& regions_groups()
    {
        return regions_groups_;
    }

    // DefinitionHolder<otf2::definition::metric_group>& metric_groups_;
    DefinitionHolder<otf2::definition::comm_locations_group>& comm_locations_groups()
    {
        return comm_locations_groups_;
    }

    DefinitionHolder<otf2::definition::comm_group>& comm_groups()
    {
        return comm_groups_;
    }

    DefinitionHolder<otf2::definition::comm_self_group>& comm_self_groups()
    {
        return comm_self_groups_;
    }

    DefinitionHolder<otf2::definition::metric_member>& metric_members()
    {
        return metric_members_;
    }

    DefinitionHolder<otf2::definition::metric_class>& metric_classes()
    {
        return metric_classes_;
    }

    DefinitionHolder<otf2::definition::metric_instance>& metric_instances()
    {
        return metric_instances_;
    }

public:
    PropertyHolder<otf2::definition::location_property>& location_properties()
    {
        return location_properties_;
    }

    PropertyHolder<otf2::definition::location_group_property>& location_group_properties()
    {
        return location_group_properties_;
    }

    PropertyHolder<otf2::definition::system_tree_node_property>& system_tree_node_properties()
    {
        return system_tree_node_properties_;
    }

    PropertyHolder<otf2::definition::calling_context_property>& calling_context_properties()
    {
        return calling_context_properties_;
    }

    PropertyHolder<otf2::definition::io_file_property>& io_file_properties()
    {
        return io_file_properties_;
    }

private:
    DefinitionHolder<otf2::definition::attribute> attributes_;
    DefinitionHolder<otf2::definition::comm> comms_;
    DefinitionHolder<otf2::definition::location> locations_;
    DefinitionHolder<otf2::definition::location_group> location_groups_;
    DefinitionHolder<otf2::definition::parameter> parameters_;
    DefinitionHolder<otf2::definition::region> regions_;
    DefinitionHolder<otf2::definition::string> strings_;
    DefinitionHolder<otf2::definition::system_tree_node> system_tree_nodes_;

    DefinitionHolder<otf2::definition::source_code_location> source_code_locations_;
    DefinitionHolder<otf2::definition::calling_context> calling_contexts_;
    DefinitionHolder<otf2::definition::interrupt_generator> interrupt_generators_;
    DefinitionHolder<otf2::definition::io_handle> io_handles_;
    DefinitionHolder<otf2::definition::io_file> io_files_;
    DefinitionHolder<otf2::definition::io_regular_file> io_regular_files_;
    DefinitionHolder<otf2::definition::io_directory> io_directories_;
    DefinitionHolder<otf2::definition::io_paradigm> io_paradigms_;
    DefinitionHolder<otf2::definition::io_pre_created_handle_state> io_pre_created_handle_states_;

    DefinitionHolder<otf2::definition::locations_group> locations_groups_;
    DefinitionHolder<otf2::definition::regions_group> regions_groups_;
    // DefinitionHolder<otf2::definition::metric_group> metric_groups_;
    DefinitionHolder<otf2::definition::comm_locations_group> comm_locations_groups_;
    DefinitionHolder<otf2::definition::comm_group> comm_groups_;
    DefinitionHolder<otf2::definition::comm_self_group> comm_self_groups_;

    DefinitionHolder<otf2::definition::metric_member> metric_members_;
    DefinitionHolder<otf2::definition::metric_class> metric_classes_;
    DefinitionHolder<otf2::definition::metric_instance> metric_instances_;

    PropertyHolder<otf2::definition::location_property> location_properties_;
    PropertyHolder<otf2::definition::location_group_property> location_group_properties_;
    PropertyHolder<otf2::definition::system_tree_node_property> system_tree_node_properties_;
    PropertyHolder<otf2::definition::calling_context_property> calling_context_properties_;
    PropertyHolder<otf2::definition::io_file_property> io_file_properties_;
};
} // namespace otf2
