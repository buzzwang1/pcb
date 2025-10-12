# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "tinyusb_src")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./class/audio/audio_device.c
	./class/bth/bth_device.c
	./class/cdc/cdc_device.c
	./class/cdc/cdc_host.c
	./class/cdc/cdc_rndis_host.c
	./class/dfu/dfu_device.c
	./class/dfu/dfu_rt_device.c
	./class/hid/hid_device.c
	./class/hid/hid_host.c
	./class/midi/midi_device.c
	./class/midi/midi_host.c
	./class/msc/msc_device.c
	./class/msc/msc_host.c
	./class/net/ecm_rndis_device.c
	./class/net/ncm_device.c
	./class/usbtmc/usbtmc_device.c
	./class/vendor/vendor_device.c
	./class/vendor/vendor_host.c
	./class/video/video_device.c
	./common/tusb_fifo.c
	./device/usbd.c
	./device/usbd_control.c
	./host/hub.c
	./host/usbh.c
	./portable/analog/max3421/hcd_max3421.c
	./portable/bridgetek/ft9xx/dcd_ft9xx.c
	./portable/chipidea/ci_fs/dcd_ci_fs.c
	./portable/chipidea/ci_hs/dcd_ci_hs.c
	./portable/chipidea/ci_hs/hcd_ci_hs.c
	./portable/dialog/da146xx/dcd_da146xx.c
	./portable/ehci/ehci.c
	./portable/espressif/esp32sx/dcd_esp32sx.c
	./portable/mentor/musb/dcd_musb.c
	./portable/mentor/musb/hcd_musb.c
	./portable/microchip/pic32mz/dcd_pic32mz.c
	./portable/microchip/pic/dcd_pic.c
	./portable/microchip/samd/dcd_samd.c
	./portable/microchip/samg/dcd_samg.c
	./portable/microchip/samx7x/dcd_samx7x.c
	./portable/mindmotion/mm32/dcd_mm32f327x_otg.c
	./portable/nordic/nrf5x/dcd_nrf5x.c
	./portable/nuvoton/nuc120/dcd_nuc120.c
	./portable/nuvoton/nuc121/dcd_nuc121.c
	./portable/nuvoton/nuc505/dcd_nuc505.c
	./portable/nxp/khci/dcd_khci.c
	./portable/nxp/khci/hcd_khci.c
	./portable/nxp/lpc17_40/dcd_lpc17_40.c
	./portable/nxp/lpc17_40/hcd_lpc17_40.c
	./portable/nxp/lpc_ip3511/dcd_lpc_ip3511.c
	./portable/ohci/ohci.c
	./portable/raspberrypi/pio_usb/dcd_pio_usb.c
	./portable/raspberrypi/pio_usb/hcd_pio_usb.c
	./portable/raspberrypi/rp2040/dcd_rp2040.c
	./portable/raspberrypi/rp2040/hcd_rp2040.c
	./portable/raspberrypi/rp2040/rp2040_usb.c
	./portable/renesas/rusb2/dcd_rusb2.c
	./portable/renesas/rusb2/hcd_rusb2.c
	./portable/renesas/rusb2/rusb2_common.c
	./portable/sony/cxd56/dcd_cxd56.c
	./portable/st/stm32_fsdev/dcd_stm32_fsdev.c
	./portable/st/typec/typec_stm32.c
	./portable/sunxi/dcd_sunxi_musb.c
	./portable/synopsys/dwc2/dcd_dwc2.c
	./portable/synopsys/dwc2/dwc2_common.c
	./portable/synopsys/dwc2/hcd_dwc2.c
	./portable/template/dcd_template.c
	./portable/template/hcd_template.c
	./portable/ti/msp430x5xx/dcd_msp430x5xx.c
	./portable/valentyusb/eptri/dcd_eptri.c
	./portable/wch/dcd_ch32_usbfs.c
	./portable/wch/dcd_ch32_usbhs.c
	./tusb.c
	./typec/usbc.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./class/audio/audio.h
	./class/audio/audio_device.h
	./class/bth/bth_device.h
	./class/cdc/cdc.h
	./class/cdc/cdc_device.h
	./class/cdc/cdc_host.h
	./class/cdc/cdc_rndis.h
	./class/cdc/cdc_rndis_host.h
	./class/cdc/serial/ch34x.h
	./class/cdc/serial/cp210x.h
	./class/cdc/serial/ftdi_sio.h
	./class/dfu/dfu.h
	./class/dfu/dfu_device.h
	./class/dfu/dfu_rt_device.h
	./class/hid/hid.h
	./class/hid/hid_device.h
	./class/hid/hid_host.h
	./class/midi/midi.h
	./class/midi/midi_device.h
	./class/midi/midi_host.h
	./class/msc/msc.h
	./class/msc/msc_device.h
	./class/msc/msc_host.h
	./class/net/ncm.h
	./class/net/net_device.h
	./class/usbtmc/usbtmc.h
	./class/usbtmc/usbtmc_device.h
	./class/vendor/vendor_device.h
	./class/vendor/vendor_host.h
	./class/video/video.h
	./class/video/video_device.h
	./common/tusb_common.h
	./common/tusb_compiler.h
	./common/tusb_debug.h
	./common/tusb_fifo.h
	./common/tusb_mcu.h
	./common/tusb_private.h
	./common/tusb_types.h
	./common/tusb_verify.h
	./device/dcd.h
	./device/usbd.h
	./device/usbd_pvt.h
	./host/hcd.h
	./host/hub.h
	./host/usbh.h
	./host/usbh_pvt.h
	./osal/osal.h
	./osal/osal_freertos.h
	./osal/osal_mynewt.h
	./osal/osal_none.h
	./osal/osal_pico.h
	./osal/osal_rtthread.h
	./osal/osal_rtx4.h
	./osal/osal_zephyr.h
	./portable/analog/max3421/hcd_max3421.h
	./portable/chipidea/ci_fs/ci_fs_kinetis.h
	./portable/chipidea/ci_fs/ci_fs_mcx.h
	./portable/chipidea/ci_fs/ci_fs_type.h
	./portable/chipidea/ci_hs/ci_hs_imxrt.h
	./portable/chipidea/ci_hs/ci_hs_lpc18_43.h
	./portable/chipidea/ci_hs/ci_hs_mcx.h
	./portable/chipidea/ci_hs/ci_hs_type.h
	./portable/ehci/ehci.h
	./portable/ehci/ehci_api.h
	./portable/mentor/musb/musb_max32.h
	./portable/mentor/musb/musb_ti.h
	./portable/mentor/musb/musb_type.h
	./portable/microchip/pic32mz/usbhs_registers.h
	./portable/microchip/samx7x/common_usb_regs.h
	./portable/nxp/lpc17_40/dcd_lpc17_40.h
	./portable/ohci/ohci.h
	./portable/raspberrypi/rp2040/rp2040_usb.h
	./portable/renesas/rusb2/rusb2_ra.h
	./portable/renesas/rusb2/rusb2_rx.h
	./portable/renesas/rusb2/rusb2_type.h
	./portable/st/stm32_fsdev/fsdev_ch32.h
	./portable/st/stm32_fsdev/fsdev_stm32.h
	./portable/st/stm32_fsdev/fsdev_type.h
	./portable/sunxi/musb_def.h
	./portable/synopsys/dwc2/dwc2_bcm.h
	./portable/synopsys/dwc2/dwc2_common.h
	./portable/synopsys/dwc2/dwc2_efm32.h
	./portable/synopsys/dwc2/dwc2_esp32.h
	./portable/synopsys/dwc2/dwc2_gd32.h
	./portable/synopsys/dwc2/dwc2_stm32.h
	./portable/synopsys/dwc2/dwc2_type.h
	./portable/synopsys/dwc2/dwc2_xmc.h
	./portable/valentyusb/eptri/dcd_eptri.h
	./portable/wch/ch32_usbfs_reg.h
	./portable/wch/ch32_usbhs_reg.h
	./tusb.h
	./tusb_option.h
	./typec/pd_types.h
	./typec/tcd.h
	./typec/usbc.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./class/audio
	./class/bth
	./class/cdc
	./class/cdc/serial
	./class/dfu
	./class/hid
	./class/midi
	./class/msc
	./class/net
	./class/usbtmc
	./class/vendor
	./class/video
	./common
	./device
	./host
	./osal
	./portable/analog/max3421
	./portable/chipidea/ci_fs
	./portable/chipidea/ci_hs
	./portable/ehci
	./portable/mentor/musb
	./portable/microchip/pic32mz
	./portable/microchip/samx7x
	./portable/nxp/lpc17_40
	./portable/ohci
	./portable/raspberrypi/rp2040
	./portable/renesas/rusb2
	./portable/st/stm32_fsdev
	./portable/sunxi
	./portable/synopsys/dwc2
	./portable/valentyusb/eptri
	./portable/wch
	.
	./typec
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

