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

    # open ocd
    #'flash': {
    #    'bin':    'openocd',
    #    'file':   '$slash{${system/file/elf}}',
    #    'target': '[target]',
    #    'args':   '-f interface/cmsis-dap.cfg -f ${target} -c "program ${file} verify reset exit"',
    #    'call':   '${bin} ${args}',
    #},

    # open ocd
    'flash': {
        'bin':    'openocd',
        'file':   '$slash{${system/file/elf}}',
        'target': '[target]',
        'args':   '-f interface/stlink-dap.cfg -f ${target} -c "program ${file} verify reset exit"',
        'call':   '${bin} ${args}',
    },

    # Elf2bin
    'elf2bin': {
        # gcc-Compiler generate bin
        'bin': 'arm-none-eabi-objcopy',
        'args': '-O binary ${system/file/elf} ${system/file/elf}.bin',
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
default_Config_ATM_CMX = {
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
Config_SAMD10    = copy.deepcopy(default_Config_ATM_CMX)

Config_SAMD10['system']['project']['target']['platform'] = 'SAMD10D14AM'
Config_SAMD10['system']['project']['target']['file_svd'] = '${system/dir/root}/scripts/atmel/ATSAMD10D14AM.svd'
Config_SAMD10['buildtool']['flash']['target']               = 'target/at91samdXX.cfg'


# ------------------------
# use-able configurations:
# ------------------------

# all Configurations are collected in this list, so that they can be accessed from the projects via project->configs
configs_Atmel = {
    'Config_SAMD10': Config_SAMD10,
}



