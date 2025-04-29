# Will be called by toolcaller via the project's configuration
# System class is passed, to get the project to get access to the projects's configuration


import os, shutil, sys, re, json

class MyClass:
    def __init__(self, lcSystem):
        self.mcSystem  = lcSystem
        self.mcProject = self.mcSystem.Project_Get()

        self.lbDebug = True

    def run(self, lszParameter):

        # Get the data from the configuration
        dir_root =        self.mcProject.isKeyOrDie("system/dir/root")
        dir_cfg_project = os.path.join(self.mcProject.isKeyOrDie("system/dir/project/build/gen"), "EmBitz")

        lszProjectName = self.mcProject.GetFullLabel()
        self.debug(lszProjectName)

        lszEmBitzFile = os.path.join(dir_cfg_project, "project.ebp")
        lszEmBitz_build_output        = self.mcProject.isKeyOrDie("system/file/elf")
        lszEmBitz_build_object_output = self.mcProject.isKeyOrDie("system/dir/project/build/bin")
        lszEmBitz_Unit_filename       = self.mcProject.isKeyOrDie("system/file/elf")
        lszEmBitz_debug_interface_svd = self.mcProject.isKeyOrDie("system/project/target/file_svd")

        self.mcSystem.mcFile.CreateDirForFile(lszEmBitzFile)

        self.debug("EmBitzFile:          " + lszEmBitzFile)
        self.debug("Build Output:        " + lszEmBitz_build_output)
        self.debug("Build Object Output: " + lszEmBitz_build_object_output)
        self.debug("Unit Filename:       " + lszEmBitz_Unit_filename)
        self.debug("Debug Interface svd  " + lszEmBitz_debug_interface_svd)

        # Create the file in a string to be compareable with a existing file
        lszEmBitzPrj = ""
        lszEmBitzPrj += '<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>\n'
        lszEmBitzPrj += '<EmBitz_project_file>\n'
        lszEmBitzPrj += '    <EmBitzVersion release="2.41" revision="0" />\n'
        lszEmBitzPrj += '    <FileVersion major="2" minor="0" />\n'
        lszEmBitzPrj += '    <Project>\n'
        lszEmBitzPrj += '        <Option title="project" />\n'
        lszEmBitzPrj += '        <Option pch_mode="2" />\n'
        lszEmBitzPrj += '        <Option compiler="armgcc_eb" />\n'
        lszEmBitzPrj += '        <Build>\n'
        lszEmBitzPrj += '            <Target title="Debug">\n'
        lszEmBitzPrj += '                <Option output="' + lszEmBitz_build_output + '" />\n'
        lszEmBitzPrj += '                <Option object_output="' + lszEmBitz_build_object_output + '" />\n'
        lszEmBitzPrj += '                <Option type="0" />\n'
        lszEmBitzPrj += '                <Option compiler="armgcc_eb" />\n'
        lszEmBitzPrj += '                <Option projectDeviceOptionsRelation="0" />\n'
        lszEmBitzPrj += '                <Compiler>\n'
        lszEmBitzPrj += '                    <Add option="-Wall" />\n'
        lszEmBitzPrj += '                    <Add option="-fdata-sections" />\n'
        lszEmBitzPrj += '                    <Add option="-ffunction-sections" />\n'
        lszEmBitzPrj += '                    <Add option="-O0" />\n'
        lszEmBitzPrj += '                    <Add option="-g3" />\n'
        lszEmBitzPrj += '                </Compiler>\n'
        lszEmBitzPrj += '                <Cpp>\n'
        lszEmBitzPrj += '                   <Add option="-Wall" />\n'
        lszEmBitzPrj += '                    <Add option="-fdata-sections" />\n'
        lszEmBitzPrj += '                    <Add option="-ffunction-sections" />\n'
        lszEmBitzPrj += '                    <Add option="-O0" />\n'
        lszEmBitzPrj += '                   <Add option="-g3" />\n'
        lszEmBitzPrj += '                </Cpp>\n'
        lszEmBitzPrj += '                <Assembler>\n'
        lszEmBitzPrj += '                    <Add option="-Wa,--gdwarf-2" />\n'
        lszEmBitzPrj += '                </Assembler>\n'
        lszEmBitzPrj += '               <Linker>\n'
        lszEmBitzPrj += '                    <Add option="-Wl,--gc-sections" />\n'
        lszEmBitzPrj += '                </Linker>\n'
        lszEmBitzPrj += '            </Target>\n'
        lszEmBitzPrj += '        </Build>\n'
        lszEmBitzPrj += '        <Device>\n'
        lszEmBitzPrj += '            <Add option="$device=cortex-m4" />\n'
        lszEmBitzPrj += '            <Add option="$lscript=./stm32f303cc_flash.ld" />\n'
        lszEmBitzPrj += '            <Add option="$stack=0x0800" />\n'
        lszEmBitzPrj += '            <Add option="$heap=0x4000" />\n'
        lszEmBitzPrj += '        </Device>\n'
        lszEmBitzPrj += '        <Compiler>\n'
        lszEmBitzPrj += '            <Add option="-mfloat-abi=soft" />\n'
        lszEmBitzPrj += '            <Add option="-O0" />\n'
        lszEmBitzPrj += '            <Add option="-fno-strict-aliasing" />\n'
        lszEmBitzPrj += '            <Add symbol="STM32F303CC" />\n'
        lszEmBitzPrj += '            <Add symbol="STM32F30X" />\n'
        lszEmBitzPrj += '            <Add directory="." />\n'
        lszEmBitzPrj += '        </Compiler>\n'
        lszEmBitzPrj += '        <Cpp>\n'
        lszEmBitzPrj += '            <Add option="-O1" />\n'
        lszEmBitzPrj += '        </Cpp>\n'
        lszEmBitzPrj += '        <Linker>\n'
        lszEmBitzPrj += '            <Add option="-eb_lib=n" />\n'
        lszEmBitzPrj += '            <Add option="-lstdc++" />\n'
        lszEmBitzPrj += '            <Add option="-eb_start_files" />\n'
        lszEmBitzPrj += '            <Add option="-Wl,--gc-sections" />\n'
        lszEmBitzPrj += '        </Linker>\n'
        lszEmBitzPrj += '        <Unit filename="' + lszEmBitz_Unit_filename + '" />\n'
        lszEmBitzPrj += '        <Extensions>\n'
        lszEmBitzPrj += '            <code_completion />\n'
        lszEmBitzPrj += '            <debugger>\n'
        lszEmBitzPrj += '                <target_debugging_settings target="Debug">\n'
        lszEmBitzPrj += '                    <debug_interface description="' + lszEmBitz_debug_interface_svd + '" options="3" reg_filter="0" probe="STlink" device="auto" speed="" bus_address="" serial="" defines="" scripts="" flash_size="" ram_size="" vcc_voltage="" verbose="4" unwind="2" rstType="0" use_jtag="false" disable_cache="false" enable_semi="false" close_eblink="true" gdb_reset="" gdb_reset_remote="reset;" gdb_before_conn="" gdb_after_conn="" use_remote="false" ip_address="localhost" ip_port="4242" path="${EMBITZ}\share\contrib" executable="STLinkGDB.exe" start_server="false" arguments="" flash_verify="false" flash_run="false" />\n'
        lszEmBitzPrj += '                </target_debugging_settings>\n'
        lszEmBitzPrj += '            </debugger>\n'
        lszEmBitzPrj += '            <envvars />\n'
        lszEmBitzPrj += '            <DoxyBlocks>\n'
        lszEmBitzPrj += '                <comment_style block="0" line="0" />\n'
        lszEmBitzPrj += '                <doxyfile_project />\n'
        lszEmBitzPrj += '                <doxyfile_build />\n'
        lszEmBitzPrj += '                <doxyfile_warnings />\n'
        lszEmBitzPrj += '                <doxyfile_output />\n'
        lszEmBitzPrj += '                <doxyfile_dot />\n'
        lszEmBitzPrj += '                <general />\n'
        lszEmBitzPrj += '            </DoxyBlocks>\n'
        lszEmBitzPrj += '        </Extensions>\n'
        lszEmBitzPrj += '    </Project>\n'
        lszEmBitzPrj += '</EmBitz_project_file>\n'


        lbWrite =True
        # Check if it is already existing
        if (os.path.exists(os.path.join(lszEmBitzFile))):
            with open(os.path.join(lszEmBitzFile), "r") as loExistingGenList:
                lbWrite = False
                #lszExistingGenList = loExistingGenList.read()
                #if (lszExistingGenList != lszEmBitzPrj):
                    #lbWrite = True
        else:
            lbWrite = True

        if (lbWrite == True):
            with open(os.path.join(lszEmBitzFile), "w") as text_file:
                text_file.write(lszEmBitzPrj)




    def write(self, lszString):
        self.mcSystem.write(lszString)

    def debug(self, lszString):
        if (self.lbDebug == True):
            self.writeln(lszString)


    def writeln(self, lszString):
        self.mcSystem.writeln(lszString)

    def error(self, lszPrompt, liErrorNumber, lszErrorText, end='\n', lbAddTraceback = False):
        self.mcSystem.error(lszPrompt, liErrorNumber, lszErrorText, end, lbAddTraceback)

    def warn(self, lszPrompt, liWarnNumber, lszWarnText):
        self.mcSystem.warn(lszPrompt, liWarnNumber, lszWarnText)

