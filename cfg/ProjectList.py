#include the configurations
from ProjectConfig_Stm32 import   configs_Stm32
from ProjectConfig_PcMsWin import configs_PcWin

#Set the configurations
configs = configs_Stm32
configs.update(configs_PcWin)


# label: Is shown on the selection list to select the project
# name: Is used as name for the project, e.g. to create build folder, target name, should be identic
# enable: Is used to turn on/off the project
# mode: Reserved for future use
# configs: Used configurations (could be more than one) for the project. List of selectable configurations is found the the end of this file.
project = lambda name, enable, mode, configs: {'name':name, 'enable':enable, 'mode':mode, 'configs':configs}
projects = {
    #label                                      name                                  enable    mode   configs
    # --- STM32F103RBT/BluePill ------------------------------------------------
    'STM/F1/STM32F103C8T/BluePill/LedBlink/1s'    : project('STM32F103C8T_LedBlink1s',        True,      '',   {'Debug' : 'Config_STM32F103xx'}),
    'STM/F1/STM32F103C8T/BluePill/LedBlink/100ms' : project('STM32F103C8T_LedBlink100ms',     True,      '',   {'Debug' : 'Config_STM32F103xx'}),
    'STM/F1/STM32F103C8T/BluePill/SM4PWM/Test01'  : project('STM32F103C8T_SW4PWM_Test01',     True,      '',   {'Debug' : 'Config_STM32F103xx'}),

    'STM/F1/STM32F103C8T/SolBx/02/MotBx/01'       : project('STM32F103C8T_SolBx_02_MotBx_01', True,      '',   {'Debug' : 'Config_STM32F103xx'}),
    'STM/F1/STM32F103C8T/SolBx/02/LightBx/01'     : project('STM32F103C8T_SolBx_02_LightBx_01', True,    '',   {'Debug' : 'Config_STM32F103xx'}),
    
    # --- STM32F303VC/Discovery -----------------------------------------------

    'STM/F3/STM32F303VC/STM32F3DISCOVERY/LedBlink/PE15'     : project('STM32F303CCT_LedBlink_PE15',   True,  '', {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303VC/STM32F3DISCOVERY/Firmware'          : project('STM32F303CCT_Disc_Firmware',   True,  '', {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303VC/STM32F3DISCOVERY/Firmware2'         : project('STM32F303CCT_Disc_Firmware2',  True,  '', {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303VC/STM32F3DISCOVERY/FirmwareBl'        : project('STM32F303CCT_Disc_FirmwareBl', True,  '', {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303VC/STM32F3DISCOVERY/Bootloader'        : project('STM32F303CCT_Disc_Bootloader', True,  '', {'Debug' : 'Config_STM32F303', 'Release' : 'Config_STM32F303'}),

    # --- STM32F303CCT/BluePill -----------------------------------------------

    'STM/F3/STM32F303CCT/BluePill/Test/Minimal'          : project('STM32F303CCT_Minimal',            True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/Test/Cpp01'            : project('STM32F303CCT_Tests_Cpp01',        True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/Test/Loop4Mhz'         : project('STM32F303CCT_Tests_Loop4Mhz',     True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/Test/Loop8Mhz'         : project('STM32F303CCT_Tests_Loop8Mhz',     True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/Test/Benchmark'        : project('STM32F303CCT_Tests_Benchmark',    True,     '',   {'Debug' : 'Config_STM32F303', 'Release' : 'Config_STM32F303'}),    

    'STM/F3/STM32F303CCT/BluePill/LedBlink/PC13'         : project('STM32F303CCT_LedBlink_PC13',      True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/LedBlink/PC13_4Mhz'    : project('STM32F303CCT_LedBlink_PC13_4Mhz', True,     '',   {'Debug' : 'Config_STM32F303', 'Release' : 'Config_STM32F303'}),

    'STM/F3/STM32F303CCT/BluePill/BotNet/02'             : project('STM32F303CCT_BotNet_02',          True,     '',   {'Debug' : 'Config_STM32F303', 'Debug_App' : 'Config_STM32F303_BlApp'}),
    'STM/F3/STM32F303CCT/BluePill/BotNet/03'             : project('STM32F303CCT_BotNet_03',          True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/BotNet/PcIfc'          : project('STM32F303CCT_BotNet_PcIfc',       True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/BotNet/PcIfcNet'       : project('STM32F303CCT_BotNet_PcIfcNet',    True,     '',   {'Debug' : 'Config_STM32F303'}),

    'STM/F3/STM32F303CCT/BluePill/TB01/Gfx/HX8347'       : project('STM32F303CCT_TB01_Gfx_HX8347',    True,     '',    {'Debug' : 'Config_STM32F303'}),

    'STM/F3/STM32F303CCT/BluePill/LP/Test01'             : project('STM32F303CCT_LP_Test01', True, '', {'Debug' : 'Config_STM32F303'}),

    'STM/F3/STM32F303CCT/BluePill/I2C/APDS9960/Als'      : project('STM32F303CCT_I2C_APDS9960_Als',      True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/I2C/APDS9960/Ds'       : project('STM32F303CCT_I2C_APDS9960_Ds',       True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/I2C/APDS9960/Gs'       : project('STM32F303CCT_I2C_APDS9960_Gs',       True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/I2C/APDS9960UartNoGfx' : project('STM32F303CCT_I2C_APDS9960UartNoGfx', True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/I2C/HTU21D'            : project('STM32F303CCT_I2C_HTU21D',            True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/I2C/INA219'            : project('STM32F303CCT_I2C_INA219',            True,     '',   {'Debug' : 'Config_STM32F303'}),
    'STM/F3/STM32F303CCT/BluePill/I2C/OLED'              : project('STM32F303CCT_I2C_OLED',              True,     '',   {'Debug' : 'Config_STM32F303'}),

    #--- STM32F407VGT/Blackboard ----------------------------------------------

    'STM/F4/STM32F407ZET/STF4BB/Ex/02_WaterLED'    : project('STM32F407ZET_STF4BB_Ex_WaterLed',    True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/03_Key'         : project('STM32F407ZET_STF4BB_Ex_Key',         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/04_Uart'        : project('STM32F407ZET_STF4BB_Ex_Uart',        True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/05_Int'         : project('STM32F407ZET_STF4BB_Ex_Int',         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/06_Watchdog'    : project('STM32F407ZET_STF4BB_Ex_Wdg',         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/07_WinWatchdog' : project('STM32F407ZET_STF4BB_Ex_WWdg',        True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/08_Timer'       : project('STM32F407ZET_STF4BB_Ex_Timer',       True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/09_Pwm'         : project('STM32F407ZET_STF4BB_Ex_Pwm',         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/10_Input'       : project('STM32F407ZET_STF4BB_Ex_Input',       True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/11_TFTLCD'      : project('STM32F407ZET_STF4BB_Ex_TFTLCD',      True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/12_USMART'      : project('STM32F407ZET_STF4BB_Ex_USMART',      True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/13_Rtc'         : project('STM32F407ZET_STF4BB_Ex_Rtc',         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/14_RandomGen'   : project('STM32F407ZET_STF4BB_Ex_RandomGen',   True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/15_Adc'         : project('STM32F407ZET_STF4BB_Ex_Adc',         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/16_Temperature' : project('STM32F407ZET_STF4BB_Ex_Temperature', True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/17_Dac'         : project('STM32F407ZET_STF4BB_Ex_Dac',         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/18_PwmDac'      : project('STM32F407ZET_STF4BB_Ex_PwmDac',      True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/19_Dma'         : project('STM32F407ZET_STF4BB_Ex_Dma',         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/20_Spi'         : project('STM32F407ZET_STF4BB_Ex_Spi',         True,     '',   {'Debug' : 'Config_STM32F407x'}),

    'STM/F4/STM32F407ZET/STF4BB/Ex/21_TouchPanel'  : project('STM32F407ZET_STF4BB_Ex_TouchPanel',  True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/22_NRF24L01'    : project('STM32F407ZET_STF4BB_Ex_NRF24L01',    True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/23_FLASH2EEPROM': project('STM32F407ZET_STF4BB_Ex_FLASH2EEPROM',True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/24_Sram'        : project('STM32F407ZET_STF4BB_Ex_Sram',        True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/25_Memory'      : project('STM32F407ZET_STF4BB_Ex_Memory',      True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/26_SD'          : project('STM32F407ZET_STF4BB_Ex_SD',          True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/27_FATFS'       : project('STM32F407ZET_STF4BB_Ex_FATFS',       True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/29_IMAGE'       : project('STM32F407ZET_STF4BB_Ex_IMAGE',       True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/30_AVPlay'      : project('STM32F407ZET_STF4BB_Ex_AVPlay',      True,     '',   {'Debug' : 'Config_STM32F407x'}),

    'STM/F4/STM32F407ZET/STF4BB/Ex/31_Julia_CPU'               : project('STM32F407ZET_STF4BB_Ex_Julia_CPU',               True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/31_Julia_FPU'               : project('STM32F407ZET_STF4BB_Ex_Julia_FPU',               True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/32_DSP_BasicMath'           : project('STM32F407ZET_STF4BB_Ex_DSP_BasicMath',           True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/32_DSP_FFT'                 : project('STM32F407ZET_STF4BB_Ex_DSP_FFT',                 True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/33_HandWriter'              : project('STM32F407ZET_STF4BB_Ex_HandWriter',              False,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/35_USBCardReader_Slave'     : project('STM32F407ZET_STF4BB_Ex_USBCardReader_Slave',     True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/37_UCOSII_EventCall'        : project('STM32F407ZET_STF4BB_Ex_UCOSII_EventCall',        True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/38_UCOSII_EventOther'       : project('STM32F407ZET_STF4BB_Ex_UCOSII_EventOther',       True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Ex/39_UCOSII_SemaphoreMailbox' : project('STM32F407ZET_STF4BB_Ex_UCOSII_SemaphoreMailbox', True,     '',   {'Debug' : 'Config_STM32F407x'}),


    'STM/F4/STM32F407ZET/STF4BB/Gfx/v1/Test01'    : project('STM32F407ZET_STF4BB_Gfx_v1_Test01',    True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/v1/Test02'    : project('STM32F407ZET_STF4BB_Gfx_v1_Test02',    True,     '',   {'Debug' : 'Config_STM32F407x', 'Release' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/v1/Test03'    : project('STM32F407ZET_STF4BB_Gfx_v1_Test03',    True,     '',   {'Debug' : 'Config_STM32F407x'}),	
    'STM/F4/STM32F407ZET/STF4BB/Gfx/v1/Test03_APDS9960'    : project('STM32F407ZET_STF4BB_Gfx_v1_Test03_APDS9960',    True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/v1/Test03_LSM303'      : project('STM32F407ZET_STF4BB_Gfx_v1_Test03_LSM303',      True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/v1/Test04'    : project('STM32F407ZET_STF4BB_Gfx_v1_Test04',    True,     '',   {'Debug' : 'Config_STM32F407x', 'Release' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/v2/Test05'    : project('STM32F407ZET_STF4BB_Gfx_v2_Test05',    True,     '',   {'Debug' : 'Config_STM32F407x', 'Release' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/Misc/MemTest' : project('STM32F407ZET_STF4BB_Gfx_Misc_MemTest', True,     '',   {'Debug' : 'Config_STM32F407x', 'Release' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Misc/ConstTest'   : project('STM32F407ZET_STF4BB_Misc_ConstTest',   True,     '',   {'Debug' : 'Config_STM32F407x', 'Release' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/I2C/APDS9960'     : project('STM32F407ZET_STF4BB_I2C_APDS9960',     True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/I2C/OLED'         : project('STM32F407ZET_STF4BB_I2C_OLED',         True,     '',   {'Debug' : 'Config_STM32F407x'}),

    #--- STM32F407VGT/F4DISC --------------------------------------------------

    'STM/F4/STM32F407VGT/F4DISC/STF4BB/Eth/FreeRTOS/HttpSrvNtc'              : project('STM32F4DISC_BB_Eth_FreeRTOS_HttpSrvNtc',         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/Eth/FreeRTOS/HttpSrvSocket'           : project('STM32F4DISC_BB_Eth_FreeRTOS_HttpSrvSocket',      True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/Eth/FreeRTOS/UdpTcpEchoSrvNtc'        : project('STM32F4DISC_BB_Eth_FreeRTOS_UdpTcpEchoSrvNtc',   True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/Eth/Standalone/HttpServer'            : project('STM32F4DISC_BB_Eth_Standalone_HttpServer',       True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/Eth/Standalone/TcpEchoClient'         : project('STM32F4DISC_BB_Eth_Standalone_TcpEchoClient',    True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/Eth/Standalone/TcpEchoServer'         : project('STM32F4DISC_BB_Eth_Standalone_TcpEchoServer',    True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/Eth/Standalone/UdpEchoClient'         : project('STM32F4DISC_BB_Eth_Standalone_UdpEchoClient',    True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/Eth/Standalone/UdpEchoServer'         : project('STM32F4DISC_BB_Eth_Standalone_UdpEchoServer',    True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/uCOSII'                               : project('STM32F4DISC_BB_uCOSII',                               False,    '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/SDIO/uSDCard'                         : project('STM32F4DISC_BB_SDIO_uSDCard',                         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/SDIO/FatFs'                           : project('STM32F4DISC_BB_SDIO_FatFs',                           True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/USART/Printf'                         : project('STM32F4DISC_BB_USART_Printf',                         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407VGT/F4DISC/STF4BB/LCD/LCD_35T'                          : project('STM32F4DISC_BB_LCD_LCD35T',                           True,     '',   {'Debug' : 'Config_STM32F407x'}),

    'STM/F4/STM32F407VGT/F4DISC/Tests/Sleep/01'                              : project('STM32F4DISC_Tests_Sleep_01',                          True,     '',   {'Debug' : 'Config_STM32F407x'}),

    #--- STM32F413ZHT/STM32F413HDISCOVERY -------------------------------------

    'STM/F4/STM32F413ZHT/F413ZHTDISCO/LedBlink/PC5'                          : project('STM32F413ZHT_LedBlink_PC5',                           True,     '',   {'Debug' : 'Config_STM32F413x'}),

    #--- STM32F429ZIT6/STM32F429IDISCOVERY -------------------------------------
    'STM/F4/STM32F429ZIT6/F429IDISCO/LedBlink/PG13'          : project('STM32F429ZIT6_LedBlink_PG13',       True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gui/v1/SPI/Test01'      : project('STM32F429ZIT6_Gui_v1_SPI_Test01',   True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gui/v1/LCD/Test01'      : project('STM32F429ZIT6_Gui_v1_LCD_Test01',   True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gui/v1/LCD/Test02'      : project('STM32F429ZIT6_Gui_v1_LCD_Test02',   True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gui/v1/LCD/Test03'      : project('STM32F429ZIT6_Gui_v1_LCD_Test03',   True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gui/v2/LCD/Test05'      : project('STM32F429ZIT6_Gui_v2_LCD_Test05',   True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Misc/Dcmi/OV7670/Test1' : project('STM32F429ZIT6_Misc_OV7670_Test1',   True,     '',   {'Debug' : 'Config_STM32F429x'}),

    'STM/F4/STM32F429ZIT6/F429IDISCO/LCD/ILI9341/Test3'      : project('STM32F429ZIT6_ILI9341_LCD_Test3',   True,     '',   {'Debug' : 'Config_STM32F429x'}),

    'STM/F4/STM32F429ZIT6/F429IDISCO/Ex/01/10_MiniOszi'      : project('STM32F429ZIT6_10_MiniOszi',   True,     '',   {'Debug' : 'Config_STM32F429x'}),

    # --- STM32L4 -----------------------------------------------
    'STM/L4/HalLib/v01.15.01'                          : project('STM32L4_HalLib',                     True,     '', {'og' : 'Config_STM32L4x3', 'o3' : 'Config_STM32L4x3', 'os'  : 'Config_STM32L4x3'}),
    'STM/L4/HalLib/v01.17.00'                          : project('STM32L4_HalLib',                     True,     '', {'og' : 'Config_STM32L4R9', 'o3' : 'Config_STM32L4R9', 'os'  : 'Config_STM32L4R9'}),

    # --- STM32L433CCT/SolBx -----------------------------------------------
    'STM/L4/STM32L433CCT/SolBx/01/App'                 : project('STM32L433CCT_SolBx_01_App',          True,     '', {'Debug' : 'Config_STM32L4x3', 'Release' : 'Config_STM32L4x3', 'Debug_App'  : 'Config_STM32L4x3_BlApp', 'Release_App' : 'Config_STM32L4x3_BlApp'}),
    'STM/L4/STM32L433CCT/SolBx/01/Blu'                 : project('STM32L433CCT_SolBx_01_Blu',          True,     '', {'Debug' : 'Config_STM32L4x3', 'Release' : 'Config_STM32L4x3', 'RamRelease' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/SolBx/02/Master/01'           : project('STM32L433CCT_SolBx_02_Master_01',    True,     '',   {'Debug' : 'Config_STM32L4x3', 'Debug_App' : 'Config_STM32L4x3_BlApp'}),
    'STM/L4/STM32L433CCT/SolBx/02/MotBx/01'            : project('STM32L433CCT_SolBx_02_MotBx_01',     True,     '',   {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/SolBx/02/LightBx/01'          : project('STM32L433CCT_SolBx_02_LightBx_01',   True,     '',   {'Debug' : 'Config_STM32L4x3'}),

    #--- STM32L433CCT/BluePill --------------------------------------------------

    'STM/L4/STM32L433CCT/BluePill/Bootloader'          : project('STM32L433CCT_Bootloader',             True,      '', {'Debug' : 'Config_STM32L4x3', 'Release' : 'Config_STM32L4x3'}),
     
    'STM/L4/STM32L433CCT/BluePill/BotNet/02'           : project('STM32L433CCT_BotNet_02',              True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/BotNet/03'           : project('STM32L433CCT_BotNet_03',              True,      '', {'Debug' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/BluePill/I2C/APDS9960/Als'    : project('STM32L433CCT_I2C_APDS9960_Als',       True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/I2C/APDS9960/Ds'     : project('STM32L433CCT_I2C_APDS9960_Ds',        True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/I2C/APDS9960/Gs'     : project('STM32L433CCT_I2C_APDS9960_Gs',        True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/I2C/HTU21D'          : project('STM32L433CCT_I2C_HTU21D',             True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/I2C/INA219'          : project('STM32L433CCT_I2C_INA219',             True,      '', {'Debug' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/BluePill/I2C/OLED'            : project('STM32L433CCT_I2C_OLED',               True,      '', {'Debug' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/BluePill/Mot/Test01'          : project('STM32L433CCT_Mot_Test01',             True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/LedBlink/PC13'       : project('STM32L433CCT_LedBlink_PC13',          True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/LedBlink/All'        : project('STM32L433CCT_LedBlink_All',           True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/Usart12'        : project('STM32L433CCT_Tests_Usart12',          True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/Cli'            : project('STM32L433CCT_Tests_Cli',              True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/PowTest'        : project('STM32L433CCT_Tests_PowTest',          True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/PowTestRtc'     : project('STM32L433CCT_Tests_PowTestRtc',       True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/Loop8Mhz'       : project('STM32L433CCT_Tests_Loop8Mhz',         True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Gfx/ST7735'          : project('STM32L433CCT_Gfx_ST7735',             True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/PwrBx'               : project('STM32L433CCT_PwrBx',                  True,      '', {'Debug' : 'Config_STM32L4x3'}),

    #--- STM32L4R9/STM32L4R9IDISCOVERY -------------------------------------

    'STM/L4/STM32L4R9/L4R9AIDISCO/LedBlink/PB13'       : project('STM32L4R9_LedBlink_PB13',             True,     '',   {'Debug' : 'Config_STM32L4R9'}),

    
    #--- STM32L552ZET/NUCLEO --------------------------------------------------
    'STM/L5/STM32L552ZET/L552_Nucleo/LedBlink/PB7'     : project('STM32L552ZET_LedBlink_PB7',           True,      '', {'Debug' : 'Config_STM32L552'}),

 
    #--- Pc Simulation ----------------------------------------------

    #--- STM32F407VGT/Blackboard ----------------------------------------------

    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test01'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v1_Test01',    True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test02'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v1_Test02',    True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test03'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v1_Test03',    True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test04'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_Gui_v1_Test04',    True,     '',   {'Debug' : 'Config_PcMsWin'}),

    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test01'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v2_Test01',   True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test02'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v2_Test02',   True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test03'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v2_Test03',   True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test04'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v2_Test04',   True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test05'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v2_Test05',   True,     '',   {'Debug' : 'Config_PcMsWin'}),

    #--- STM32L433CCT/BluePill --------------------------------------------------

    'PcSim/L4/STM32L433CCT/BluePill/Bn/Test01'            : project('PcSim_STM32L433CCT_Bn_Test01',              True,  '', {'Debug' : 'Config_PcMsWin'}),
    'PcSim/L4/STM32L433CCT/BluePill/Bootloader/Test01'    : project('PcSim_STM32L433CCT_Bootloader_Test01',      True,  '', {'Debug' : 'Config_PcMsWinConsole'}),

    #--- STM32L552ZET/NUCLEO --------------------------------------------------

    'PcSim/L5/STM32L552ZET/L552_Nucleo/Safety/Test01'    : project('PcSim_STM32L552ZET_L552_Nucleo_Safety_Test01',    True,     '',   {'Debug' : 'Config_PcMsWinConsole'}),
}



#Test output
if __name__ == "__main__":
    import os
    import sys

    # Normally, this file is located in [dir_source]/projects
    # Load the scripts from [dir_source]/scripts/python
    dir_root = os.path.dirname(os.path.realpath(os.path.join(__file__, "..")))
    dir_source = os.path.dirname(os.path.realpath(os.path.join(dir_root, "source")))
    sys.path.insert(0, dir_root + "/scripts/pcb/py/core")

    from PyClass_Projects import tcProject
    from PyClass_Projects import tcProjects

    lcProjects = tcProjects(projects)
    print(lcProjects.toString())

    lLstProjects = lcProjects.GetListMatchedProjects("*.*")

    for lszProjectLabel in lLstProjects:
        lcProject = lcProjects.LoadProject(lszProjectLabel, configs)
        if (lcProject != None):
            # Example: script replaced parameter
            lcProject['system/dir/root']   = dir_root
            lcProject['system/dir/source'] = dir_source
            print(lcProject.GetFullLabel() + "'s config:\n" + lcProject.toStringConfig() + "\n\n")
