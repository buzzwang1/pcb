#include the configurations
from ProjectConfig_Atmel   import configs_Atmel
from ProjectConfig_Stm32   import configs_Stm32
from ProjectConfig_nRf     import configs_nRf
from ProjectConfig_Ch32    import configs_Ch32
from ProjectConfig_PcMsWin import configs_PcWin

#Set the configurations
configs = configs_Stm32
configs.update(configs_Atmel)
configs.update(configs_nRf)
configs.update(configs_Ch32)
configs.update(configs_PcWin)


# label: Is shown on the selection list to select the project
# name: Is used as name for the project, e.g. to create build folder, target name, should be identic
# enable: Is used to turn on/off the project
# mode: Reserved for future use
# configs: Used configurations (could be more than one) for the project. List of selectable configurations is found the the end of this file.
project = lambda name, enable, mode, configs: {'name':name, 'enable':enable, 'mode':mode, 'configs':configs}
projects = {
    #label                                      name                                  enable    mode   configs

    # --- CH641 ------------------------------------------------
    'CH/CH641/Servo/App'                         : project('CH641_Servo_App',                         True,      '',   {'Release' : 'Config_CH641'}),
    'CH/CH641/Servo/AppDebug'                    : project('CH641_Servo_Debug',                       True,      '',   {'Release' : 'Config_CH641'}),
    'CH/CH641/Servo/Blu'                         : project('CH641_Servo_Blu',                         True,      '',   {'Release' : 'Config_CH641'}),
    'CH/CH641/Servo/Test'                        : project('CH641_Servo_Test',                        True,      '',   {'Release' : 'Config_CH641'}),

    # --- CH32V305 ------------------------------------------------
    'CH/V3/CH32V305/Mot/Ledblink'                : project('CH32V305_Mot_Ledblink',                   True,      '',   {'Release' : 'Config_CH32V305'}),
    'CH/V3/CH32V305/Mot/Servo5/App'              : project('CH32V305_Mot_Servo5_App',                 True,      '',   {'Release' : 'Config_CH32V305'}),
    'CH/V3/CH32V305/Mot/Servo5/AppDebug'         : project('CH32V305_Mot_Servo5_Debug',               True,      '',   {'Release' : 'Config_CH32V305'}),
    'CH/V3/CH32V305/Mot/Servo5/AppDebugI2c'      : project('CH32V305_Mot_Servo5_DebugI2c',            True,      '',   {'Release' : 'Config_CH32V305'}),
    'CH/V3/CH32V305/Mot/Servo5/MemTest'          : project('CH32V305_Mot_Servo5_MemTest',             True,      '',   {'Release' : 'Config_CH32V305'}),
    'CH/V3/CH32V305/Mot/Servo5/Blu'              : project('CH32V305_Mot_Servo5_Blu',                 True,      '',   {'Release' : 'Config_CH32V305'}),

    'CH/V3/CH32V305/LeuchtEi/Ledblink'           : project('CH32V305_LeuchtEi_Ledblink',              True,      '',   {'Release' : 'Config_CH32V305'}),
    'CH/V3/CH32V305/LeuchtEi/AppDebug'           : project('CH32V305_LeuchtEi_AppDebug',              True,      '',   {'Release' : 'Config_CH32V305'}),
    'CH/V3/CH32V305/LeuchtEi/AppDebugV9'         : project('CH32V305_LeuchtEi_AppDebugV9',            True,      '',   {'Release' : 'Config_CH32V305'}),
    'CH/V3/CH32V305/LeuchtEi/I2cTest'            : project('CH32V305_LeuchtEi_I2cTest',               True,      '',   {'Release' : 'Config_CH32V305'}),
    'CH/V3/CH32V305/LeuchtEi/EiBoot'             : project('CH32V305_LeuchtEi_EiBoot',                True,      '',   {'Release' : 'Config_CH32V305'}),
    'CH/V3/CH32V305/LeuchtEi/EiBootV9'           : project('CH32V305_LeuchtEi_EiBootV9',              True,      '',   {'Release' : 'Config_CH32V305'}),

    # --- nrf5340 ------------------------------------------------
    'nRF/nrf52840/LedBlink/P0.15'                : project('NRF52840_LedBlink_P0.15',                 True,      '',   {'Release'   : 'Config_nRF5'}),
    'nRF/nrf52840/Ls/Test0'                      : project('NRF52840_Ls_Test0',                       True,      '',   {'Release'   : 'Config_nRF5'}),
    'nRF/nrf52840/ble/uart'                      : project('NRF52840_ble_usart',                      True,      '',   {'Release'   : 'Config_nRF5'}),
    'nRF/nrf52840/ble/blinky'                    : project('NRF52840_ble_Blinky',                     True,      '',   {'Release'   : 'Config_nRF5'}),
    'nRF/nrf52840/ble/Ls'                        : project('NRF52840_ble_Ls',                         True,      '',   {'Release'   : 'Config_nRF5'}),


    # --- SAMD10/UBTECH/Servo ------------------------------------------------
    'ATMEL/SAMD10/XpMini/Ledblink'                : project('SAMD10_XpMini_Ledblink',                 True,      '',   {'Debug' : 'Config_SAMD10'}),
    'ATMEL/SAMD10/XpMini/Usart/Poll'              : project('SAMD10_XpMini_Usart_Poll',               True,      '',   {'Debug' : 'Config_SAMD10'}),
    'ATMEL/SAMD10/XpMini/Usart/Int'               : project('SAMD10_XpMini_Usart_Int',                True,      '',   {'Debug' : 'Config_SAMD10'}),
    'ATMEL/SAMD10/XpMini/Usart/Dma'               : project('SAMD10_XpMini_Usart_Dma',                True,      '',   {'Debug' : 'Config_SAMD10'}),

    'ATMEL/SAMD10/UbTech/Servo3/Ledblink'         : project('SAMD10_UbTech_Servo3_Ledblink',          True,      '',   {'Debug' : 'Config_SAMD10'}),
    'ATMEL/SAMD10/UbTech/Servo3/Mot'              : project('SAMD10_UbTech_Servo3_Mot',               True,      '',   {'Debug' : 'Config_SAMD10'}),
    'ATMEL/SAMD10/UbTech/Servo3/Adc'              : project('SAMD10_UbTech_Servo3_Adc',               True,      '',   {'Debug' : 'Config_SAMD10'}),
    'ATMEL/SAMD10/UbTech/Servo3/Servo'            : project('SAMD10_UbTech_Servo3_Servo',             True,      '',   {'Debug' : 'Config_SAMD10'}),

    'ATMEL/SAMD10/UbTech/Servo4/Ledblink'         : project('SAMD10_UbTech_Servo4_Ledblink',          True,      '',   {'Debug' : 'Config_SAMD10'}),
    'ATMEL/SAMD10/UbTech/Servo4/Mot'              : project('SAMD10_UbTech_Servo4_Mot',               True,      '',   {'Debug' : 'Config_SAMD10'}),
    'ATMEL/SAMD10/UbTech/Servo4/Enc'              : project('SAMD10_UbTech_Servo4_Enc',               True,      '',   {'Debug' : 'Config_SAMD10'}),
    'ATMEL/SAMD10/UbTech/Servo4/Adc'              : project('SAMD10_UbTech_Servo4_Adc',               True,      '',   {'Debug' : 'Config_SAMD10'}),

    # --- GD32F130G6/UBTECH/Servo ------------------------------------------------
    'STM/F1/GD32F1x0/UbTech/Servo1/Ledblink'      : project('GD32F1x0_UbTech_Servo1_Ledblink',        True,      '',   {'Debug' : 'Config_GD32F1x0'}),
    'STM/F1/GD32F1x0/UbTech/Servo1/App'           : project('GD32F1x0_UbTech_Servo1_App',             True,      '',   {'Release' : 'Config_GD32F1x0'}),
    'STM/F1/GD32F1x0/UbTech/Servo1/AppDebug'      : project('GD32F1x0_UbTech_Servo1_Debug',           True,      '',   {'Release' : 'Config_GD32F1x0'}),
    'STM/F1/GD32F1x0/UbTech/Servo1/Blu'           : project('GD32F1x0_UbTech_Servo1_Blu',             True,      '',   {'Release' : 'Config_GD32F1x0'}),
    'STM/F1/GD32F1x0/UbTech/Servo1/Tim0'          : project('GD32F1x0_UbTech_Servo1_Tim0',            True,      '',   {'Debug' : 'Config_GD32F1x0'}),
    'STM/F1/GD32F1x0/UbTech/Servo1/Usart/HdInt01' : project('GD32F1x0_UbTech_Servo1_Usart_HdInt01',   True,      '',   {'Debug' : 'Config_GD32F1x0'}),
    'STM/F1/GD32F1x0/UbTech/Servo1/Usart/MdHd01'  : project('GD32F1x0_UbTech_Servo1_Usart_MdInt01',   True,      '',   {'Debug' : 'Config_GD32F1x0'}),

    'STM/F1/GD32F1x0/MicroBn/MpHd01'              : project('GD32F1x0_MicroBn_MpHp01',                True,      '',   {'Release' : 'Config_GD32F1x0'}),

    # --- STM32F103RBT/BluePill ------------------------------------------------
    'STM/F1/STM32F103C8T/BluePill/LedBlink/1s'    : project('STM32F103C8T_LedBlink1s',                True,      '',   {'Debug' : 'Config_STM32F103xx'}),
    'STM/F1/STM32F103C8T/BluePill/LedBlink/100ms' : project('STM32F103C8T_LedBlink100ms',             True,      '',   {'Debug' : 'Config_STM32F103xx'}),
    'STM/F1/STM32F103C8T/BluePill/SM4PWM/Test01'  : project('STM32F103C8T_SW4PWM_Test01',             True,      '',   {'Debug' : 'Config_STM32F103xx'}),

    # --- STM32F3 ------------------------------------------------------------------
    'STM/F3/HalLib/v01.11.00'                     : project('STM32F3_HalLib',                         True,      '',   {'og' : 'Config_STM32F30x', 'o3' : 'Config_STM32F30x', 'os'  : 'Config_STM32F30x'}),

    # --- GD32F330G8 ------------------------------------------------
    'STM/F3/GD32F3x0/Mot/Ledblink'                : project('GD32F3x0_Mot_Ledblink',                  True,      '',   {'Release' : 'Config_GD32F3x0'}),
    'STM/F3/GD32F3x0/Mot/Servo5/App'              : project('GD32F3x0_Mot_Servo5_App',                True,      '',   {'Release' : 'Config_GD32F3x0'}),
    'STM/F3/GD32F3x0/Mot/Servo5/AppDebug'         : project('GD32F3x0_Mot_Servo5_Debug',              True,      '',   {'Release' : 'Config_GD32F3x0'}),
    'STM/F3/GD32F3x0/Mot/Servo5/Blu'              : project('GD32F3x0_Mot_Servo5_Blu',                True,      '',   {'Release' : 'Config_GD32F3x0'}),


    # --- STM32F302R8/Nucleo -------------------------------------------------
    'STM/F3/STM32F302R8/IHM07M1/Demo1'                      : project('STM32F302R8_IHM07M1_Demo1',    True,      '',   {'Release' : 'Config_STM32F30x'}),

    # --- STM32F302C8T6/BluePill -----------------------------------------------                    
    'STM/F3/STM32F302C8T6/Lidar/01'                         : project('STM32F302C8T6_Lidar_01',       True,      '',   {'Release' : 'Config_STM32F30x'}),

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
    'STM/F3/STM32F303CCT/BluePill/BotNet/PcIfcNet'       : project('STM32F303CCT_BotNet_PcIfcNet',    True,     '',   {'Debug' : 'Config_STM32F303', 'Release' : 'Config_STM32F303'}),

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


    'STM/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test01'    : project('STM32F407ZET_STF4BB_Gfx_Gui_v1_Test01',    True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test02'    : project('STM32F407ZET_STF4BB_Gfx_Gui_v1_Test02',    True,     '',   {'Debug' : 'Config_STM32F407x', 'Release' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test03'    : project('STM32F407ZET_STF4BB_Gfx_Gui_v1_Test03',    True,     '',   {'Debug' : 'Config_STM32F407x'}),	
    'STM/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test03_APDS9960'    : project('STM32F407ZET_STF4BB_Gfx_Gui_v1_Test03_APDS9960',    True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test03_LSM303'      : project('STM32F407ZET_STF4BB_Gfx_Gui_v1_Test03_LSM303',      True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test04'    : project('STM32F407ZET_STF4BB_Gfx_Gui_v1_Test04',    True,     '',   {'Debug' : 'Config_STM32F407x', 'Release' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test05'    : project('STM32F407ZET_STF4BB_Gfx_Gui_v2_Test05',    True,     '',   {'Debug' : 'Config_STM32F407x', 'Release' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/Misc/MemTest'     : project('STM32F407ZET_STF4BB_Gfx_Misc_MemTest', True,     '',   {'Debug' : 'Config_STM32F407x', 'Release' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/Gfx/WaveGen'          : project('STM32F407ZET_STF4BB_Gfx_WaveGen',      True,     '',   {'Debug' : 'Config_STM32F407x'}),

    'STM/F4/STM32F407ZET/STF4BB/Misc/ConstTest'   : project('STM32F407ZET_STF4BB_Misc_ConstTest',   True,     '',   {'Debug' : 'Config_STM32F407x', 'Release' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/I2C/APDS9960'     : project('STM32F407ZET_STF4BB_I2C_APDS9960',     True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/I2C/OLED'         : project('STM32F407ZET_STF4BB_I2C_OLED',         True,     '',   {'Debug' : 'Config_STM32F407x'}),
    'STM/F4/STM32F407ZET/STF4BB/I2C/SW6007'       : project('STM32F407ZET_STF4BB_I2C_SW6007',       True,     '',   {'Debug' : 'Config_STM32F407x'}),

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
    'STM/F4/STM32F429ZIT6/F429IDISCO/LedBlink/PG13'            : project('STM32F429ZIT6_LedBlink_PG13',           True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gfx/Gui/v1/SPI/Test01'    : project('STM32F429ZIT6_Gfx_Gui_v1_SPI_Test01',   True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gfx/Gui/v1/LCD/Test01'    : project('STM32F429ZIT6_Gfx_Gui_v1_LCD_Test01',   True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gfx/Gui/v1/LCD/Test02'    : project('STM32F429ZIT6_Gfx_Gui_v1_LCD_Test02',   True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gfx/Gui/v1/LCD/Test03'    : project('STM32F429ZIT6_Gfx_Gui_v1_LCD_Test03',   True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gfx/Gui/v2/LCD/Test05'    : project('STM32F429ZIT6_Gfx_Gui_v2_LCD_Test05',   True,     '',   {'Debug' : 'Config_STM32F429x'}),
    'STM/F4/STM32F429ZIT6/F429IDISCO/Gfx/Dcmi/OV7670/Test1'    : project('STM32F429ZIT6_Gfx_Dcmi_OV7670_Test1',   True,     '',   {'Debug' : 'Config_STM32F429x'}),

    'STM/F4/STM32F429ZIT6/F429IDISCO/Ex/01/10_MiniOszi'        : project('STM32F429ZIT6_10_MiniOszi',             True,     '',   {'Debug' : 'Config_STM32F429x'}),

    # --- STM32G4 -----------------------------------------------
    'STM/G4/HalLib/v01.05.00'                                  : project('STM32G4_HalLib',                        True,     '', {'og' : 'Config_STM32G474', 'o3' : 'Config_STM32G474', 'os'  : 'Config_STM32G474'}),

    # --- STM32G474/Discovery -----------------------------------------------
    'STM/G4/STM32G474RET6/DPOW1/BoostVoltHw'                   : project('STM32G474RET6_DPOW1_BoostVoltHw',       True,     '', {'Release' : 'Config_STM32G474'}),
    'STM/G4/STM32G474RET6/DPOW1/BuckCrntHw'                    : project('STM32G474RET6_DPOW1_BuckCrntHw',        True,     '', {'Release' : 'Config_STM32G474'}),
    'STM/G4/STM32G474RET6/DPOW1/BuckCrntSw'                    : project('STM32G474RET6_DPOW1_BuckCrntSw',        True,     '', {'Release' : 'Config_STM32G474'}),
    'STM/G4/STM32G474RET6/DPOW1/BuckVoltHw'                    : project('STM32G474RET6_DPOW1_BuckVoltHw',        True,     '', {'Release' : 'Config_STM32G474'}),
    'STM/G4/STM32G474RET6/DPOW1/BuckVoltSw'                    : project('STM32G474RET6_DPOW1_BuckVoltSw',        True,     '', {'Release' : 'Config_STM32G474'}),


    # --- STM32L4 -----------------------------------------------
    'STM/L4/HalLib/v01.15.01_8Mhz'                     : project('STM32L4_HalLib_8Mhz',                True,     '', {'og' : 'Config_STM32L4x3', 'o3' : 'Config_STM32L4x3', 'os'  : 'Config_STM32L4x3'}),
    'STM/L4/HalLib/v01.15.01_24Mhz'                    : project('STM32L4_HalLib_24Mhz',               True,     '', {'og' : 'Config_STM32L4x3', 'o3' : 'Config_STM32L4x3', 'os'  : 'Config_STM32L4x3'}),
    'STM/L4/HalLib/v01.17.00'                          : project('STM32L4_HalLib',                     True,     '', {'og' : 'Config_STM32L4R9', 'o3' : 'Config_STM32L4R9', 'os'  : 'Config_STM32L4R9'}),

    # --- STM32L433CCT/SolBx -----------------------------------------------
    'STM/L4/STM32L433CCT/SolBx/01/App'                 : project('STM32L433CCT_SolBx_01_App',          True,     '', {'Debug' : 'Config_STM32L4x3', 'Release' : 'Config_STM32L4x3', 'Debug_App'  : 'Config_STM32L4x3_BlApp', 'Release_App' : 'Config_STM32L4x3_BlApp'}),
    'STM/L4/STM32L433CCT/SolBx/01/Blu'                 : project('STM32L433CCT_SolBx_01_Blu',          True,     '', {'Debug' : 'Config_STM32L4x3', 'Release' : 'Config_STM32L4x3', 'RamRelease' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/SolBx/02/Master/01'           : project('STM32L433CCT_SolBx_02_Master_01',    True,     '',   {'Debug' : 'Config_STM32L4x3', 'Debug_App' : 'Config_STM32L4x3_BlApp'}),
    'STM/L4/STM32L433CCT/SolBx/02/MotBx/01'            : project('STM32L433CCT_SolBx_02_MotBx_01',     True,     '',   {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/SolBx/02/LightBx/01'          : project('STM32L433CCT_SolBx_02_LightBx_01',   True,     '',   {'Debug' : 'Config_STM32L4x3'}),

    #--- STM32L433CCT/Mini --------------------------------------------------
    'STM/L4/STM32L433CCT/Mini/Sys/BL'                  : project('STM32L433CCT_Mini_Sys_BL',           True,      '', {'Release' : 'Config_STM32L4x3', 'Test'        : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/Mini/Sys/BLU/MB_nrf905'       : project('STM32L433CCT_Mini_Sys_BLU_MB_nrf905',True,      '', {'Release' : 'Config_STM32L4x3', 'RamRelease'  : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/Mini/Sys/BLU/LedBrd_I2c'      : project('STM32L433CCT_Mini_Sys_BLU_LedBrd_I2c',True,     '', {'Release' : 'Config_STM32L4x3', 'RamRelease'  : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/Mini/Sys/BLU/PSwitch_I2c'     : project('STM32L433CCT_Mini_Sys_BLU_PSwitch_I2c',True,    '', {'Release' : 'Config_STM32L4x3', 'RamRelease'  : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/Mini/AddOn/LedBrdv1/Main'     : project('STM32L433CCT_Mini_AddOn_LedBrdv1_Main',   True, '', {'Release' : 'Config_STM32L4x3', 'Release_App' : 'Config_STM32L4x3_BlApp'}),
    'STM/L4/STM32L433CCT/Mini/AddOn/LedBrdv2/Main'     : project('STM32L433CCT_Mini_AddOn_LedBrdv2_Main',   True, '', {'Release' : 'Config_STM32L4x3', 'Release_App' : 'Config_STM32L4x3_BlApp'}),

    'STM/L4/STM32L433CCT/Mini/AddOn/PSwitch/v3/I2c'    : project('STM32L433CCT_Mini_AddOn_PSwitch_v3_I2c', True, '',  {'Release' : 'Config_STM32L4x3', 'Release_App' : 'Config_STM32L4x3_BlApp'}),
    'STM/L4/STM32L433CCT/Mini/AddOn/PSwitch/v3/SLed'   : project('STM32L433CCT_Mini_AddOn_PSwitch_v3_SLed', True, '', {'Release' : 'Config_STM32L4x3', 'Release_App' : 'Config_STM32L4x3_BlApp'}),

    'STM/L4/STM32L433CCT/Mini/LedBlink/PB9Int'         : project('STM32L433CCT_Mini_LedBlink_PB9Int',  True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/Mini/LedBlink/PB9Int'         : project('STM32L433CCT_Mini_LedBlink_PB9Int',  True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/Mini/LedBlink/PB9Ext'         : project('STM32L433CCT_Mini_LedBlink_PB9Ext',  True,      '', {'Debug' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/Mini/MBv10b2/TestEep'         : project('STM32L433CCT_Mini_MBv10b2_TestEep',  True,      '', {'Release' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/Mini/MBv10b2/Main01'          : project('STM32L433CCT_Mini_MBv10b2_Main01',   True,      '', {'Release' : 'Config_STM32L4x3', 'Release_App' : 'Config_STM32L4x3_BlApp', 'Test' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/Mini/MBv10b2/Lampe/Pflanzen/01':project('STM32L433CCT_Mini_MBv10b2_Pflanzen_01', True,   '', {'Release' : 'Config_STM32L4x3', 'Release_App' : 'Config_STM32L4x3_BlApp', 'Test' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/Mini/MBv10b2/Lampe/Bad/01'    : project('STM32L433CCT_Mini_MBv10b2_Bad_01',   True,      '', {'Release' : 'Config_STM32L4x3', 'Release_App' : 'Config_STM32L4x3_BlApp', 'Test' : 'Config_STM32L4x3'}),



    'STM/L4/STM32L433CCT/Mini/Test/Btr'                : project('STM32L433CCT_Mini_Test_Btr',         True,      '', {'Release' : 'Config_STM32L4x3'}),

    #--- STM32L433CCT/BluePill --------------------------------------------------
    'STM/L4/STM32L433CCT/BluePill/I2C/APDS9960/Als'    : project('STM32L433CCT_I2C_APDS9960_Als',       True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/I2C/APDS9960/Ds'     : project('STM32L433CCT_I2C_APDS9960_Ds',        True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/I2C/APDS9960/Gs'     : project('STM32L433CCT_I2C_APDS9960_Gs',        True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/I2C/HTU21D'          : project('STM32L433CCT_I2C_HTU21D',             True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/I2C/INA219'          : project('STM32L433CCT_I2C_INA219',             True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/I2C/OLED'            : project('STM32L433CCT_I2C_OLED',               True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/I2C/SW6007'          : project('STM32L433CCT_I2C_SW6007',             True,      '', {'Debug' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/BluePill/Mot/Test01'          : project('STM32L433CCT_Mot_Test01',             True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Mot/Test02'          : project('STM32L433CCT_Mot_Test02',             True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Mot/Test03'          : project('STM32L433CCT_Mot_Test02',             True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/LedBlink/PC13'       : project('STM32L433CCT_LedBlink_PC13',          True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/LedBlink/All'        : project('STM32L433CCT_LedBlink_All',           True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/LedBlink/All2'       : project('STM32L433CCT_LedBlink_All2',          True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/Usart12'        : project('STM32L433CCT_Tests_Usart12',          True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/Cli'            : project('STM32L433CCT_Tests_Cli',              True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/Compiler'       : project('STM32L433CCT_Tests_Compiler',         True,      '', {'Release' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/PowTest'        : project('STM32L433CCT_Tests_PowTest',          True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/PowTestRtc'     : project('STM32L433CCT_Tests_PowTestRtc',       True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Test/Loop8Mhz'       : project('STM32L433CCT_Tests_Loop8Mhz',         True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Gfx/ST7735'          : project('STM32L433CCT_Gfx_ST7735',             True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/PwrBx'               : project('STM32L433CCT_PwrBx',                  True,      '', {'Debug' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/BluePill/Usart/Usart1Echo'    : project('STM32L433CCT_Tests_Usart1Echo',       True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Usart/Servo/Switch'  : project('STM32L433CCT_Tests_Servo_Switch',     True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Usart/Servo/Test01'  : project('STM32L433CCT_Tests_Servo_Test01',     True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Usart/Servo/Test02'  : project('STM32L433CCT_Tests_Servo_Test02',     True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Usart/Servo/Test03'  : project('STM32L433CCT_Tests_Servo_Test03',     True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/Usart/Servo/Test04'  : project('STM32L433CCT_Tests_Servo_Test04',     True,      '', {'Debug' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/BluePill/MAX31855/Test01'     : project('STM32L433CCT_MAX31855_Test01',        True,      '', {'Debug' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/BluePill/LaufSens/Test01'     : project('STM32L433CCT_LaufSens_Test01',        True,      '', {'Release' : 'Config_STM32L4x3'}),

    'STM/L4/STM32L433CCT/BluePill/WS2812/Test01'       : project('STM32L433CCT_Tests_WS2812_Test01',    True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/WS2812/Test02'       : project('STM32L433CCT_Tests_WS2812_Test02',    True,      '', {'Debug' : 'Config_STM32L4x3'}),
    'STM/L4/STM32L433CCT/BluePill/WS2812/Test03'       : project('STM32L433CCT_Tests_WS2812_Test03',    True,      '', {'Debug' : 'Config_STM32L4x3'}),

    #--- STM32L4R9/STM32L4R9IDISCOVERY -------------------------------------
    'STM/L4/STM32L4R9/L4R9AIDISCO/LedBlink/PB13'       : project('STM32L4R9_LedBlink_PB13',             True,      '', {'Debug' : 'Config_STM32L4R9'}),

    
    #--- STM32L552ZET/NUCLEO --------------------------------------------------
    'STM/L5/STM32L552ZET/L552_Nucleo/LedBlink/PB7'     : project('STM32L552ZET_LedBlink_PB7',           True,      '', {'Debug' : 'Config_STM32L552'}),

    #--- STM32U575ZI/NUCLEO --------------------------------------------------
    'STM/U5/HalLib/v01.01.00_32Mhz'                    : project('STM32U5_HalLib_32Mhz',                True,      '', {'og' : 'Config_STM32U5xx', 'o3' : 'Config_STM32U5xx', 'os'  : 'Config_STM32U5xx'}),
    'STM/U5/STM32U575ZI/Nucleo/LedBlink/PB7'           : project('STM32U575ZI_LedBlink_PB7',            True,      '', {'Debug' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U575ZI/Nucleo/Gfx/Gui/v2/Xpt2046Test' : project('STM32U575ZI_U575_Nucleo_Gfx_Gui_v2_Xpt2046Test', True,     '',   {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U575ZI/Nucleo/Gfx/Gui/v2/Test05'      : project('STM32U575ZI_U575_Nucleo_Gfx_Gui_v2_Test05',      True,     '',   {'Debug' : 'Config_STM32U5xx', 'Release' : 'Config_STM32U5xx'}),

    'STM/U5/STM32U575ZI/Nucleo/Examples/DMA/M2MLL'     : project('STM32U575ZI_Examples_DMA_M2MLL',      True,      '', {'Debug' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U575ZI/Nucleo/Examples/DMA/M2SpiHal'  : project('STM32U575ZI_Examples_DMA_M2SpiHal',   True,      '', {'Debug' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U575ZI/Nucleo/Examples/SPI/ITLL'      : project('STM32U575ZI_Examples_SPI_ITLL',       True,      '', {'Debug' : 'Config_STM32U5xx'}),

    #--- STM32U585CIT --------------------------------------------------
    'STM/U5/STM32U585CIT/BluePill/I2C/APDS9960/Als'    : project('STM32U585CIT_I2C_APDS9960_Als',       True,      '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/BluePill/I2C/APDS9960/Ds'     : project('STM32U585CIT_I2C_APDS9960_Ds',        True,      '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/BluePill/I2C/APDS9960/Gs'     : project('STM32U585CIT_I2C_APDS9960_Gs',        True,      '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/BluePill/I2C/OLED'            : project('STM32U585CIT_I2C_OLED',               True,      '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/LedBlink/All2'                : project('STM32U585CIT_LedBlink_PB7',           True,      '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/Gfx/Gui/v2/Test05'       : project('STM32U585CIT_U585_Mini_Gfx_Gui_v2_Test05', True, '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/Gfx/Test01'              : project('STM32U585CIT_U585_Mini_Gfx_Test01',   True,      '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/Gfx/Test02'              : project('STM32U585CIT_U585_Mini_Gfx_Test02',   True,      '', {'Debug'   : 'Config_STM32U5xx'}),

    'STM/U5/STM32U585CIT/Mini/MBv7b4/I2C/INA3221'      : project('STM32U585CIT_Mini_MBv7b4_I2C_INA3221',True,      '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/MBv7b4/Main01'           : project('STM32U585CIT_Mini_MBv7b4_Main01',    True,       '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/MBv7b4/SleepTest'        : project('STM32U585CIT_Mini_MBv7b4_SleepTest', True,       '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/MBv7b2/I2C/INA3221'      : project('STM32U585CIT_Mini_MBv7b2_I2C_INA3221',True,      '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/MBv7b2/Main01'           : project('STM32U585CIT_Mini_MBv7b2_Main01',    True,       '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/MBv7b2/SleepTest'        : project('STM32U585CIT_Mini_MBv7b2_SleepTest', True,       '', {'Release' : 'Config_STM32U5xx'}),
    
    'STM/U5/STM32U585CIT/Mini/Test/OSpi'               : project('STM32U585CIT_Mini_Test_OSpi',          True,     '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/MBv10b2/MainOSpi01'      : project('STM32U585CIT_Mini_MBv10b2_MainOSpi01', True,     '', {'Release' : 'Config_STM32L4x3', 'Release_App' : 'Config_STM32L4x3_BlApp', 'Test' : 'Config_STM32L4x3'}),

    'STM/U5/STM32U585CIT/Mini/MBv28b2/I2C/INA3221'     : project('STM32U585CIT_Mini_MBv28b2_I2C_INA3221',True,     '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/MBv28b2/I2C/TPS55288'    : project('STM32U585CIT_Mini_MBv28b2_I2C_TPS55288',True,    '', {'Release' : 'Config_STM32U5xx'}),
    'STM/U5/STM32U585CIT/Mini/MBv28b2/Main01'          : project('STM32U585CIT_Mini_MBv28b2_Main01',    True,      '', {'Release' : 'Config_STM32U5xx'}),


    #--- STM32H7B0 -------
    'STM/H7/HalLib/v01.11.00'                          : project('STM32H7_HalLib',                      True,      '', {'og' : 'Config_STM32H7B0', 'o3' : 'Config_STM32H7B0', 'os'  : 'Config_STM32H7B0'}),
    'STM/H7/STM32H7B0/WasMini/LedBlink/PE3'            : project('STM32H7B0_LedBlink_PE3',              True,      '', {'Release' : 'Config_STM32H7B0'}),
    'STM/H7/STM32H7B0/WasMini/QSpi/Test'               : project('STM32H7B0_QSpi_Test',                 True,      '', {'Release' : 'Config_STM32H7B0'}),
    'STM/H7/STM32H7B0/WasMini/Gfx/Test01'              : project('STM32H7B0_Gfx_Test01',                True,      '', {'Debug' : 'Config_STM32H7B0'}),
    'STM/H7/STM32H7B0/WasMini/Cam/OV7725/Test01'       : project('STM32H7B0_Cam_OV7725_Test01',         True,      '', {'Debug' : 'Config_STM32H7B0'}),
    'STM/H7/STM32H7B0/WasMini/CNC/01'                  : project('STM32H7B0_CNC_01',                    True,      '', {'Release' : 'Config_STM32H7B0'}),

 
    #--- Pc Simulation ----------------------------------------------

    #--- STM32F407VGT/Blackboard ----------------------------------------------
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test01'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v1_Test01',    True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test02'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v1_Test02',    True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test03'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v1_Test03',    True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v1/Test04'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_Gui_v1_Test04',True,     '',   {'Debug' : 'Config_PcMsWin'}),

    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test01'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v2_Test01',    True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test02'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v2_Test02',    True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test03'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v2_Test03',    True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test04'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v2_Test04',    True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/Gui/v2/Test05'   : project('PcSim_STM32F407ZET_STF4BB_Gfx_Gui_v2_Test05',    True,     '',   {'Debug' : 'Config_PcMsWin'}),

    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/WaveGen'         : project('PcSim_STM32F407ZET_STF4BB_Gfx_WaveGen',          True,     '',   {'Debug' : 'Config_PcMsWin'}),

    'PcSim/F4/STM32F407ZET/STF4BB/Gfx/FrakGra/01'      : project('PcSim_STM32F407ZET_STF4BB_Gfx_FrakGra_01',       True,     '',   {'Debug' : 'Config_PcMsWin'}),
    
    #--- STM32H7B0 -------
    'PcSim/H7/STM32H7B0/WasMini/CNC/Test01'            : project('PcSim_STM32H7B0_WasMini_CNC_Test01',             True,     '',   {'Debug' : 'Config_PcMsWinConsole'}),

    #--- STM32L433CCT/BluePill --------------------------------------------------
    'PcSim/L4/STM32L433CCT/BluePill/Bootloader/Test01'    : project('PcSim_STM32L433CCT_Bootloader_Test01',        True,     '',   {'Debug' : 'Config_PcMsWinConsole'}),

    #--- STM32L552ZET/NUCLEO --------------------------------------------------
    'PcSim/L5/STM32L552ZET/L552_Nucleo/Safety/Test01'     : project('PcSim_STM32L552ZET_L552_Nucleo_Safety_Test01',    True,     '',   {'Debug' : 'Config_PcMsWinConsole'}),

    #--- Pc -------------------------------------------------------------------
    'Pc/Garmin/Test01'                                    : project('Pc_Garmin_Test01',            True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'Pc/Garmin/Test02'                                    : project('Pc_Garmin_Test02',            True,     '',   {'Debug' : 'Config_PcMsWin'}),
    'Pc/Test/OverloadOpTest01'                            : project('Pc_Test_OverloadOpTest01',    True,     '',   {'Debug' : 'Config_PcMsWinConsole'}),
    'Pc/Slicer'                                           : project('Pc_Slicer',                   True,     '',   {'Debug' : 'Config_PcMsWin'}),
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
                                                                                                                                                                                                                                                                                                                    