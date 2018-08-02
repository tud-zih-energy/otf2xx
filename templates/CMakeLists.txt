# Find programs required for template processing
get_filename_component(otf2_CONFIG_path "${OTF2_CONFIG_EXE}" DIRECTORY)
find_program(OTF2_TEMPLATE otf2-template
    HINTS "${otf2_CONFIG_path}")
find_package(ClangFormat 5.0.0)

foreach(dep OTF2_TEMPLATE CLANG_FORMAT_EXECUTABLE)
    if(NOT EXISTS ${${dep}})
        message(STATUS "Could not find ${dep}. Target for template generation disabled")
        return()
    endif()
endforeach()

set(input_dir ${CMAKE_CURRENT_SOURCE_DIR})
# Write files directly into the source directory with the same structure as found in this folder
# Note: When changing this, make sure to also copy the clang-format style file to a parent dir
set(target_dir ${PROJECT_SOURCE_DIR})
file(GLOB_RECURSE templates RELATIVE ${input_dir} ${input_dir}/*.tmpl)
set(generated_files "")
foreach(template IN LISTS templates)
    set(input_file ${input_dir}/${template})
    string(REGEX REPLACE "\\.tmpl$" "" new_file  ${target_dir}/${template})
    get_filename_component(target_folder "${new_file}" DIRECTORY)
    # The generator cannot handle non-existing directories
    if(NOT EXISTS ${target_folder})
        file(MAKE_DIRECTORY ${target_folder})
    endif()
    add_custom_command(
        OUTPUT  ${new_file}
        COMMAND ${OTF2_TEMPLATE} ${input_file} ${new_file}
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i -style=file ${new_file}
        DEPENDS ${input_file}
    )
    list(APPEND generated_files ${new_file})
endforeach()
# Add a target on which others can depend and which can be "make"d
add_custom_target(GenerateTemplates DEPENDS ${generated_files})

message(STATUS "Template generation available. "
    "Run 'make GenerateTemplates' (or similar) to update source headers by template generated ones.")
