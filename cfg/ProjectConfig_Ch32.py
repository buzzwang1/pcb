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

default_buildtool_gcc_RiscV = {
    # EmBitz debugger
    'debug': {
        'bin': '$path{${system/dir/project/build/bin}/${project/name}.wvproj}',
        'args': '',
        'call': '${bin}',
    },

    # open ocd
    'flash': {
        'bin':    'openocd',
        'file':   '$slash{${system/file/elf}}',
        'target': '[target]',
        #Workaround, because 0.11.0-WCH don't accepts the relative path
        'args':   '-f $slash{${system/dir/tools}/openocd/0.11.0-WCH/scripts/target/wch-riscv.cfg} -c "program ${file} verify reset exit"',
        'call':   '${bin} ${args}',
    },


    # Elf2bin
    'elf2bin': {
        # gcc-Compiler generate bin
        'bin': 'riscv-none-embed-objcopy',
        'args': '-O binary ${system/file/elf} ${system/file/elf}.bin',
        'call': '${bin} ${args}',
    },

    # Elf2hex
    'elf2hex': {
        # gcc-Compiler generate bin
        'bin': 'riscv-none-embed-objcopy',
        'args': '-O ihex ${system/file/elf} ${system/file/elf}.hex',
        'call': '${bin} ${args}',
    },

    'dirbin': {
        # gcc-Compiler generate bin
        'bin': 'dir',
        'args': '$path{system/file/elf}.bin',
        'call': '${bin} ${args}',
    },

    # create list file
    'elf2lst': {
        'bin': 'riscv-none-embed-objdump',
        'args': '--all-headers --demangle --disassemble -M xw ${system/file/elf} > ${system/file/elf}.lst',
        'call': '${bin} ${args}',
    },

    'size': {
        'memreport': {
            'bin': 'riscv-none-embed-size',
            'args': '--format=berkeley -tA ${system/file/elf}',
            'call': '${bin} ${args}',
        },
    },
}

# --- for 'maketool' ---
default_cmake_ninja_RiscV = {
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
        'toolchain': 'GccRiscV-Ninja.cmake',
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

default_tools_gcc_RiscV = {
    'prepare_debug': {
        # Subscript
        'subscript': {
            'script': '${system/dir/root}/scripts/pcb/py/addons/mrs/mrs.py',
            'call': '$python{script}',
        },
    },
}

default_tools_gcc_RiscV.update(default_tools)

# Choose the desired system, codgen, codegentool, buildtool, maketool, reports, tests
# Cortex M [x] => CMX
default_Config_RiscV = {
    # Get the default values
    'system'   : default_system,
    'codegen'  : default_codegeneration,
    'buildtool': default_buildtool_gcc_RiscV,
    'maketool' : default_cmake_ninja_RiscV,
    'reports'  : default_reports,
    'tests'    : default_tests,
    'tools'    : default_tools_gcc_RiscV,
}

# Use default values and modify them, if necessary
Config_CH32V305    = copy.deepcopy(default_Config_RiscV)
Config_CH641       = copy.deepcopy(default_Config_RiscV)

Config_CH32V305['system']['project']['target']['platform'] = 'CH32V305'
Config_CH32V305['system']['project']['target']['file_svd'] = '${system/dir/root}/scripts/ch/CH32V305xx.svd'
Config_CH641['system']['project']['target']['platform'] = 'CH641'
Config_CH641['system']['project']['target']['file_svd'] = '${system/dir/root}/scripts/ch/CH641.svd'

# ------------------------
# use-able configurations:
# ------------------------

# all Configurations are collected in this list, so that they can be accessed from the projects via project->configs
configs_Ch32 = {
    'Config_CH32V305'   : Config_CH32V305,
    'Config_CH641'      : Config_CH641,
}



