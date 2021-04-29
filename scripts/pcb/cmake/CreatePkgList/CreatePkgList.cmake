# This script converts the cmake coded package list in a text file
# Must be calles in script mode "-P" https://cmake.org/cmake/help/latest/manual/cmake.1.html#options
# Input param by cmake define:      
#    - PCB_Project:  link to the project, that package list should be converted
#    - dest_file:    Destination file (complete path+file), where the package list is written

# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.0)

# Project's name
# https://stackoverflow.com/questions/46176955/is-it-possible-to-use-cmake-without-any-compiler


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
else()
    message(FATAL_ERROR "FATAL_ERROR: Project's configuration file is missing ! Should be here ${PCB_ProjectCfgFile}")
endif()

# The project's package list is located in repo/cfg/prj/[project]/project.cmake
message(STATUS "Include the project's config file: ${PCB_ProjCfg_system_dir_cfg_project}/project.cmake")
include("${PCB_ProjCfg_system_dir_project_cfg_root}/project.cmake")


# The package list is written in "${project_package_list}"
# Just plot this in a file
message(STATUS "Write package list to ${dest_file}")


# sort the list before writing to file
list(SORT PCB_ProjectPackageList)

string(REGEX REPLACE ";" "\n" PCB_ProjectPackageList2 "${PCB_ProjectPackageList}")
FILE(WRITE ${dest_file} "${PCB_ProjectPackageList2}")

