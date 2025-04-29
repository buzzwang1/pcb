# PyClass_Packages.py: This is a generated file. Do not modify it.

set(gen_pkg_name "lwip")

# List of all source files found in the packages filetree
set (gen_files 
	# List of all source files found in gen_files
	./src/api/api_lib.c
	./src/api/api_msg.c
	./src/api/err.c
	./src/api/netbuf.c
	./src/api/netdb.c
	./src/api/netifapi.c
	./src/api/sockets.c
	./src/api/tcpip.c
	./src/core/dhcp.c
	./src/core/dns.c
	./src/core/init.c
	./src/core/mem.c
	./src/core/memp.c
	./src/core/netif.c
	./src/core/pbuf.c
	./src/core/raw.c
	./src/core/snmp/asn1_dec.c
	./src/core/snmp/asn1_enc.c
	./src/core/snmp/mib2.c
	./src/core/snmp/mib_structs.c
	./src/core/snmp/msg_in.c
	./src/core/snmp/msg_out.c
	./src/core/stats.c
	./src/core/sys.c
	./src/core/tcp.c
	./src/core/tcp_in.c
	./src/core/tcp_out.c
	./src/core/udp.c
	./src/netif/etharp.c
	./src/netif/loopif.c
	./src/netif/ppp/auth.c
	./src/netif/ppp/chap.c
	./src/netif/ppp/chpms.c
	./src/netif/ppp/fsm.c
	./src/netif/ppp/ipcp.c
	./src/netif/ppp/lcp.c
	./src/netif/ppp/magic.c
	./src/netif/ppp/md5.c
	./src/netif/ppp/pap.c
	./src/netif/ppp/ppp.c
	./src/netif/ppp/ppp_oe.c
	./src/netif/ppp/randm.c
	./src/netif/ppp/vj.c
	./src/netif/slipif.c
	# List of all source files found in FreeRTOS_Ipv4_gen_files
	./_var/FreeRTOS_Ipv4/Ipv4/src/core/ipv4/autoip.c
	./_var/FreeRTOS_Ipv4/Ipv4/src/core/ipv4/icmp.c
	./_var/FreeRTOS_Ipv4/Ipv4/src/core/ipv4/igmp.c
	./_var/FreeRTOS_Ipv4/Ipv4/src/core/ipv4/inet.c
	./_var/FreeRTOS_Ipv4/Ipv4/src/core/ipv4/inet_chksum.c
	./_var/FreeRTOS_Ipv4/Ipv4/src/core/ipv4/ip.c
	./_var/FreeRTOS_Ipv4/Ipv4/src/core/ipv4/ip_addr.c
	./_var/FreeRTOS_Ipv4/Ipv4/src/core/ipv4/ip_frag.c
	./_var/FreeRTOS_Ipv4/ethernetif.c
	./_var/FreeRTOS_Ipv4/sys_arch.c
)


# List of all header files found in the packages filetree
set (gen_headers 
	# List of all source files found in gen_headers
	./port/STM32F4x7/arch/bpstruct.h
	./port/STM32F4x7/arch/cc.h
	./port/STM32F4x7/arch/cpu.h
	./port/STM32F4x7/arch/epstruct.h
	./port/STM32F4x7/arch/init.h
	./port/STM32F4x7/arch/lib.h
	./port/STM32F4x7/arch/perf.h
	./port/STM32F4x7/arch/sys_arch.h
	./port/STM32F4x7/dummy.h
	./src/include/dummy.h
	./src/include/lwip/api.h
	./src/include/lwip/api_msg.h
	./src/include/lwip/arch.h
	./src/include/lwip/debug.h
	./src/include/lwip/def.h
	./src/include/lwip/dhcp.h
	./src/include/lwip/dns.h
	./src/include/lwip/err.h
	./src/include/lwip/init.h
	./src/include/lwip/mem.h
	./src/include/lwip/memp.h
	./src/include/lwip/memp_std.h
	./src/include/lwip/netbuf.h
	./src/include/lwip/netdb.h
	./src/include/lwip/netif.h
	./src/include/lwip/netifapi.h
	./src/include/lwip/opt.h
	./src/include/lwip/pbuf.h
	./src/include/lwip/raw.h
	./src/include/lwip/sio.h
	./src/include/lwip/snmp.h
	./src/include/lwip/snmp_asn1.h
	./src/include/lwip/snmp_msg.h
	./src/include/lwip/snmp_structs.h
	./src/include/lwip/sockets.h
	./src/include/lwip/stats.h
	./src/include/lwip/sys.h
	./src/include/lwip/tcp.h
	./src/include/lwip/tcpip.h
	./src/include/lwip/udp.h
	./src/include/netif/etharp.h
	./src/include/netif/loopif.h
	./src/include/netif/ppp_oe.h
	./src/include/netif/slipif.h
	./src/netif/ppp/auth.h
	./src/netif/ppp/chap.h
	./src/netif/ppp/chpms.h
	./src/netif/ppp/fsm.h
	./src/netif/ppp/ipcp.h
	./src/netif/ppp/lcp.h
	./src/netif/ppp/magic.h
	./src/netif/ppp/md5.h
	./src/netif/ppp/pap.h
	./src/netif/ppp/ppp.h
	./src/netif/ppp/pppdebug.h
	./src/netif/ppp/randm.h
	./src/netif/ppp/vj.h
	./src/netif/ppp/vjbsdhdr.h
	# List of all source files found in FreeRTOS_Ipv4_gen_headers
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4/dummy.h
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4/lwip/autoip.h
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4/lwip/icmp.h
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4/lwip/igmp.h
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4/lwip/inet.h
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4/lwip/inet_chksum.h
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4/lwip/ip.h
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4/lwip/ip_addr.h
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4/lwip/ip_frag.h
	./_var/FreeRTOS_Ipv4/ethernetif.h
	./_var/FreeRTOS_Ipv4/sys_arch.h
)


# List of all assembler files found in the packages filetree
set (gen_asmfiles 
	# List of all source files found in gen_asmfiles
	# List of all source files found in FreeRTOS_Ipv4_gen_asmfiles
)


# List of all folders containing any header in files the packages filetree
set (gen_include_directories 
	# List of all source files found in gen_include_directories
	./port/STM32F4x7/arch
	./port/STM32F4x7
	./src/include
	./src/include/lwip
	./src/include/netif
	./src/netif/ppp
	# List of all source files found in FreeRTOS_Ipv4_gen_include_directories
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4
	./_var/FreeRTOS_Ipv4/Ipv4/src/include/ipv4/lwip
	./_var/FreeRTOS_Ipv4
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

