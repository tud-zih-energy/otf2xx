option(OTF2_USE_STATIC_LIBS "Link OTF2 statically." OFF)
if(OTF2_USE_STATIC_LIBS)
  set(_OTF2_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
  set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
endif()

include(CfgExeSearchModule)
cfg_exe_search_module(OTF2 otf2-config
  HINTS ENV OTF2_ROOT "${OTF2_CONFIG_PATH}"
  PATHS /opt/otf2
  VERSION_FLAG --interface-version
  IMPORTED_TARGET
)
string(REPLACE ":" "." OTF2_VERSION "${OTF2_VERSION}")
if(OTF2_CONFIG)
  get_filename_component(config_path "${OTF2_CONFIG}" DIRECTORY)
  find_program(OTF2_PRINT "otf2-print" HINTS "${config_path}" "${OTF2_CONFIG_PATH}")

  if(python_bindings IN_LIST OTF2_FIND_COMPONENTS)
    execute_process (COMMAND "${OTF2_CONFIG}" --pythonpath
        RESULT_VARIABLE otf2_result
        OUTPUT_VARIABLE OTF2_python_bindings
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(OTF2_python_bindings_FOUND 0)
    if(otf2_result AND EXISTS "${OTF2_python_bindings}")
      set(OTF2_python_bindings_FOUND 1)
    else()
      get_filename_component(otf2_bin_path "${OTF2_CONFIG_EXE}" DIRECTORY)
      get_filename_component(otf2_root_path "${otf2_bin_path}" DIRECTORY)
      find_path (OTF2_python_bindings
        otf2/__init__.py
        HINTS "${otf2_bin_path}" "${otf2_root_path}" ENV OTF2_ROOT
        PATH_SUFFIXES lib/python2.7/site-packages python2.7/site-packages
      )
      if (OTF2_python_bindings)
        set(OTF2_python_bindings_FOUND 1)
      endif()
    endif()
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OTF2
  REQUIRED_VARS OTF2_CONFIG OTF2_PRINT OTF2_INCLUDE_DIRS ${OTF2_LIB_VARS}
  VERSION_VAR OTF2_VERSION
  HANDLE_COMPONENTS
)

if(OTF2_USE_STATIC_LIBS)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ${_OTF2_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
endif()
