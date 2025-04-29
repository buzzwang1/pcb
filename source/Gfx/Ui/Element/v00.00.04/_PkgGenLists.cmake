# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "gfxelement")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./Base/uielement_base.cpp
	./Box3D/uielement_box3d.cpp
	./Box3D/uielement_box3dobj.cpp
	./Box3Df/uielement_box3df.cpp
	./Box3Df/uielement_box3df_face.cpp
	./Box3Df/uielement_box3df_obj.cpp
	./Box3Df/uielement_box3df_point.cpp
	./Box3Df/uielement_box3df_viewer_base.cpp
	./Box3Df/uielement_box3df_viewer_basicz.cpp
	./Box3Df/uielement_box3df_viewer_flat.cpp
	./Box3Df/uielement_box3df_viewer_txgradz.cpp
	./Box3Df/uielement_box3df_world.cpp
	./Button/uielement_button.cpp
	./Root/uielement_root.cpp
	./Split/uielement_split.cpp
	./Switch/uielement_switch.cpp
	./TextBox/uielement_textbox.cpp
	./Window/uielement_window.cpp
	./fGraphYBox/uielement_fgraphybox.cpp
	./iGraphXYBox/uielement_igraphxybox.cpp
	./iGraphYBox/uielement_igraphybox.cpp
	./uielement.cpp
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./Base/uielement_base.h
	./Box3D/uielement_box3d.h
	./Box3D/uielement_box3dobj.h
	./Box3Df/uielement_box3df.h
	./Box3Df/uielement_box3df_face.h
	./Box3Df/uielement_box3df_obj.h
	./Box3Df/uielement_box3df_point.h
	./Box3Df/uielement_box3df_viewer_base.h
	./Box3Df/uielement_box3df_viewer_basicz.h
	./Box3Df/uielement_box3df_viewer_flat.h
	./Box3Df/uielement_box3df_viewer_txgradz.h
	./Box3Df/uielement_box3df_world.h
	./Button/uielement_button.h
	./Root/uielement_root.h
	./Split/uielement_split.h
	./Switch/uielement_switch.h
	./TextBox/uielement_textbox.h
	./Window/uielement_window.h
	./fGraphYBox/uielement_fgraphybox.h
	./iGraphXYBox/uielement_igraphxybox.h
	./iGraphYBox/uielement_igraphybox.h
	./uielement.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./Base
	./Box3D
	./Box3Df
	./Button
	./Root
	./Split
	./Switch
	./TextBox
	./Window
	./fGraphYBox
	./iGraphXYBox
	./iGraphYBox
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

