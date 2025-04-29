# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "nfc_ndef")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./conn_hand_parser/ac_rec_parser/nfc_ac_rec_parser.c
	./conn_hand_parser/ble_oob_advdata_parser/nfc_ble_oob_advdata_parser.c
	./conn_hand_parser/le_oob_rec_parser/nfc_le_oob_rec_parser.c
	./connection_handover/ac_rec/nfc_ac_rec.c
	./connection_handover/ble_oob_advdata/nfc_ble_oob_advdata.c
	./connection_handover/ble_pair_lib/nfc_ble_pair_lib.c
	./connection_handover/ble_pair_msg/nfc_ble_pair_msg.c
	./connection_handover/common/nfc_ble_pair_common.c
	./connection_handover/ep_oob_rec/nfc_ep_oob_rec.c
	./connection_handover/hs_rec/nfc_hs_rec.c
	./connection_handover/le_oob_rec/nfc_le_oob_rec.c
	./generic/message/nfc_ndef_msg.c
	./generic/record/nfc_ndef_record.c
	./launchapp/nfc_launchapp_msg.c
	./launchapp/nfc_launchapp_rec.c
	./parser/message/nfc_ndef_msg_parser.c
	./parser/message/nfc_ndef_msg_parser_local.c
	./parser/record/nfc_ndef_record_parser.c
	./text/nfc_text_rec.c
	./uri/nfc_uri_msg.c
	./uri/nfc_uri_rec.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./conn_hand_parser/ac_rec_parser/nfc_ac_rec_parser.h
	./conn_hand_parser/ble_oob_advdata_parser/nfc_ble_oob_advdata_parser.h
	./conn_hand_parser/le_oob_rec_parser/nfc_le_oob_rec_parser.h
	./connection_handover/ac_rec/nfc_ac_rec.h
	./connection_handover/ble_oob_advdata/nfc_ble_oob_advdata.h
	./connection_handover/ble_pair_lib/nfc_ble_pair_lib.h
	./connection_handover/ble_pair_msg/nfc_ble_pair_msg.h
	./connection_handover/common/nfc_ble_pair_common.h
	./connection_handover/ep_oob_rec/nfc_ep_oob_rec.h
	./connection_handover/hs_rec/nfc_hs_rec.h
	./connection_handover/le_oob_rec/nfc_le_oob_rec.h
	./generic/message/nfc_ndef_msg.h
	./generic/record/nfc_ndef_record.h
	./launchapp/nfc_launchapp_msg.h
	./launchapp/nfc_launchapp_rec.h
	./parser/message/nfc_ndef_msg_parser.h
	./parser/message/nfc_ndef_msg_parser_local.h
	./parser/record/nfc_ndef_record_parser.h
	./text/nfc_text_rec.h
	./uri/nfc_uri_msg.h
	./uri/nfc_uri_rec.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./conn_hand_parser/ac_rec_parser
	./conn_hand_parser/ble_oob_advdata_parser
	./conn_hand_parser/le_oob_rec_parser
	./connection_handover/ac_rec
	./connection_handover/ble_oob_advdata
	./connection_handover/ble_pair_lib
	./connection_handover/ble_pair_msg
	./connection_handover/common
	./connection_handover/ep_oob_rec
	./connection_handover/hs_rec
	./connection_handover/le_oob_rec
	./generic/message
	./generic/record
	./launchapp
	./parser/message
	./parser/record
	./text
	./uri
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

