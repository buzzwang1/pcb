﻿import copy # for deepcopy

from ProjectConfig_Default import default_system
from ProjectConfig_Default import default_codegeneration
from ProjectConfig_Default import default_reports
from ProjectConfig_Default import default_tests
from ProjectConfig_Default import default_tools

# ----------------------------------
# extend the default configurations:
# ----------------------------------

# --- for 'buildtool' ---

default_buildtool_gcc_cm = {
    # EmBitz debugger
    'debug': {
        'bin': 'embitz $path{${system/dir/project/build/gen}/EmBitz/project.ebp}',
        'args': '',
        'call': '${bin}',
    },

    # ST flash
    'flash': {
        'bin': 'ST-LINK_CLI',
        'file':   '${system/file/elf}.bin',
        'adress': '0x8000000',
        'args': '-P ${file} ${adress} -Rst',
        'call':   '${bin} ${args}',
    },

    # ST flash
    'flash2': {
        'bin':    'STM32_Programmer_CLI',
        'file':   '${system/file/elf}.bin',
        'adress': '0x8000000',
        'args':   '-c port=SWD -w ${file} ${adress} -rst',
        'call':   '${bin} ${args}',
    },

    # Elf2bin
    'elf2bin': {
        # gcc-Compiler generate bin
        'bin': 'arm-none-eabi-objcopy',
        'args': '-O binary ${system/file/elf} ${system/file/elf}.bin',
        'call': '${bin} ${args}',
    },

    'size': {
        'memreport': {
            'bin': 'arm-none-eabi-size',
            'args': '-tA ${system/file/elf}',
            'call': '${bin} ${args}',
        },
    },
}

# --- for 'maketool' ---
default_cmake_ninja_CM4 = {
    # --- CMake Generate & Build process. Used by build.py

    # Used by the hardcoded command projects/Export_ProjCfgLst
    # Will be imported by cmake
    'exportcfg':
        {
            'file_out': '$path{${codegen/dir}/ProjectCfg.cmake}',
        },

    'generate': {
        'bin': 'cmake',
        'type': '${project/config}',
        'toolchain': 'GccArmCortex-Ninja.cmake',
        'args': '-G Ninja -DPCB_ProjectLabel="${project/label}" -DPCB_ProjectCfgFile="${maketool/exportcfg/file_out}" -DCMAKE_TOOLCHAIN_FILE=${system/dir/root}/scripts/pcb/cmake/ToolChainFiles/${toolchain} -H. -B${system/dir/project/build/cmake}',
        'call': '${bin} ${args}',
    },

    'build': {
        'bin': 'cmake',
        'args': '--build ${system/dir/project/build/cmake} -- -j 1',
        'call': '${bin} ${args}',
    },
}


# --- for 'tools' ---

default_tools_gcc_cmx = {
    'create_embitz_ebp': {
        # Subscript
        'subscript': {
            'script': '${system/dir/root}/scripts/pcb/py/addons/embitz/embitz.py',
            'call': '$python{script}',
        },
    },
}

default_tools_gcc_cmx.update(default_tools)

# Choose the desired system, codgen, codegentool, buildtool, maketool, reports, tests
# Cortex M [x] => CMX
default_Config_STM32_CMX = {
    # Get the default values
    'system'   : default_system,
    'codegen'  : default_codegeneration,
    'buildtool': default_buildtool_gcc_cm,
    'maketool' : default_cmake_ninja_CM4,
    'reports'  : default_reports,
    'tests'    : default_tests,
    'tools'    : default_tools_gcc_cmx,
}

