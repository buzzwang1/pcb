import copy # for deepcopy

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

    # ST Link
    'flash': {
        'bin': 'ST-LINK_CLI',
        'file':   '${system/file/elf}.bin',
        'adress': '0x8000000',
        'args': '-P ${file} ${adress} -Rst',
        'call':   '${bin} ${args}',
    },

    # ST programmer
    'flash2': {
        'bin':    'STM32_Programmer_CLI',
        'file':   '${system/file/elf}',
        'args':   '-c port=SWD -w ${file} -rst',
        'call':   '${bin} ${args}',
    },

    # open ocd
    'flash3': {
        'bin':    'openocd',
        'file':   '$slash{${system/file/elf}.hex}',
        'target': '[target]',
        #Workaround, because 0.11.0-WCH don't accepts the relative path
        'arg_target': '-f $slash{${system/dir/tools}/openocd/0.11.0-WCH/scripts/${target}}',
        'arg_intf':   '-f $slash{${system/dir/tools}/openocd/0.11.0-WCH/scripts/interface/stlink.cfg}',
        'args':   '${arg_intf} ${arg_target} -c "program ${file} verify reset exit"',
        #'args':   '-f interface/stlink.cfg -f ${target} -c "program ${file} verify reset exit"',
        'call':   '${bin} ${args}',
    },


    # Elf2bin
    'elf2bin': {
        # gcc-Compiler generate bin
        'bin': 'arm-none-eabi-objcopy',
        'args1': '-O binary ${system/file/elf} ${system/file/elf}.bin',
        'args2': '-O ihex ${system/file/elf} ${system/file/elf}.hex',
        'call': '${bin} ${args1} && ${bin} ${args2}',
    },

    'dirbin': {
        # gcc-Compiler generate bin
        'bin': 'dir',
        'args': '$path{system/file/elf}.bin',
        'call': '${bin} ${args}',
    },

    # create list file
    'elf2lst': {
        'bin': 'arm-none-eabi-objdump',
        'args': '-S -h ${system/file/elf} > ${system/file/elf}.lst',
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
    'prepare_debug': {
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
default_Config_nRF_CMX = {
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
Config_nRF5        = copy.deepcopy(default_Config_nRF_CMX)

Config_nRF5['system']['project']['target']['platform'] = 'nRF5'
Config_nRF5['system']['project']['target']['file_svd'] = '${system/dir/root}/scripts/nrf/nrf52840.svd'

Config_nRF5['buildtool']['flash']                         = Config_nRF5['buildtool']['flash3']
Config_nRF5['buildtool']['flash']['target']               = 'target/nrf52.cfg'

# ------------------------
# use-able configurations:
# ------------------------

# all Configurations are collected in this list, so that they can be accessed from the projects via project->configs
configs_nRf = {
    'Config_nRF5'       : Config_nRF5,
}



