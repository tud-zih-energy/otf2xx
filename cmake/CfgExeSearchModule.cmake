include (CMakeParseArguments)

macro(cfg_exe_check_config_output what)
  if (NOT result EQUAL 0)
    if(ARG_QUIET)
      return() # Return from parent, not this macro
    else()
      message (FATAL_ERROR "Failed to get ${what} from ${${prefix}_CONFIG}")
    endif()
  endif()
endmacro()

# Find a module using a config exe similar to pkg_search_module
# cfg_exe_search_module(<PREFIX> <CFG_NAME> ...)
# Arguments:
# PREFIX               ... Prefix for variable names
# CFG_NAME             ... Name of the config exe
# HINTS, PATHS         ... Additional search path for the config exe
# VERSION_FLAG         ... Flag to query version (default: --version)
# CFLAG                ... Flag to query C/C++ flags (including include paths) (default: --cflags)
# LINK_FLAG            ... Flag to query linker flags (link paths, ...) (default: --ldflags)
# LIB_FLAG             ... Flag to query libraries (may contain other linker flags) (default: --libs)
# ADDITIONAL_FLAGS     ... Additional flags to pass to the config exe
# QUIET                ... Don't print error messages
# IMPORTED_TARGET      ... Creates an imported target <LOWER_PREFIX>::<LOWER_PREFIX> ready to link
# IMPORTED_TARGET_NAME ... Creates an imported target with the given name ready to link
# Sets the following variables:
# <PREFIX>_CONFIG        ... Path to config exe
# <PREFIX>_LIBRARIES     ... only the libraries (w/o the '-l')
# <PREFIX>_LIBRARY_DIRS  ... the paths of the libraries (w/o the '-L')
# <PREFIX>_LIB_VARS      ... List of variables with libraries (for validation)
# <PREFIX>_LDFLAGS       ... all required linker flags
# <PREFIX>_LDFLAGS_OTHER ... all other linker flags
# <PREFIX>_INCLUDE_DIRS  ... the '-I' preprocessor flags (w/o the '-I')
# <PREFIX>_CFLAGS        ... all required cflags
# <PREFIX>_CFLAGS_OTHER  ... the other compiler flags
# <PREFIX>-<libName>     ... Path to dependent library
function(cfg_exe_search_module prefix cfg_name)
  set(options IMPORTED_TARGET QUIET)
  set(one_value_options VERSION_FLAG CFLAG LINK_FLAG LIB_FLAG IMPORTED_TARGET_NAME)
  set(multi_value_options PATHS HINTS ADDITIONAL_FLAGS)
  cmake_parse_arguments(ARG "${options}" "${one_value_options}" "${multi_value_options}" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unexpected arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()
  macro(set_if_unset var default)
    if("${${var}}" STREQUAL "")
      set(${var} "${default}")
    endif()
  endmacro()
  set_if_unset(ARG_VERSION_FLAG --version)
  set_if_unset(ARG_CFLAG --cflags)
  set_if_unset(ARG_LINK_FLAG --ldflags)
  set_if_unset(ARG_LIB_FLAG --libs)
  if(ARG_IMPORTED_TARGET)
    string(TOLOWER ${prefix}::${prefix} libName)
    set_if_unset(ARG_IMPORTED_TARGET_NAME ${libName})
  endif()

  find_program(${prefix}_CONFIG NAMES "${cfg_name}"
    PATHS ${ARG_PATHS}
    HINTS ${ARG_HINTS}
    PATH_SUFFIXES bin
  )
  if(NOT ${prefix}_CONFIG)
    return()
  endif()
  set(cfg_exe "${${prefix}_CONFIG}") # Shorter
  execute_process (COMMAND "${cfg_exe}"
      ${ARG_VERSION_FLAG}
      RESULT_VARIABLE result
      OUTPUT_VARIABLE version
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  cfg_exe_check_config_output("version")
  set(${prefix}_VERSION "${version}" PARENT_SCOPE)

  if(ARG_ADDITIONAL_FLAGS)
    separate_arguments(ARG_ADDITIONAL_FLAGS)
  endif()

  if(ARG_CFLAG)
    # compile flags
    execute_process(COMMAND "${cfg_exe}"
      ${ARG_CFLAG} ${ARG_ADDITIONAL_FLAGS}
      RESULT_VARIABLE result
      OUTPUT_VARIABLE cflags
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    cfg_exe_check_config_output("c flags")
    separate_arguments(cflags)
    set(cflags_other "")
    set(include_dirs "")
    foreach(flag IN LISTS cflags)
      if(flag MATCHES "^-I(.*)")
        list(APPEND include_dirs "${CMAKE_MATCH_1}")
      else()
        list(APPEND cflags_other "${flag}")
      endif()
    endforeach()
    set(${prefix}_INCLUDE_DIRS "${include_dirs}" PARENT_SCOPE)
    set(${prefix}_CFLAGS "${cflags}" PARENT_SCOPE)
    set(${prefix}_CFLAGS_OTHER "${cflags_other}" PARENT_SCOPE)
  endif()

  if(ARG_LINK_FLAG)
    # link flags
    execute_process(COMMAND "${cfg_exe}"
      ${ARG_LINK_FLAG} ${ARG_ADDITIONAL_FLAGS}
      RESULT_VARIABLE result
      OUTPUT_VARIABLE ldflags
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    cfg_exe_check_config_output("ld flags")
    separate_arguments(ldflags)
  else()
    set(ldflags "")
  endif()
  if(ARG_LIB_FLAG)
    # libraries
    execute_process(COMMAND "${cfg_exe}"
      ${ARG_LIB_FLAG} ${ARG_ADDITIONAL_FLAGS}
      RESULT_VARIABLE result
      OUTPUT_VARIABLE libs
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    cfg_exe_check_config_output("libraries")
    separate_arguments(libs)
  else()
    set(libs "")
  endif()
  # Use both outputs in case only one is used
  list(APPEND ldflags "${libs}")
  set(tmp_flags "")
  set(library_dirs "")
  # Get only link directories
  foreach(flag IN LISTS ldflags)
    if(flag MATCHES "^-L(.*)")
      list(APPEND library_dirs "${CMAKE_MATCH_1}")
    else()
      list(APPEND tmp_flags "${flag}")
    endif()
  endforeach()
  set(libraries "")
  set(lib_vars "")
  set(ldflags_other "")
  # Get libraries
  set(continue_next FALSE)
  foreach(flag IN LISTS tmp_flags)
    if(NOT flag)
      continue()
    endif()
    if(continue_next)
      set(continue_next FALSE)
      continue()
    endif()
    if(flag MATCHES "^-l(.*)")
      set(lib_name "${CMAKE_MATCH_1}")
      string(MAKE_C_IDENTIFIER ${lib_name} var_name)
      set(var_name "${prefix}-${var_name}")
      find_library(${var_name}
        NAMES "${lib_name}"
        HINTS ${library_dirs}
        NO_DEFAULT_PATH
      )
      find_library(${var_name}
        NAMES "${lib_name}"
      )
      list(APPEND lib_vars "${var_name}")
      if(${var_name})
        list(APPEND libraries "${${var_name}}")
      endif()
    else()
      # Filter out rpath flags. Not required and broken in CMake (CMake filters duplicate '-Wl,-rpath')
      if(flag MATCHES "^-Wl,-rpath$")
        set(continue_next TRUE)
        continue()
      endif()
      list(APPEND ldflags_other "${flag}")
    endif()
  endforeach()
  set(${prefix}_LIBRARIES "${libraries}" PARENT_SCOPE)
  set(${prefix}_LIBRARY_DIRS "${library_dirs}" PARENT_SCOPE)
  set(${prefix}_LDFLAGS "${ldflags}" PARENT_SCOPE)
  set(${prefix}_LDFLAGS_OTHER "${ldflags_other}" PARENT_SCOPE)
  set(${prefix}_LIB_VARS "${lib_vars}" PARENT_SCOPE)

  if(ARG_IMPORTED_TARGET_NAME)
    add_library(${ARG_IMPORTED_TARGET_NAME} INTERFACE IMPORTED)
    set_target_properties(${ARG_IMPORTED_TARGET_NAME} PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${include_dirs}"
      INTERFACE_LINK_LIBRARIES "${libraries};${ldflags_other}"
      INTERFACE_COMPILE_OPTIONS "${cflags_other}"
    )
  endif()
endfunction()
