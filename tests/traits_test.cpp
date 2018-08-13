/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2018, Technische Universitaet Dresden, Germany
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

#include <otf2xx/definition/definitions.hpp>
#include <otf2xx/reference.hpp>

/**
 * @brief: Checks the OTF2 and OTF2xx types to be equal This test is VERY!
 * important. Compile time test only.
 *
 * \warning { If this test fails, expect problems with all traces.
 * Fix the problem first. Seriously! }
 *
 */

// clang-format off

static_assert(std::is_same<otf2::reference<otf2::definition::attribute>::ref_type,
                     OTF2_AttributeRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::comm>::ref_type,
                     OTF2_CommRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::location>::ref_type,
                     OTF2_LocationRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::location_group>::ref_type,
                     OTF2_LocationGroupRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::parameter>::ref_type,
                     OTF2_ParameterRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::region>::ref_type,
                     OTF2_RegionRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::string>::ref_type,
                     OTF2_StringRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::system_tree_node>::ref_type,
                     OTF2_SystemTreeNodeRef>::value, "type mismatch");

static_assert(std::is_same<otf2::reference<otf2::definition::source_code_location>::ref_type,
                     OTF2_SourceCodeLocationRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::calling_context>::ref_type,
                     OTF2_CallingContextRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::interrupt_generator>::ref_type,
                     OTF2_InterruptGeneratorRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::io_handle>::ref_type,
                     OTF2_IoHandleRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::io_regular_file>::ref_type,
                     OTF2_IoFileRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::io_directory>::ref_type,
                     OTF2_IoFileRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::io_paradigm>::ref_type,
                     OTF2_IoParadigmRef>::value, "type mismatch");

static_assert(std::is_same<otf2::reference<otf2::definition::locations_group>::ref_type,
                     OTF2_GroupRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::regions_group>::ref_type,
                     OTF2_GroupRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::comm_locations_group>::ref_type,
                     OTF2_GroupRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::comm_group>::ref_type,
                     OTF2_GroupRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::comm_self_group>::ref_type,
                     OTF2_GroupRef>::value, "type mismatch");

static_assert(std::is_same<otf2::reference<otf2::definition::metric_member>::ref_type,
                     OTF2_MetricMemberRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::metric_class>::ref_type,
                     OTF2_MetricRef>::value, "type mismatch");
static_assert(std::is_same<otf2::reference<otf2::definition::metric_instance>::ref_type,
                     OTF2_MetricRef>::value, "type mismatch");

static_assert(std::is_same<otf2::reference<otf2::definition::marker>::ref_type,
                     OTF2_MarkerRef>::value, "type mismatch");

int shut_up_the_warning_this_has_no_global_symbol_defined() {
    return 42;
}
