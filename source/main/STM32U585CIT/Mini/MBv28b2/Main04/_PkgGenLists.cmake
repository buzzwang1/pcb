# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "main")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./main.cpp
	./syscalls.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./Comp/cComp3V3.h
	./Comp/cCompAddOn.h
	./Comp/cCompAddOn5V0.h
	./Comp/cCompAddOn5V0CanFd1.h
	./Comp/cCompAddOn5V0Out.h
	./Comp/cCompAddOnBal.h
	./Comp/cCompAddOnBalIna.h
	./Comp/cCompAddOnBalPio.h
	./Comp/cCompAddOnBatOut.h
	./Comp/cCompAddOnCharger.h
	./Comp/cCompAddOnUsb.h
	./Comp/cCompBase.h
	./Comp/cCompBoardI2C2.h
	./Comp/cCompBoardMonitor.h
	./Comp/cCompComIn.h
	./Comp/cCompComOut.h
	./Comp/cCompComOutCntrl.h
	./Comp/cCompGfx.h
	./Comp/cCompGfxSpi2.h
	./Comp/cCompLed.h
	./Comp/cCompPomoOut1.h
	./Comp/cCompPomoOut1Temp.h
	./Comp/cCompRadioNrf905.h
	./Comp/cCompRadioSpi1.h
	./SysDPool/cSysDPool.h
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
	./Comp
	./SysDPool
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

