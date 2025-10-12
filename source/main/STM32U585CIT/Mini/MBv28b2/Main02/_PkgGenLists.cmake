# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "main")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./System/StageHdl/S1/cSysStageHdlS1.cpp
	./System/StageHdl/S2/cSysStageHdlS2.cpp
	./System/StageHdl/S4/cSysStageHdlS4.cpp
	./System/StageHdl/S5/cSysStageHdlS5.cpp
	./System/StageHdl/cSysStageHdl.cpp
	./System/Stages/S1/cSysStageS1.cpp
	./System/Stages/S2/cSysStageS2.cpp
	./System/Stages/S4/cSysStageS4.cpp
	./System/Stages/S5/cSysStageS5.cpp
	./System/Stages/cSysStage.cpp
	./System/Stages/cSysStages.cpp
	./System/Task/4/cSysTaskClock.cpp
	./System/Task/4/cSysTaskMem.cpp
	./System/Task/5/cSysTaskBoard2.cpp
	./main.cpp
	./syscalls.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./System/StageHdl/S1/cSysStageHdlS1.h
	./System/StageHdl/S2/cSysStageHdlS2.h
	./System/StageHdl/S4/cSysStageHdlS4.h
	./System/StageHdl/S5/cSysStageHdlS5.h
	./System/StageHdl/cSysStageHdl.h
	./System/Stages/S1/cSysStageS1.h
	./System/Stages/S2/cSysStageS2.h
	./System/Stages/S4/cSysStageS4.h
	./System/Stages/S5/cSysStageS5.h
	./System/Stages/cSysStage.h
	./System/Stages/cSysStages.h
	./System/Task/4/cSysTaskClock.h
	./System/Task/4/cSysTaskMem.h
	./System/Task/4/cSysTaskS4.h
	./System/Task/5/cSysTaskBoard2.h
	./System/Task/5/cSysTaskS5.h
	./main.h
	./stm32u5xx_hal_conf.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./System/StageHdl/S1
	./System/StageHdl/S2
	./System/StageHdl/S4
	./System/StageHdl/S5
	./System/StageHdl
	./System/Stages/S1
	./System/Stages/S2
	./System/Stages/S4
	./System/Stages/S5
	./System/Stages
	./System/Task/4
	./System/Task/5
	.
)


# List of all source files excluded by the user
set (gen_excluded_sourcefiles 
)


# List of all header files excluded by the user
set (gen_excluded_headerfiles 
)


# List of all include folders excluded by the user
set (gen_excluded_includes 
)


# Final list of source files. allfiles minus excluded files
if (gen_files AND gen_excluded_sourcefiles)
    list(REMOVE_ITEM gen_files ${gen_excluded_sourcefiles})
endif()

# Final list of source files. allfiles minus excluded files
if (gen_asmfiles AND gen_excluded_sourcefiles)
    list(REMOVE_ITEM gen_asmfiles ${gen_excluded_sourcefiles})
endif()

# Final list of header files. allfiles minus excluded headers
if (gen_headers AND gen_excluded_headerfiles)
    list(REMOVE_ITEM gen_headers ${gen_excluded_headerfiles})
endif()

# Final list of included folders. allfolders minus excluded folders
if (gen_include_directories AND gen_excluded_includes)
    list(REMOVE_ITEM gen_include_directories ${gen_excluded_includes})
endif()

