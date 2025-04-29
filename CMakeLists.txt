#    Filename : [dir_root]/CMakeLists.txt
# Description : root cmake file
#


# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.0)
message(STATUS "CMake Version: ${CMAKE_VERSION}")

#set(PCB_DEBUG "On")

# Check for the PCB_Project's configuration
message(STATUS "Check for the PCB_Project's configuration.")
set (PCB_ProjectCfgFileExists "False")
if (PCB_ProjectCfgFile)
  if(EXISTS ${PCB_ProjectCfgFile})
    message(STATUS "PCB_Project's configuration found.")
    set (PCB_ProjectCfgFileExists "True")
  endif()
endif()

# If it is missing, try to gernerate it.
if (PCB_ProjectCfgFileExists STREQUAL "False")
  message(STATUS "Try to generate PCB_Project's ${PCB_ProjectLabel} configuration: ${PCB_ProjectCfgFile}")
  if (PCB_ProjectLabel)
    message(STATUS "Call: tc.bat Projects/ExportCfgCm ${PCB_ProjectLabel} all")
    execute_process(
      COMMAND tc.bat Projects/ExportCfgCm ${PCB_ProjectLabel} all
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

    execute_process(
      COMMAND PkgGenLists.bat ${PCB_ProjectLabel} all
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

    while(PCB_ProjectCfgFileExists STREQUAL "False")
      if(EXISTS ${PCB_ProjectCfgFile})
        message(STATUS "PCB_Project's configuration found.")
        set (PCB_ProjectCfgFileExists "True")
      endif()
    endwhile()

  else()
    message(FATAL_ERROR "FATAL_ERROR: Project's configuration could not be generated !")
  endif()
endif()

# Try to include the PCB_Project's configuration
if (PCB_ProjectCfgFile)
  if(EXISTS ${PCB_ProjectCfgFile})
    message(STATUS "Include PCB_Project's configuration: ${PCB_ProjectCfgFile}")
    include("${PCB_ProjectCfgFile}")

    #Take over some variables from the configuration
    set(PCB_Project      "${PCB_ProjCfg_project_name}"   CACHE STRING "" FORCE)
    set(PCB_ProjectLabel "${PCB_ProjCfg_project_label}"  CACHE STRING "" FORCE) 
    set(PCB_ProjectCfg   "${PCB_ProjCfg_project_config}" CACHE STRING "" FORCE)
  endif()
endif()

if (PCB_ProjectCfgFileExists STREQUAL "False")
  if (PCB_ProjectCfgFile STREQUAL "")
    message(FATAL_ERROR "FATAL_ERROR: Project's configuration file is undefined !")
  else()
    message(FATAL_ERROR "FATAL_ERROR: Project's configuration file is missing ! Should be here: ${PCB_ProjectCfgFile}")
  endif()
endif()

#----------------------------------------------------------------------------------

if (NOT "${CMAKE_TOOLCHAIN_FILE}" STREQUAL "")
  message(STATUS "Include ToolChain file: ${CMAKE_TOOLCHAIN_FILE}")
  include("${CMAKE_TOOLCHAIN_FILE}")
endif()

# https://stackoverflow.com/questions/10599038/can-i-skip-cmake-compiler-tests-or-avoid-error-unrecognized-option-rdynamic
# Skip the compiler checks by adding NONE to your project call
project(${PCB_Project} NONE)

enable_language(C CXX ASM)
file(GLOB_RECURSE ASSEMBLER_FILES *.arm *.800)
set_source_files_properties(${ASSEMBLER_FILES} PROPERTIES LANGUAGE ASM)
  
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if ((PCB_ProjCfg_system_project_target_platform STREQUAL "MsWin") OR
    (PCB_ProjCfg_system_project_target_platform STREQUAL "MsWinConsole"))
  set(PCB_TargetName          "${PCB_Project}")
  set(PCB_TargetWIN32         "WIN32")
else()
  set(PCB_TargetName          "${PCB_Project}.elf")
  set(PCB_TargetWIN32         "")
endif()
message(STATUS "Target platform: ${PCB_ProjCfg_system_project_target_platform}")
message(STATUS "Target name:     ${PCB_TargetName}")

set(PCB_ROOT_BUILD_DIRECTORY "${CMAKE_BINARY_DIR}"            CACHE STRING "" FORCE)
set(PCB_ROOT_PCB_Project     "${PCB_Project}"                 CACHE STRING "" FORCE)

# Set some variables in the CMakeCache.txt, that other tools&scripts can use it, e.g. for flashing/debugging
get_filename_component(PCB_COMPILER_PATH "${CMAKE_C_COMPILER}" DIRECTORY)
set(PCB_CompilerPath   "${PCB_COMPILER_PATH}"              CACHE STRING "" FORCE)
set(PCB_DebuggerPath   "${PCB_COMPILER_PATH}/_ide"         CACHE STRING "" FORCE)
set(PCB_BinaryFilename "${PCB_TargetName}"                 CACHE STRING "" FORCE)

# path of binary, PCB_Project depending, also write it to the cache, that is readable for other tools
set(EXECUTABLE_OUTPUT_PATH "${PCB_ProjCfg_system_dir_project_build_bin}"  CACHE STRING "" FORCE)

# path of binary'S libs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PCB_ROOT_BUILD_DIRECTORY}/lib")
# For static libraries you have to set the CMAKE_ARCHIVE_OUTPUT_DIRECTORY, not the CMAKE_LIBRARY_OUTPUT_DIRECTORY
# https://cmake.org/cmake/help/latest/variable/CMAKE_ARCHIVE_OUTPUT_DIRECTORY.html
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PCB_ROOT_BUILD_DIRECTORY}/lib")

# Just write this somewhere, to avoid the "CMAKE_TOOLCHAIN_FILE was not used by the project" message
set(CMAKE_TOOLCHAIN_FILE_DUMMY "${CMAKE_TOOLCHAIN_FILE}")

# Make list files
if (PCB_TargetWIN32 STREQUAL "")
  set(CMAKE_C_COMPILE_OBJECT "${CMAKE_C_COMPILE_OBJECT} -Wa,-alh=<OBJECT>.lst")
  set(CMAKE_CXX_COMPILE_OBJECT "${CMAKE_CXX_COMPILE_OBJECT} -Wa,-alh=<OBJECT>.lst")
endif()

# For link libraries
#cmake_policy(SET CMP0079 NEW)



# 1. Initiate several lists
#    has to be filled from the subdirectory
set(PCB_ListTargetExeFiles)
set(PCB_ListTargetIncDirs)
set(PCB_ListTargetLinkLibs)

set(PCB_ListTargetDefinition)
set(PCB_ListTargetCompileOptionsCommon)
set(PCB_ListTargetCompileOptionsAsm)
set(PCB_ListTargetCompileOptionsC)
set(PCB_ListTargetCompileOptionsCpp)
set(PCB_ListTargetLinkOptions)

# 2. Try to includes ${project}'s specific settings located in [dir_root]/cfg/prj/${PCB_ProjectLabel}/project.cmake
message(STATUS "Include the project's config file: ${PCB_ProjCfg_system_dir_project_cfg_root}/project.cmake")
include("${PCB_ProjCfg_system_dir_project_cfg_root}/project.cmake")

# 3. Add packages
add_subdirectory(${PCB_ProjCfg_system_dir_source})


# Remove leading ";" by checking if the first position(=0) of the string is equal to ";"
# Remove leading ";" for PCB_ListTargetExeFiles
  string(FIND "${PCB_ListTargetExeFiles}" ";" liSemiPos)
  if (${liSemiPos} EQUAL 0)
    string(SUBSTRING "${PCB_ListTargetExeFiles}" 1 -1 PCB_ListTargetExeFiles)
  endif()
# Remove leading ";" for PCB_ListTargetIncDirs  
  string(FIND "${PCB_ListTargetIncDirs}" ";" liSemiPos)
  if (${liSemiPos} EQUAL 0)
    string(SUBSTRING "${PCB_ListTargetIncDirs}" 1 -1 PCB_ListTargetIncDirs)
  endif()
# Remove leading ";" for PCB_ListTargetLinkLibs  
  string(FIND "${PCB_ListTargetLinkLibs}" ";" liSemiPos)
  if (${liSemiPos} EQUAL 0)
    string(SUBSTRING "${PCB_ListTargetLinkLibs}" 1 -1 PCB_ListTargetLinkLibs)
  endif()

message(STATUS "-----------------------------------")
message(STATUS "Exe files: ${PCB_ListTargetExeFiles}")
message(STATUS "Inc folders: ${PCB_ListTargetIncDirs}")
message(STATUS "Lib list: ${PCB_ListTargetLinkLibs}")
if (PCB_DEBUG)
  message(STATUS "Linker file: ${LINKER_SCRIPT}")
  message(STATUS "Linker flags: ${PCB_ListTargetLinkOptions}")
  message(STATUS "Definition list: ${PCB_ListTargetDefinition}")
  message(STATUS "Compile common list: ${PCB_ListTargetCompileOptionsCommon}")
  message(STATUS "Compile asm list: ${PCB_ListTargetCompileOptionsAsm}")
  message(STATUS "Compile c list: ${PCB_ListTargetCompileOptionsC}")
  message(STATUS "Compile c++ list: ${PCB_ListTargetCompileOptionsCpp}")
endif()

# 5. Set build options for the used library
foreach(lLibrary ${PCB_ListTargetLinkLibs})
  get_target_property(target_type ${lLibrary} TYPE)
  if (target_type STREQUAL "INTERFACE_LIBRARY")
    if (PCB_DEBUG)
      message(STATUS "Skipped ${target_type} ${lLibrary}")
    endif()
  else()
    # link all libs to each other. Then this must not be done manually
    set (lLstTemp ${PCB_ListTargetLinkLibs})
    list(REMOVE_ITEM lLstTemp "${lLibrary}")
    set_target_properties(${lLibrary} PROPERTIES LINK_LIBRARIES "${lLstTemp}")
  
    #if (PCB_DEBUG)
      get_target_property(target_link_libs ${lLibrary} LINK_LIBRARIES)
      message(STATUS "Set properties for ${lLibrary}. Linked to ${target_link_libs}")
    #endif()

    # The CMake 'BEFORE' in target_compile_options didn't work, so I cut the given COMPILE_OPTIONS here
    # and append them later again 
    # It seems, that the option in front are weaker, so that options, that were added by the pkg 
    # must be located at the end, to be stronger then then global options
    get_target_property(lTemp ${lLibrary} COMPILE_OPTIONS)
    if (lTemp)
      set_target_properties(${lLibrary} PROPERTIES COMPILE_OPTIONS "")	
    endif()
    target_compile_definitions(${lLibrary}  PRIVATE ${PCB_ListTargetDefinition})
    
    if (PCB_TargetWIN32 STREQUAL "")
      target_compile_options(    ${lLibrary}  BEFORE PRIVATE ${PCB_ListTargetCompileOptionsCommon})
      target_compile_options(    ${lLibrary}  BEFORE PRIVATE $<$<COMPILE_LANGUAGE:ASM>:${PCB_ListTargetCompileOptionsAsm}>)
      target_compile_options(    ${lLibrary}  BEFORE PRIVATE $<$<COMPILE_LANGUAGE:C>:${PCB_ListTargetCompileOptionsC}>)
      target_compile_options(    ${lLibrary}  BEFORE PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${PCB_ListTargetCompileOptionsCpp}>)
    endif()
    if (lTemp)
      #### # Check if there is a optimization option in the local list
      #### set(lszStr_lTemp "${lTemp}")
      #### #  message(STATUS "local: ${lszStr_lTemp}") # => e.g. "local: -Ofast"
      #### string(FIND "${lszStr_lTemp}" "-O" lIndexOpt)
      #### if (${lIndexOpt} GREATER -1)
      ####   # Check if there is a optimization option in the global list
      ####   get_target_property(lTemp1 ${lLibrary} COMPILE_OPTIONS )
      ####   set(lszStr_lTemp1 "${lTemp1}")
      ####   # message(STATUS "global: ${lszStr_lTemp1}") # => e.g. "global: ;-Os;-mcpu=cortex-m4;-mthumb;-mfloat-abi=hard;-mfpu=fpv4-sp-d16;-Wall;-g3;-fdata-sections;-ffunction-sections;-Wdouble-promotion;$<$<COMPILE_LANGUAGE:ASM>:>;$<$<COMPILE_LANGUAGE:C>:-std=c11;-fmessage-length=0;-fsigned-char;-fdata-sections;-Wextra;-std=gnu99>;$<$<COMPILE_LANGUAGE:CXX>:-fmessage-length=0;-fsigned-char;-fdata-sections;-Wextra;-std=gnu++17;-fabi-version=6;-fno-exceptions;-fno-rtti;-fno-use-cxa-atexit;-fno-threadsafe-statics;-nostdinc++>"
      ####   set(lszStr_lFound "")
      ####   string(FIND "${lszStr_lTemp1}" "-O" lIndexOpt1)
      ####   if (${lIndexOpt1} GREATER -1)
      ####     foreach(lOption ${lTemp1})
      ####       set(lszStr_lOption "${lOption}")
      ####       string(FIND "${lszStr_lOption}" "-O" lIndexOpt2)
      ####       if (${lIndexOpt2} GREATER -1)
      ####         set(lszStr_lFound "${lszStr_lOption}")
      ####         # message(STATUS "global: ${lszStr_lOption}") # => e.g. "global: -Os"
      ####       endif()
      ####     endforeach()
      ####   # remove the option
      ####   # list (REMOVE_ITEM lTemp1 "${lszStr_lFound}")
      ####   list (REMOVE_ITEM lTemp1 "${lszStr_lFound}")
      ####   set_target_properties(${lLibrary} PROPERTIES COMPILE_OPTIONS "${lTemp1}" )
      ####   endif()
      #### endif()
      #### 
      #### target_compile_options(  ${lLibrary}  PRIVATE ${lTemp})
      #### get_target_property(lTemp1 ${lLibrary} COMPILE_OPTIONS )
      #### set(lszStr_lTemp1 "${lTemp1}")
      #### message(STATUS "  Target compile options: ${lszStr_lTemp1}") # => e.g. "global: ;-mcpu=cortex-m4;-mthumb;-mfloat-abi=hard;-mfpu=fpv4-sp-d16;-Wall;-g3;-fdata-sections;-ffunction-sections;-Wdouble-promotion;$<$<COMPILE_LANGUAGE:ASM>:>;$<$<COMPILE_LANGUAGE:C>:-std=c11;-fmessage-length=0;-fsigned-char;-fdata-sections;-Wextra;-std=gnu99>;$<$<COMPILE_LANGUAGE:CXX>:-fmessage-length=0;-fsigned-char;-fdata-sections;-Wextra;-std=gnu++17;-fabi-version=6;-fno-exceptions;-fno-rtti;-fno-use-cxa-atexit;-fno-threadsafe-statics;-nostdinc++>"
    endif()
  endif()
endforeach()


# 6. Optional includes ${project}'s specific settings located in [dir_root]/projects/${PCB_ProjectLabel}/project_post.cmake
if(EXISTS ${PCB_ProjCfg_system_dir_project_cfg_root}/project_post.cmake)
  message(STATUS "Include project's post file: ${PCB_ProjCfg_system_dir_project_cfg_root}/project_post.cmake")
  include("${PCB_ProjCfg_system_dir_project_cfg_root}/project_post.cmake")
endif()


# 7. Set build option and linker flags for the binary
add_executable(            ${PCB_TargetName} ${PCB_ListTargetExeFiles})
target_include_directories(${PCB_TargetName} PUBLIC ${PCB_ListTargetIncDirs})
target_link_libraries(     ${PCB_TargetName} PUBLIC ${PCB_ListTargetLinkLibs})

string(REGEX REPLACE ";" " " lszTemp "${PCB_ListTargetLinkOptions}")
set_target_properties(     ${PCB_TargetName} PROPERTIES LINK_FLAGS   ${lszTemp})

if (PCB_TargetWIN32 STREQUAL "")
  target_compile_options(    ${PCB_TargetName} PUBLIC ${PCB_ListTargetCompileOptionsCommon})
  target_compile_options(    ${PCB_TargetName} PUBLIC $<$<COMPILE_LANGUAGE:ASM>:${PCB_ListTargetCompileOptionsAsm}>)
  target_compile_options(    ${PCB_TargetName} PUBLIC $<$<COMPILE_LANGUAGE:C>:${PCB_ListTargetCompileOptionsC}>)
  target_compile_options(    ${PCB_TargetName} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${PCB_ListTargetCompileOptionsCpp}>)
endif()



target_compile_definitions(${PCB_TargetName} PUBLIC ${PCB_ListTargetDefinition})

#rename to elf file
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy "${EXECUTABLE_OUTPUT_PATH}/${PCB_TargetName}" "${EXECUTABLE_OUTPUT_PATH}/${PCB_TargetName}.elf"
#  COMMENT "copy ${EXECUTABLE_OUTPUT_PATH}/${PCB_TargetName} ${EXECUTABLE_OUTPUT_PATH}/${PCB_TargetName}.elf"
#)


if (PCB_DEBUG)
  #enable for debugging, prints all cmake variables
  get_cmake_property(_variableNames VARIABLES)
  list (SORT _variableNames)
  foreach (_variableName ${_variableNames})
      message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endif()

