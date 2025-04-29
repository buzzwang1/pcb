::Prepared template file for use of Conan and set the environment variables
::To use this, Conan must be installed and Conan cache must be correct.
@set  PathSave=%PATH%
@call scripts/prepare.bat || echo failed prepare.bat && goto :error


IF "%PCB_ENVIRONMENT_ROOT_DIR%"=="" (
	@echo Variable PCB_ENVIRONMENT_ROOT_DIR is NOT defined
	goto :error	
)
IF "%PCB_ENVIRONMENT_BUILD_DIR%"=="" (
	@echo Variable PCB_ENVIRONMENT_BUILD_DIR is NOT defined
	goto :error
)
IF "%PCB_ENVIRONMENT_SOURCE_DIR%"=="" (
	@echo Variable PCB_ENVIRONMENT_SOURCE_DIR is NOT defined
	goto :error
)

::Check den given arguments
:: 0 args given: project=*           selectionmode=select_build => User can select from all available project
:: 1 arg  given: project=[argument1] selectionmode=first        => First project matches [argument1] will be used
:: 2 args given: project=[argument1] selectionmode=[argument2]  => See toolcaller documentation

::Count the given arguments
set TcArgCount="999"
IF "%3"=="" set TcArgCount="2"
IF "%2"=="" set TcArgCount="1"
IF "%1"=="" set TcArgCount="0"

::Exit, in case more then 2 arguments were entered
IF %TcArgCount%=="999" (
  @echo Maximum 2 arguments are allowed
	goto :error
)

set Tcproject=*
set TcSelectionMode=select_build

IF %TcArgCount%=="1" (
  set Tcproject=%1
  set TcSelectionMode=first
)

IF %TcArgCount%=="2" (
  set Tcproject=%1
  set TcSelectionMode=%2
)

:: ToolCaller Commands 
set TcCmds=reports/memory/call


:: Start of ToolCaller 
@call python scripts/toolcaller.py -cmds=%TcCmds% -project=%Tcproject% -selectionmode=%TcSelectionMode% -root_dir=%PCB_ENVIRONMENT_ROOT_DIR% -build_dir=%PCB_ENVIRONMENT_BUILD_DIR% -source_dir=%PCB_ENVIRONMENT_SOURCE_DIR% -tools_dir=%PCB_ENVIRONMENT_TOOLS_DIR% || echo failed toolcaller.py && goto :error

:success
@set  PATH=%PathSave%
@exit /B 0

:error
@echo failure in Debug.bat
@set  PATH=%PathSave%
@exit /B 1