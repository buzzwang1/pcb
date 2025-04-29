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


set root_dir="-root_dir=%PCB_ENVIRONMENT_ROOT_DIR%"
set build_dir="-build_dir=%PCB_ENVIRONMENT_BUILD_DIR%"
set source_dir="-source_dir=%PCB_ENVIRONMENT_SOURCE_DIR%"
set tools_dir="-tools_dir=%PCB_ENVIRONMENT_TOOLS_DIR%"


:: 1. Paramter: Commands
:: 2. Paramter: Variant
:: 3. Paramter: Selectionmode

@call python scripts/toolcaller.py -cmds=%1 -project=%2  -selectionmode=%3 %root_dir% %build_dir% %source_dir% %tools_dir% || echo failed toolcaller.py && goto :error

:success
@set  PATH=%PathSave%
@exit /B 0

:error
@echo failure in toolcaller.bat
@set  PATH=%PathSave%
@exit /B 1