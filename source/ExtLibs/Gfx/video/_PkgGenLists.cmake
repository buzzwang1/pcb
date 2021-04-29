# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "video")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./APP/audioplay.c
	./APP/recorder.c
	./APP/videoplayer.c
	./AUDIOCODEC/wav/wavplay.c
	./MJPEG/JPEG/jaricom.c
	./MJPEG/JPEG/jcapimin.c
	./MJPEG/JPEG/jcapistd.c
	./MJPEG/JPEG/jcarith.c
	./MJPEG/JPEG/jccoefct.c
	./MJPEG/JPEG/jccolor.c
	./MJPEG/JPEG/jcdctmgr.c
	./MJPEG/JPEG/jchuff.c
	./MJPEG/JPEG/jcinit.c
	./MJPEG/JPEG/jcmainct.c
	./MJPEG/JPEG/jcmarker.c
	./MJPEG/JPEG/jcmaster.c
	./MJPEG/JPEG/jcomapi.c
	./MJPEG/JPEG/jcparam.c
	./MJPEG/JPEG/jcprepct.c
	./MJPEG/JPEG/jcsample.c
	./MJPEG/JPEG/jctrans.c
	./MJPEG/JPEG/jdapimin.c
	./MJPEG/JPEG/jdapistd.c
	./MJPEG/JPEG/jdarith.c
	./MJPEG/JPEG/jdatadst.c
	./MJPEG/JPEG/jdatasrc.c
	./MJPEG/JPEG/jdcoefct.c
	./MJPEG/JPEG/jdcolor.c
	./MJPEG/JPEG/jddctmgr.c
	./MJPEG/JPEG/jdhuff.c
	./MJPEG/JPEG/jdinput.c
	./MJPEG/JPEG/jdmainct.c
	./MJPEG/JPEG/jdmarker.c
	./MJPEG/JPEG/jdmaster.c
	./MJPEG/JPEG/jdmerge.c
	./MJPEG/JPEG/jdpostct.c
	./MJPEG/JPEG/jdsample.c
	./MJPEG/JPEG/jerror.c
	./MJPEG/JPEG/jidctflt.c
	./MJPEG/JPEG/jidctfst.c
	./MJPEG/JPEG/jidctint.c
	./MJPEG/JPEG/jmemmgr.c
	./MJPEG/JPEG/jmemnobs.c
	./MJPEG/JPEG/jquant1.c
	./MJPEG/JPEG/jquant2.c
	./MJPEG/JPEG/jutils.c
	./MJPEG/JPEG/wrppm.c
	./MJPEG/avi.c
	./MJPEG/mjpeg.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./APP/audioplay.h
	./APP/recorder.h
	./APP/videoplayer.h
	./AUDIOCODEC/wav/wavplay.h
	./MJPEG/JPEG/cderror.h
	./MJPEG/JPEG/cdjpeg.h
	./MJPEG/JPEG/jconfig.h
	./MJPEG/JPEG/jdct.h
	./MJPEG/JPEG/jerror.h
	./MJPEG/JPEG/jinclude.h
	./MJPEG/JPEG/jmemsys.h
	./MJPEG/JPEG/jmorecfg.h
	./MJPEG/JPEG/jpegint.h
	./MJPEG/JPEG/jpeglib.h
	./MJPEG/JPEG/jversion.h
	./MJPEG/JPEG/transupp.h
	./MJPEG/avi.h
	./MJPEG/mjpeg.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./APP
	./AUDIOCODEC/wav
	./MJPEG/JPEG
	./MJPEG
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

