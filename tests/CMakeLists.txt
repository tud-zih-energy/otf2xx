# This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
# otf2xx - A wrapper for the Open Trace Format 2 library
#
# Copyright (c) 2016, Technische Universität Dresden, Germany
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Simplify adding a test
# Pass the name of a cpp file and the name of the library to link.
# Optionally some additional args for add_test
function(otf2xx_add_test name library)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE ${library})
    add_test(NAME ${name} COMMAND ${name} ${ARGV2})
endfunction()

# Add a compile-only test. The compilation must succeed
# Pass the name of a cpp file and the name of the library to link.
function(otf2xx_add_compile_test name library)
    set(lib_name otf2xx-compiletest-${name})
    add_library(${lib_name} STATIC ${name}.cpp)
    target_link_libraries(${lib_name} PRIVATE ${library})
endfunction()

# Add a test that will fail compilation
# Pass the name of a cpp file in the folder 'expected_failures'
# This will create 2 targets: One with and one w/o -DOTF2XX_MAKE_FAIL
# For the latter a compile test is added which expects a failure
function(add_failing_test name)
    # Positive test to avoid unintended failures
    add_executable(${name}-pass expected_failures/${name}.cpp)
    target_link_libraries(${name}-pass PRIVATE otf2xx::Core)
    # Provoke failure and compile only during test
    add_executable(${name} EXCLUDE_FROM_ALL expected_failures/${name}.cpp)
    target_link_libraries(${name} PRIVATE otf2xx::Core)
    target_compile_definitions(${name} PRIVATE -DOTF2XX_MAKE_FAIL)
    add_test(NAME metric_event_with_instance
        COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG> --target metric_event_with_instance
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(${name} PROPERTIES WILL_FAIL TRUE)
endfunction()

# Always add compile tests
otf2xx_add_compile_test(enums_test otf2xx::Core)
otf2xx_add_compile_test(traits_test otf2xx::Core)
# Skip other tests when not enabled
if(NOT OTF2XX_ENABLE_ALL_TESTS)
    return()
endif()

otf2xx_add_test(tmp_test otf2xx::Core)

otf2xx_add_test(intrusive_ptr_test otf2xx::Core)
otf2xx_add_test(ref_gen_test otf2xx::Core)
otf2xx_add_test(registry_test otf2xx::Core)
otf2xx_add_test(lookup_registry_test otf2xx::Core)
otf2xx_add_test(metric_events otf2xx::Core)

otf2xx_add_test(writer_test otf2xx::Writer)
set_property(TEST writer_test PROPERTY FIXTURES_SETUP writer_trace)

otf2xx_add_test(writer_registry_test otf2xx::Writer)
set_property(TEST writer_registry_test PROPERTY FIXTURES_SETUP writer_registry_trace)

otf2xx_add_test(reader_test otf2xx::Reader ${CMAKE_CURRENT_BINARY_DIR}/otf2xx_writer_trace/traces.otf2)
set_property(TEST reader_test PROPERTY FIXTURES_REQUIRED writer_trace)

add_test(NAME reader_registry_test COMMAND reader_test ${CMAKE_CURRENT_BINARY_DIR}/otf2xx_writer_registry_trace/traces.otf2)
set_property(TEST reader_registry_test PROPERTY FIXTURES_REQUIRED writer_registry_trace)

add_test(NAME trace_compare_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/trace_compare.sh ${OTF2_PRINT} ${CMAKE_CURRENT_BINARY_DIR}/otf2xx_writer_registry_trace/traces.otf2 ${CMAKE_CURRENT_BINARY_DIR}/otf2xx_writer_trace/traces.otf2
)
set_property(TEST trace_compare_test PROPERTY FIXTURES_REQUIRED "writer_registry_trace;writer_trace")


# cleanup traces
add_test(NAME writer_test_cleanup COMMAND cmake -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/otf2xx_writer_trace)
set_property(TEST writer_test_cleanup PROPERTY FIXTURES_CLEANUP writer_trace)
add_test(NAME writer_test_registry_cleanup COMMAND cmake -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/otf2xx_writer_registry_trace)
set_property(TEST writer_test_registry_cleanup PROPERTY FIXTURES_CLEANUP writer_registry_trace)
