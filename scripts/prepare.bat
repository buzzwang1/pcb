@echo off
::Used to initialize the scripts. Could be relative or absolute path
@set PCB_ENVIRONMENT_ROOT_DIR=%CD%
@set PCB_ENVIRONMENT_BUILD_DIR=bin
@set PCB_ENVIRONMENT_SOURCE_DIR=source
@set PCB_ENVIRONMENT_TOOLS_DIR=%PCB_ENVIRONMENT_ROOT_DIR%/../pcb_tools


::Toolchain settings
@set PATH="%PCB_ENVIRONMENT_TOOLS_DIR%/cmake/bin";%PATH%
@set PATH="%PCB_ENVIRONMENT_TOOLS_DIR%/gcc/bin";%PATH%
@set PATH="%PCB_ENVIRONMENT_TOOLS_DIR%/stlink";%PATH%
@set PATH="%PCB_ENVIRONMENT_TOOLS_DIR%/Programmer/bin";%PATH%
@set PATH="%PCB_ENVIRONMENT_TOOLS_DIR%/ninja";%PATH%
@set PATH="%PCB_ENVIRONMENT_TOOLS_DIR%/EmBitz";%PATH%
@set PATH="%PCB_ENVIRONMENT_TOOLS_DIR%/amap";%PATH%
@exit /B 0