# Use default values and modify them, if necessary
Config_STM32_CM3   = copy.deepcopy(default_Config_STM32_CMX)
Config_STM32_CM4   = copy.deepcopy(default_Config_STM32_CMX)
Config_STM32F100xx = copy.deepcopy(default_Config_STM32_CMX)
Config_STM32F103xx = copy.deepcopy(default_Config_STM32_CMX)
Config_STM32F303   = copy.deepcopy(default_Config_STM32_CMX)
Config_STM32F407x  = copy.deepcopy(default_Config_STM32_CMX)
Config_STM32F413x  = copy.deepcopy(default_Config_STM32_CMX)
Config_STM32F429x  = copy.deepcopy(default_Config_STM32_CMX)
Config_STM32L4x3   = copy.deepcopy(default_Config_STM32_CMX)
Config_STM32L4R9   = copy.deepcopy(default_Config_STM32_CMX)
Config_STM32L552   = copy.deepcopy(default_Config_STM32_CMX)

Config_STM32F100xx['system']['project']['target']['platform'] = 'STM32F100xx'
Config_STM32F100xx['system']['project']['target']['file_svd'] = '${system/dir/root}/scripts/stm/STM32F100xx.svd'
Config_STM32F103xx['system']['project']['target']['platform'] = 'STM32F103xx'
Config_STM32F103xx['system']['project']['target']['file_svd'] = '${system/dir/root}/scripts/stm/STM32F103xx.svd'
Config_STM32F303['system']['project']['target']['platform']   = 'STM32F303'
Config_STM32F303['system']['project']['target']['file_svd']   = '${system/dir/root}/scripts/stm/STM32F30x.svd'
Config_STM32F407x['system']['project']['target']['platform']  = 'STM32F407x'
Config_STM32F407x['system']['project']['target']['file_svd']  = '${system/dir/root}/scripts/stm/STM32F429x.svd'
Config_STM32F413x['system']['project']['target']['platform']  = 'STM32F413x'
Config_STM32F413x['system']['project']['target']['file_svd']  = '${system/dir/root}/scripts/stm/STM32F413.svd'
Config_STM32F429x['system']['project']['target']['platform']  = 'STM32F429x'
Config_STM32F429x['system']['project']['target']['file_svd']  = '${system/dir/root}/scripts/stm/STM32F429x.svd'
Config_STM32L4x3['system']['project']['target']['platform']   = 'STM32L4x3'
Config_STM32L4x3['system']['project']['target']['file_svd']   = '${system/dir/root}/scripts/stm/STM32L4x3.svd'
Config_STM32L4R9['system']['project']['target']['platform']   = 'STM32L4R9'
Config_STM32L4R9['system']['project']['target']['file_svd']   = '${system/dir/root}/scripts/stm/STM32L4R9.svd'
Config_STM32L552['system']['project']['target']['platform']   = 'STM32L552'
Config_STM32L552['system']['project']['target']['file_svd']   = '${system/dir/root}/scripts/stm/STM32L552.svd'

Config_STM32F303_BlApp = copy.deepcopy(Config_STM32F303)
Config_STM32F303_BlApp['buildtool']['flash']['adress']        = '0x8008000' # 8000 (32kb) offset for bootloader

Config_STM32L4x3_BlApp = copy.deepcopy(Config_STM32L4x3)
Config_STM32L4x3_BlApp['buildtool']['flash']['adress']        = '0x8008000' # 8000 (32kb) offset for bootloader

# ------------------------
# use-able configurations:
# ------------------------

# all Configurations are collected in this list, so that they can be accessed from the projects via project->configs
configs_Stm32 = {
    'Config_STM32_CM3'  : Config_STM32_CM3,
    'Config_STM32_CM4'  : Config_STM32_CM4,

    'Config_STM32F100xx': Config_STM32F100xx,
    'Config_STM32F103xx': Config_STM32F103xx,
    'Config_STM32F303'  : Config_STM32F303,
    'Config_STM32F303_BlApp' : Config_STM32F303_BlApp,
    'Config_STM32F407x' : Config_STM32F407x,
    'Config_STM32F413x' : Config_STM32F413x,
    'Config_STM32F429x' : Config_STM32F429x,
    'Config_STM32L4x3'  : Config_STM32L4x3,
    'Config_STM32L4x3_BlApp' : Config_STM32L4x3_BlApp,
    'Config_STM32L4R9'  : Config_STM32L4R9,
    'Config_STM32L552'  : Config_STM32L552,
}


