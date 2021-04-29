cmake_policy(SET CMP0079 NEW)
target_link_libraries(usmart_lib PUBLIC fatfs_lib)
target_link_libraries(fatfs_lib PUBLIC sdio_lib)
target_link_libraries(main_lib m)
