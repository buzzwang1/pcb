# -----------------------
# default configurations:
# -----------------------

# --- for 'system' ---
default_system = {
    # Placeholders, will be replaced during runtime
    # [dir_root]: location of the projects root (location of ${root}/CMakeLists.txt)
    # [dir_source]: location of the source (normally in ${dir_root}/source)
    # [dir_tools]: location of the build-tools
    # [dir_build]: location of the build-folder (normally in ${dir_root}/bin)
    # [dir_build_project]: location of the build-folder of the project(normally in ${dir_root}/bin/${project})
    # [dir_build_project_bin]: location of the project's binary\executable. It is defined in ${root}/CMakeLists.txt->EXECUTABLE_OUTPUT_PATH
    # [dir_debugger]: location of the debugger. It is defined in CMAKE_TOOLCHAIN_FILE->PCB_DebuggerPath
    # [dir_compiler]: location of the compiler (e.g. to load the device files). It is defined in CMAKE_TOOLCHAIN_FILE->PCB_CompilerPath
    # [elf_file]: location and name of the elf file. It is defined in ${root}/CMakeLists.txt-> PCB_BinaryFilename
    
    'dir' : {
        'root'    : '[dir_root]',
        'source'  : '[dir_source]',
        'build'   : '[dir_build]',
        'tools'   : '[dir_tools]',
        'cfg'     : '${root}/cfg',
        'debugger': '[dir_debugger]',
        'compiler': '[dir_compiler]',

        'scripts': {
            'root'         : '${../build}/scripts',
            'pcb_py_core'  : '${root}/pcb/py/core',
            'pcb_py_addon' : '${root}/pcb/py/addons',
            'pcb_cm'       : '${root}/pcb/cmake',
            },

        'project': {
            'build': {
                # Build directory, used as destination for Cmake
                # optional add ${project/config} to separate project's configuration e.g. release/debug
                'root'  : '${../../build}/${project/label}/${project/config}',
                'cmake' : '${root}/cmake',
                'bin'   : '${root}/bin',

                # gen directory, to generate files, like PackageList
                # optional add ${project/config} to separate project's configuration e.g. release/debug
                'gen'   : '${root}/gen',
                'tool'  : '${root}/tool',
                'report': '${root}/tool',
            },
            'cfg': {
                'root'  : '${../../cfg}/prj/${project/label}',
            },
        },

        'check': # is used in PkgGenLists, to warn if any package entry file length is over the limit
            {
                'warn_threshold' : '220'
            }
    },
        
    'file': {
            'elf' : '[elf_file]',
            'filters' : {
                'source' : '*.c;*.cpp',
                'asm' :    '*.s;*.800;*.asm',
                'header' : '*.h;*.hpp',
                'bin':     '*.elf;*.bin;*.hex;*.srec',
            },
    },

    'project': {
        'target': {
            # Definitions, must be implemented by the child configuration
            'platform': '', # Cross, MsWin, MsWinConsole, STM32F100xx, ...
            'file_svd': ''
        },
    },
}


# --- for 'codegen' ---

default_codegeneration = {
    'dir' : '${system/dir/project/build/gen}',
    'CreatedPkgList': '${dir}/PkgList.txt',

    'cmake_create_pkg_list' : {
        'bin': 'cmake',
        'type': '${project/config}',
        'args': '-Ddest_file="${../CreatedPkgList}" -DPCB_ProjectLabel="${project/label}" -DPCB_ProjectCfgFile="${maketool/exportcfg/file_out}" -P ${system/dir/root}/scripts/pcb/cmake/CreatePkgList/CreatePkgList.cmake',
        'call': '${bin} ${args}',
    },
}


# --- for 'reports' ---

default_reports = {
    'memory' : {
        # Definitions, must be implemented by the child configuration
        'call' : '${buildtool/size/memreport/call}',
    },
}


# --- for 'tests' ---

default_tests = {
    'tbd':'tbd',
}

# --- for 'tools' ---

default_tools = {
    'create_vs2017_json': {
        # Subscript
        'subscript': {
            'script': '${system/dir/root}/scripts/pcb/py/addons/vs/vs2017.py',
            'call': '$python{script}',
        },
    },
    'strip': {
        # Subscript
        'subscript': {
            'script': '${system/dir/root}/scripts/pcb/py/addons/strip.py',
            'call': '$python{script}',
        },
    },
    'amap': {
        'bin': 'amap',
        'args': '${system/dir/project/build/bin}/${project/name}.map',
        'call': '${bin} ${args}',
    },
}




