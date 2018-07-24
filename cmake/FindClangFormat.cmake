find_program(CLANG_FORMAT_EXECUTABLE
    NAMES clang-format clang-format-5.0
          clang-format-4.0 clang-format-3.9
          clang-format-3.8 clang-format-3.7
          clang-format-3.6 clang-format-3.5
          clang-format-3.4 clang-format-3.3
)
mark_as_advanced(CLANG_FORMAT_EXECUTABLE)

if(CLANG_FORMAT_EXECUTABLE)
    execute_process(COMMAND ${CLANG_FORMAT_EXECUTABLE} -version
        OUTPUT_VARIABLE clang_format_version_raw
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(clang_format_version_raw MATCHES "^clang-format version ([.0-9]+)")
        set(CLANG_FORMAT_VERSION ${CMAKE_MATCH_1})
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ClangFormat
    REQUIRED_VARS CLANG_FORMAT_EXECUTABLE
    VERSION_VAR CLANG_FORMAT_VERSION)
