# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "gfxseng")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./Base/SpriteEngine_base.cpp
	./Bpp1_1G/BPP1_1G_SpriteEngine.cpp
	./SEngine_Res8b_Bpp1_1G/v00.00.01/Base/Res8b_Bpp1_1G_SpriteEngine_Base.cpp
	./SEngine_Res8b_Bpp1_1G/v00.00.01/Bpp16_5R6G5B/Res8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B.cpp
	./SEngine_Res8b_Bpp1_1G/v00.00.01/Bpp1_1G/Res8b_Bpp1_1G_SpriteEngine_Bpp1_1G.cpp
	./SpriteEngine.cpp
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./Base/SpriteEngine_base.h
	./Bpp1_1G/BPP1_1G_SpriteEngine.h
	./SEngine_Res8b_Bpp1_1G/v00.00.01/Base/Res8b_Bpp1_1G_SpriteEngine_Base.h
	./SEngine_Res8b_Bpp1_1G/v00.00.01/Bpp16_5R6G5B/Res8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B.h
	./SEngine_Res8b_Bpp1_1G/v00.00.01/Bpp1_1G/Res8b_Bpp1_1G_SpriteEngine_Bpp1_1G.h
	./spriteengine.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./Base
	./Bpp1_1G
	./SEngine_Res8b_Bpp1_1G/v00.00.01/Base
	./SEngine_Res8b_Bpp1_1G/v00.00.01/Bpp16_5R6G5B
	./SEngine_Res8b_Bpp1_1G/v00.00.01/Bpp1_1G
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

