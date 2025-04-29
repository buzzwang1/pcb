# https://stackoverflow.com/questions/19854613/cmake-toolchain-includes-multiple-files
# To avoid, that the toolchain is multiple loaded
if (NOT PCB_ToolchainInstalled)
  SET(PCB_ToolchainInstalled "yes")
  INCLUDE(CMakeForceCompiler)

  #https://stackoverflow.com/questions/10599038/can-i-skip-cmake-compiler-tests-or-avoid-error-unrecognized-option-rdynamic
  SET(CMAKE_SYSTEM_NAME Generic)
  SET(CMAKE_SYSTEM_PROCESSOR arm)
  SET(CMAKE_CROSSCOMPILING 1)

  if (("${CMAKE_MAKE_PROGRAM}" STREQUAL "") OR 
      ("${CMAKE_MAKE_PROGRAM}" STREQUAL "CMAKE_MAKE_PROGRAM-NOTFOUND"))
    SET(CMAKE_MAKE_PROGRAM "${PCB_ProjCfg_system_dir_tools}/ninja/ninja.exe"     CACHE FILEPATH "" FORCE)
    message(STATUS "Make tool: ${PCB_ProjCfg_system_dir_tools}/ninja/ninja.exe")
  endif()
  if ("${DIR_GNU_RISCV}" STREQUAL "")
    SET(DIR_GNU_RISCV "${PCB_ProjCfg_system_dir_tools}/gcc/RiscV/bin/")
  endif()
  message(STATUS "Build tool: ${DIR_GNU_RISCV}")

  #https://stackoverflow.com/questions/53633705/cmake-the-c-compiler-is-not-able-to-compile-a-simple-test-program
  SET(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
  SET(CMAKE_C_COMPILER_WORKS 1)
  SET(CMAKE_CXX_COMPILER_WORKS 1)    

  message(STATUS "Make tool: ${CMAKE_MAKE_PROGRAM}")
  message(STATUS "Toolchain path: ${DIR_GNU_ARM_CORTEX}")
  #SET(CMAKE_AR                          ${DIR_GNU_RISCV}riscv-none-embed-ar.exe       CACHE FILEPATH "" FORCE)
  SET(CMAKE_C_COMPILER                  "${DIR_GNU_RISCV}riscv-none-embed-gcc.exe"     CACHE FILEPATH "" FORCE)
  SET(CMAKE_CXX_COMPILER                "${DIR_GNU_RISCV}riscv-none-embed-g++.exe"     CACHE FILEPATH "" FORCE)
  #SET(CMAKE_ASM_COMPILER                ${DIR_GNU_RISCV}riscv-none-embed-as.exe       CACHE FILEPATH "" FORCE)
  #SET(CMAKE_LINKER                      ${DIR_GNU_RISCV}riscv-none-embed-ld.exe       CACHE FILEPATH "" FORCE)
  #SET(CMAKE_NM                          ${DIR_GNU_RISCV}riscv-none-embed-nm.exe       CACHE FILEPATH "" FORCE)
  #SET(CMAKE_OBJDUMP                     ${DIR_GNU_RISCV}riscv-none-embed-objdump.exe  CACHE FILEPATH "" FORCE)
  #SET(CMAKE_STRIP                       ${DIR_GNU_RISCV}riscv-none-embed-strip.exe    CACHE FILEPATH "" FORCE)
  #SET(CMAKE_SIZE                        ${DIR_GNU_RISCV}riscv-none-embed-size.exe     CACHE FILEPATH "" FORCE)


  # overwrite the GCC way to generate header dependencies
  SET(CMAKE_DEPFILE_FLAGS_ASM "-MD -MF=<DEPFILE>")
  SET(CMAKE_DEPFILE_FLAGS_C   "-MD -MF=<DEPFILE>")
  SET(CMAKE_DEPFILE_FLAGS_CXX "-MD -MF=<DEPFILE>")
  
  SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
  SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
  SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
else()
  message(STATUS "Toolchain already installed. Skipping toolchain installation")
endif()
