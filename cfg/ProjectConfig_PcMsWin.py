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

default_buildtool_PcMsWin = {

}

# --- for 'maketool' ---
default_cmake_ninja_PcMsWin = copy.deepcopy(default_tools)
default_cmake_ninja_PcMsWin = {
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
        'args': '-G Ninja -DPCB_ProjectLabel="${project/label}" -DPCB_ProjectCfgFile="${maketool/exportcfg/file_out}" -H. -B${system/dir/project/build/cmake}',
        'call': '${bin} ${args}',
    },
}


# --- for 'tools' ---
default_tools_PcMsWin = copy.deepcopy(default_tools)


# Choose the desired system, codgen, codegentool, buildtool, maketool, reports, tests
# Cortex M [x] => CMX
default_Config_PcMsWin = {
    # Get the default values
    'system'   : default_system,
    'codegen'  : default_codegeneration,
    'buildtool': default_buildtool_PcMsWin,
    'maketool' : default_cmake_ninja_PcMsWin,
    'reports'  : default_reports,
    'tests'    : default_tests,
    'tools'    : default_tools_PcMsWin,
}

# Use default values and modify them, if necessary
Config_PcMsWin        = copy.deepcopy(default_Config_PcMsWin)
Config_PcMsWinConsole = copy.deepcopy(default_Config_PcMsWin)

Config_PcMsWin['system']['project']['target']['platform'] = 'MsWin'
Config_PcMsWinConsole['system']['project']['target']['platform'] = 'MsWinConsole'

# ------------------------
# use-able configurations:
# ------------------------

# all Configurations are collected in this list, so that they can be accessed from the projects via project->configs
configs_PcWin = {
    'Config_PcMsWin'         : Config_PcMsWin,
    'Config_PcMsWinConsole'  : Config_PcMsWinConsole,
}



