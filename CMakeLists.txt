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

cmake_minimum_required(VERSION 3.10)

project(otf2xx VERSION 2.0.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
find_package(OTF2 3.1 EXACT)

if (NOT OTF2_FOUND)
    message(FATAL_ERROR "Please download and install OTF2 3.1.\n"
    "Available from: https://perftools.pages.jsc.fz-juelich.de/cicd/otf2/tags/otf2-3.1/otf2-3.1.tar.gz")
endif()

set(OTF2XX_CHRONO_DURATION_TYPE "picoseconds" CACHE STRING "The used underlying type for otf2::chrono::duration.")
set(otf2xx_allowed_duration_types "nanoseconds" "picoseconds")
set_property(CACHE OTF2XX_CHRONO_DURATION_TYPE PROPERTY STRINGS ${otf2xx_allowed_duration_types})
if(OTF2XX_CHRONO_DURATION_TYPE IN_LIST otf2xx_allowed_duration_types)
    message(STATUS "OTF2xx uses '${OTF2XX_CHRONO_DURATION_TYPE}' as chrono duration type.")
else()
    message(SEND_ERROR "OTF2XX_CHRONO_DURATION_TYPE must be one of ${otf2xx_allowed_duration_types}")
endif()

option(OTF2XX_WITH_MPI "Whether OTF2xx should be build with MPI support or not. (Requires Boost.MPI)" OFF)
option(OTF2XX_USE_STATIC_LIBS "Whether OTF2xx should be build static or not." ON)
# Set CMake variable which determines default library type
if(OTF2XX_USE_STATIC_LIBS)
    set(BUILD_SHARED_LIBS OFF)
else()
    set(BUILD_SHARED_LIBS ON)
endif()

add_library(otf2xx-core INTERFACE)
target_include_directories(otf2xx-core
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(otf2xx-core
    INTERFACE
        otf2::otf2
)
target_compile_features(otf2xx-core INTERFACE cxx_std_17)
target_compile_definitions(otf2xx-core
    INTERFACE
        OTF2XX_CHRONO_DURATION_TYPE=${OTF2XX_CHRONO_DURATION_TYPE}
)

if(OTF2XX_WITH_MPI)
    message(STATUS "Building OTF2xx with MPI support.")
    target_compile_definitions(otf2xx-core
        INTERFACE
            OTF2XX_HAS_MPI
    )
    find_package(MPI REQUIRED COMPONENTS CXX)
    if(NOT TARGET MPI::MPI_CXX)
        message(FATAL_ERROR "MPI support requires CMake >= 3.9 or a properly defined MPI::MPI_CXX target")
    endif()
    target_link_libraries(otf2xx-core INTERFACE MPI::MPI_CXX)
endif()

add_library(otf2xx-reader src/reader/callback/definitions.cpp src/reader/callback/events.cpp)
target_link_libraries(otf2xx-reader
    PUBLIC
        otf2xx::Core)

add_library(otf2xx-writer INTERFACE)
target_link_libraries(otf2xx-writer
    INTERFACE
        otf2xx::Core
)

add_library(otf2xx-all INTERFACE)
target_link_libraries(otf2xx-all
    INTERFACE
        otf2xx::Core
        otf2xx::Reader
        otf2xx::Writer
)

add_library(otf2xx::otf2xx ALIAS otf2xx-all)
add_library(otf2xx::Core ALIAS otf2xx-core)
add_library(otf2xx::Reader ALIAS otf2xx-reader)
add_library(otf2xx::Writer ALIAS otf2xx-writer)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    target_compile_options(otf2xx-core
        INTERFACE
            -Wall -Wextra -pedantic
    )

    install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/include/otf2xx
        DESTINATION include
    )
    install(TARGETS otf2xx-core otf2xx-all otf2xx-reader otf2xx-writer EXPORT otf2xxTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )
    install(EXPORT otf2xxTargets
        FILE otf2xxTargets.cmake
        NAMESPACE otf2xx::
        DESTINATION lib/cmake/otf2xx
    )

    include(CMakePackageConfigHelpers)
    write_basic_package_version_File("otf2xxConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    install(FILES "otf2xxConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/otf2xxConfigVersion.cmake"
        DESTINATION lib/cmake/otf2xx
    )

    include(CTest)
    set(OTF2XX_ENABLE_ALL_TESTS ON)
else()
    set_target_properties(otf2xx-reader PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

add_subdirectory(tests)
