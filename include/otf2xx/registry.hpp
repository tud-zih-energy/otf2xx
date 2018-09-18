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
class Registry;

template <typename Definition>
class DefinitionHolder
{
    static_assert(otf2::traits::is_referable_definition<Definition>::value, "Whoopsy.");

public:
    DefinitionHolder(otf2::trace_reference_generator& refs) : refs_(refs)
    {
    }

public:
    const Definition& operator[](typename Definition::reference_type ref) const
    {
        return definitions_[ref];
    }

    void operator()(const Definition& def)
    {
        definitions_.add_definition(def);
        refs_.register_definition(def);
    }

    void operator()(otf2::definition::detail::weak_ref<Definition> ref)
    {
        auto def = ref.lock();
        refs_.register_definition(def);
        definitions_.add_definition(std::move(def));
    }

    template <typename... Args>
    const Definition& create(Args&&... args)
    {
        return definitions_.emplace(refs_.next<Definition>(), std::forward<Args>(args)...);
    }

    template <typename RefType, typename... Args>
    std::enable_if_t<std::is_convertible<RefType, typename Definition::reference_type>::value,
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

    auto begin() const
    {
        return definitions_.begin();
    }

    auto end() const
    {
        return definitions_.end();
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

    auto begin() const
    {
        return properties_.begin();
    }

    auto end() const
    {
        return properties_.end();
    }

private:
    otf2::definition::container<Property> properties_;
};

class Registry
{
    template <typename Definition>
    using map_type = otf2::DefinitionHolder<Definition>;

public:
    Registry()
    : attributes_(refs_), comms_(refs_), locations_(refs_), location_groups_(refs_),
      parameters_(refs_), regions_(refs_), strings_(refs_), system_tree_nodes_(refs_),
      source_code_locations_(refs_), calling_contexts_(refs_), interrupt_generators_(refs_),
      io_handles_(refs_), io_regular_files_(refs_), io_directories_(refs_), io_paradigms_(refs_),
      locations_groups_(refs_), regions_groups_(refs_),
      // metric_groups_(refs_),
      comm_locations_groups_(refs_), comm_groups_(refs_), comm_self_groups_(refs_),
      metric_members_(refs_), metric_classes_(refs_), metric_instances_(refs_)
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

    PropertyHolder<otf2::definition::io_pre_created_handle_state>& io_pre_created_handle_states()
    {
        return io_pre_created_handle_states_;
    }

public:
    const DefinitionHolder<otf2::definition::attribute>& attributes() const
    {
        return attributes_;
    }

    const DefinitionHolder<otf2::definition::comm>& comms() const
    {
        return comms_;
    }

    const DefinitionHolder<otf2::definition::location>& locations() const
    {
        return locations_;
    }

    const DefinitionHolder<otf2::definition::location_group>& location_groups() const
    {
        return location_groups_;
    }

    const DefinitionHolder<otf2::definition::parameter>& parameters() const
    {
        return parameters_;
    }

    const DefinitionHolder<otf2::definition::region>& regions() const
    {
        return regions_;
    }

    const DefinitionHolder<otf2::definition::string>& strings() const
    {
        return strings_;
    }

    const DefinitionHolder<otf2::definition::system_tree_node>& system_tree_nodes() const
    {
        return system_tree_nodes_;
    }

    const DefinitionHolder<otf2::definition::source_code_location>& source_code_locations() const
    {
        return source_code_locations_;
    }

    const DefinitionHolder<otf2::definition::calling_context>& calling_contexts() const
    {
        return calling_contexts_;
    }

    const DefinitionHolder<otf2::definition::interrupt_generator>& interrupt_generators() const
    {
        return interrupt_generators_;
    }

    const DefinitionHolder<otf2::definition::io_regular_file>& io_regular_files() const
    {
        return io_regular_files_;
    }

    const DefinitionHolder<otf2::definition::io_directory>& io_directories() const
    {
        return io_directories_;
    }

    const DefinitionHolder<otf2::definition::io_handle>& io_handles() const
    {
        return io_handles_;
    }

    const DefinitionHolder<otf2::definition::io_paradigm>& io_paradigms() const
    {
        return io_paradigms_;
    }

    const DefinitionHolder<otf2::definition::locations_group>& locations_groups() const
    {
        return locations_groups_;
    }

    const DefinitionHolder<otf2::definition::regions_group>& regions_groups() const
    {
        return regions_groups_;
    }

