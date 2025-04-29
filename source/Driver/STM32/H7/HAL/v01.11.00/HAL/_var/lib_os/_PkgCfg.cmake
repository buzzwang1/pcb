# Uncomment that line, in case of 3rd party package
set(PkgCfg_3rd_party 1)

# Uncomment for link libraries
set(PkgCfg_ListLinkLibraries core_lib ${CMAKE_CURRENT_LIST_DIR}/libhal_lib_os.a)

set(PkgCfg_ListTargetExeFiles "${CMAKE_CURRENT_LIST_DIR}/main.cpp;${CMAKE_CURRENT_LIST_DIR}/syscalls.c")

# Optional set package specific compile options
# e.g. Enable the following line in case you need NO-Optimizations for debugging
set(PkgCfg_TargetCompileOption -Os)

# 3rd party. Turn off all warnings
if (PkgCfg_3rd_party AND (NOT PCB_ShowWarnings))
  message(STATUS "  3rd party => warnings off")
  if (PkgCfg_TargetCompileOption)
    set(PkgCfg_TargetCompileOption -w)
  else()
    list(APPEND PkgCfg_TargetCompileOption -w)
  endif()
endif()

set(PkgCfg_LibOptiLevel os)
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/libhal_lib_${PkgCfg_LibOptiLevel}.a")
  message(STATUS "  Lib ${CMAKE_CURRENT_LIST_DIR}/libhal_lib_${PkgCfg_LibOptiLevel}.a is missing. Trying to create")
  message(STATUS "  This will take a moment ...")
  execute_process(
      COMMAND build.bat STM/L4/HalLib/v01.17.00.${PkgCfg_LibOptiLevel} first
      WORKING_DIRECTORY ${PCB_ProjCfg_system_dir_root})
      
  file(COPY ${PCB_ProjCfg_system_dir_build}/STM/L4/HalLib/v01.17.00/${PkgCfg_LibOptiLevel}/cmake/lib/libhal_lib.a
        DESTINATION ${CMAKE_CURRENT_LIST_DIR})
	
  file(RENAME ${CMAKE_CURRENT_LIST_DIR}/libhal_lib.a ${CMAKE_CURRENT_LIST_DIR}/libhal_lib_${PkgCfg_LibOptiLevel}.a)
  
  file(REMOVE_RECURSE ${PCB_ProjCfg_system_dir_build}/STM/L4/HalLib/v01.17.00/${PkgCfg_LibOptiLevel})
endif()