    // DefinitionHolder<otf2::definition::metric_group>& metric_groups_;
    const DefinitionHolder<otf2::definition::comm_locations_group>& comm_locations_groups() const
    {
        return comm_locations_groups_;
    }

    const DefinitionHolder<otf2::definition::comm_group>& comm_groups() const
    {
        return comm_groups_;
    }

    const DefinitionHolder<otf2::definition::comm_self_group>& comm_self_groups() const
    {
        return comm_self_groups_;
    }

    const DefinitionHolder<otf2::definition::metric_member>& metric_members() const
    {
        return metric_members_;
    }

    const DefinitionHolder<otf2::definition::metric_class>& metric_classes() const
    {
        return metric_classes_;
    }

    const DefinitionHolder<otf2::definition::metric_instance>& metric_instances() const
    {
        return metric_instances_;
    }

public:
    const PropertyHolder<otf2::definition::location_property>& location_properties() const
    {
        return location_properties_;
    }

    const PropertyHolder<otf2::definition::location_group_property>&
    location_group_properties() const
    {
        return location_group_properties_;
    }

    const PropertyHolder<otf2::definition::system_tree_node_property>&
    system_tree_node_properties() const
    {
        return system_tree_node_properties_;
    }

    const PropertyHolder<otf2::definition::calling_context_property>&
    calling_context_properties() const
    {
        return calling_context_properties_;
    }

    const PropertyHolder<otf2::definition::io_file_property>& io_file_properties() const
    {
        return io_file_properties_;
    }

    const PropertyHolder<otf2::definition::io_pre_created_handle_state>&
    io_pre_created_handle_states() const
    {
        return io_pre_created_handle_states_;
    }

public:
    void register_definition(const otf2::definition::attribute& def)
    {
        attributes_(def);
    }

    void register_definition(const otf2::definition::comm& def)
    {
        comms_(def);
    }

    void register_definition(const otf2::definition::location& def)
    {
        locations_(def);
    }

    void register_definition(const otf2::definition::location_group& def)
    {
        location_groups_(def);
    }

    void register_definition(const otf2::definition::parameter& def)
    {
        parameters_(def);
    }

    void register_definition(const otf2::definition::region& def)
    {
        regions_(def);
    }

    void register_definition(const otf2::definition::string& def)
    {
        strings_(def);
    }

    void register_definition(const otf2::definition::system_tree_node& def)
    {
        system_tree_nodes_(def);
    }

    void register_definition(const otf2::definition::source_code_location& def)
    {
        source_code_locations_(def);
    }

    void register_definition(const otf2::definition::calling_context& def)
    {
        calling_contexts_(def);
    }

    void register_definition(const otf2::definition::interrupt_generator& def)
    {
        interrupt_generators_(def);
    }

    void register_definition(const otf2::definition::io_handle& def)
    {
        io_handles_(def);
    }

    void register_definition(const otf2::definition::io_regular_file& def)
    {
        io_regular_files_(def);
    }

    void register_definition(const otf2::definition::io_directory& def)
    {
        io_directories_(def);
    }

    void register_definition(const otf2::definition::io_paradigm& def)
    {
        io_paradigms_(def);
    }

    void register_definition(const otf2::definition::io_pre_created_handle_state& def)
    {
        io_pre_created_handle_states_(def);
    }

    void register_definition(const otf2::definition::locations_group& def)
    {
        locations_groups_(def);
    }

    void register_definition(const otf2::definition::regions_group& def)
    {
        regions_groups_(def);
    }

    void register_definition(const otf2::definition::comm_locations_group& def)
    {
        comm_locations_groups_(def);
    }

    void register_definition(const otf2::definition::comm_group& def)
    {
        comm_groups_(def);
    }

    void register_definition(const otf2::definition::comm_self_group& def)
    {
        comm_self_groups_(def);
    }

    void register_definition(const otf2::definition::metric_member& def)
    {
        metric_members_(def);
    }

    void register_definition(const otf2::definition::metric_class& def)
    {
        metric_classes_(def);
    }

    void register_definition(const otf2::definition::metric_instance& def)
    {
        metric_instances_(def);
    }

private:
    trace_reference_generator refs_;

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
    DefinitionHolder<otf2::definition::io_regular_file> io_regular_files_;
    DefinitionHolder<otf2::definition::io_directory> io_directories_;
    DefinitionHolder<otf2::definition::io_paradigm> io_paradigms_;

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
    PropertyHolder<otf2::definition::io_pre_created_handle_state> io_pre_created_handle_states_;
};
} // namespace otf2
