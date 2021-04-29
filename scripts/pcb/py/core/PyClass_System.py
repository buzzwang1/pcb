#!/usr/local/bin/python
#    Filename : PyClass_System.py
# Description : Base Class for the System


import os
import sys
import re
import copy
import glob

from xml.etree.ElementTree import ElementTree

from PyClass_FileStatus import tcFileStatus
from PyClass_FileHandler  import tcFileHandler
from PyClass_Log import tcLog
from PyClass_ErrorHandler import tcErrorHandler

from PyClass_Projects import tcProject
from PyClass_Projects import tcProjects
from PyClass_Cmd import tcCmd

class tcSystem:
    def __init__(self, lszRootFolder, lszSourceFolder, lszBuildFolder, lszToolsFolder, lcLog = None, lcErrHdl = None):

        # Create Log File, Error Handler and File Handler
        self.mcLog = lcLog
        self.mcErrHdl = lcErrHdl

        self.mboValid  = True

        self.mszRootFolder = lszRootFolder
        self.mszSourceFolder = lszSourceFolder
        self.mszBuildFolder = lszBuildFolder
        self.mszToolsFolder = lszToolsFolder
        
        self.mcCmd = tcCmd(self.mcLog, self.mcErrHdl)
        self.mcFile = tcFileHandler(self.mcLog, self.mcErrHdl)
        self.mcConanBuildInfo = None
        self.mbSubScripts = False


        # check the version
        # check python version for print flush keyword support
        MIN_PYTHON = (3, 3)
        self.writeln("Python version check: " + str(sys.version_info))
        if (sys.version_info < MIN_PYTHON):
            self.mboValid  = False
            self.error("", 1, "Python %s.%s or later is required.\n" % MIN_PYTHON)
            self.mboValid  = False

        if (self.mboValid):
            self.writeln("Load projects")
            self.mcProjects = None
            self.mDctProjectConfigs = None
            try:
                lszProjectsConfigFolder = os.path.realpath(os.path.join(self.mszRootFolder, "cfg"))
                sys.path.insert(0, lszProjectsConfigFolder)

                # Load Projects
                # https://hackernoon.com/4-ways-to-manage-the-configuration-in-python-4623049e841b
                import ProjectList
                self.mcProjects = tcProjects(ProjectList.projects, self, self.mcLog, self.mcErrHdl)
                self.mDctProjectConfigs = ProjectList.configs
            except:
                self.error("", 1, "Failed to load projects\n", "\n", True)
                self.mboValid  = False

        self.mcProject    = None

    def Call(self, lszExec, lszPath = ""):
        return self.mcCmd.Call(lszExec, lszPath)


    def CreatePackageGenLists(self, lcProject):
        cmd = lcProject.isKeyOrDie("codegen/cmake_create_pkg_list/call")
        if (self.Call(cmd, "") == 0):
            lszTresos_PkgList = lcProject.isKeyOrDie("codegen/CreatedPkgList")

            from PyClass_Packages import tcPackages
            lcPackages = tcPackages(self.mszSourceFolder, self, self.mcLog, self.mcErrHdl)
            lcPackages.LoadPackageList(lszTresos_PkgList)
            lcPackages.CreateGenLists()


    # Project - Handling

    def Projects_GetListOfGeneratedProjects(self, lszProjectsFilter = "*") ->tcProjects:
        if (self.mboValid):
            # Make a copy
            # Need to be implemented
            lcProjects = copy.deepcopy(self.mcProjects)
            return lcProjects
        return None

    def Projects_GetListOfBuildProjects(self, lszProjectsFilter = "*") ->tcProjects:
        if (self.mboValid):
            # Make a copy
            lcProjects = copy.deepcopy(self.mcProjects)

            # Parse all projects and check
            #   - if the elf file is existing
            #   - if system config == project config
            # if not remove the configuration from the selection
            lLstProjectLabels = lcProjects.GetListOfLabelsMatchedProjects(lszProjectsFilter)

            for lszProjectFullLabel in lLstProjectLabels:
                lcProject = self.Project_Load(lszProjectFullLabel)
                if (lcProject != None):
                    lbRemoveCfg = False

                    lszElfFile = lcProject["system/file/elf"]
                    if not os.path.exists(lszElfFile):
                        lbRemoveCfg = True

                    lszSysCfg = lcProject["system/config"].strip()
                    lszVarCfg = lcProject["project/config"].strip()
                    if (lszSysCfg != lszVarCfg):
                        lbRemoveCfg = True

                    if (lbRemoveCfg == True):
                        lszProjectLabel = lcProject.GetLabel()
                        lcProjects.mDctProjects[lszProjectLabel]['configs'].pop(lszVarCfg, None)

                        # if no configs left, disable the project
                        if (len(lcProjects.mDctProjects[lszProjectLabel]['configs']) == 0):
                            lcProjects.mDctProjects[lszProjectLabel]['enable'] = False

            return lcProjects
        return None

    # SelectionMode :
    # all:          All projects, match the project
    # first:        First project, match the project
    # all_build:    All build projects, match the project
    # all_gen:      All generated projects, match the project
    # first_build:  First build project, match the project
    # first_gen:    First generated project, match the project
    # select_all:   User can select from all projects
    # select_build: User can select from build projects
    # select_gen:   User can select from generated projects

    def Projects_GetSelectionList(self, lszSelectionMode = "", lszProjectsFilter = "*") ->list:
        lszSelectionMode = lszSelectionMode.lower()

        lbUserSelect = False
        lbFirst = False

        if lszSelectionMode.startswith("select"):
            lbUserSelect = True

        if lszSelectionMode.startswith("first"):
            lbFirst = True

        if (lszSelectionMode.find("build") >= 0):
            lcProjects = self.Projects_GetListOfBuildProjects(lszProjectsFilter)
        elif (lszSelectionMode.find("gen") >= 0):
            lcProjects = self.Projects_GetListOfGeneratedProjects(lszProjectsFilter)
        else:
            lcProjects = self.mcProjects

        lLstProjects = lcProjects.GetListOfLabelsMatchedProjects(lszProjectsFilter)

        if lbUserSelect:
            lLstUserSelectedProject = list()

            # Only one for selection, take is direct
            if (len(lLstProjects) == 1):
                self.writeln("Only one project fits. Autoselect")
                lLstUserSelectedProject.append(lLstProjects[0])
            elif (len(lLstProjects) > 1):
                # more then one, show the projects
                self.writeln(lcProjects.toString(lszProjectsFilter))

                try:
                    lszUserSelection = input("Please select a project: ").strip()
                    try:
                        # Try to convert to int and adress it
                        liUserSelection = int(lszUserSelection) - 1
                        try:
                            lLstUserSelectedProject.append(lLstProjects[liUserSelection])
                        except:
                            pass
                    except ValueError:
                        # That's not an int
                        # Get the first matching project
                        lszProjectLabel = lcProjects.FindFirst(lszUserSelection)
                        if (lszProjectLabel != ""):
                            lLstUserSelectedProject.append(lszProjectLabel)
                    except:
                        pass
                except:
                    pass
            lLstProjects = lLstUserSelectedProject

        elif lbFirst:
            lLstFirstMatchedProject = list()
            # Get the first matching project
            lszProjectLabel = lcProjects.FindFirst(lszProjectsFilter)
            if (lszProjectLabel != ""):
                lLstFirstMatchedProject.append(lszProjectLabel)
            lLstProjects = lLstFirstMatchedProject


        return lLstProjects

    def Projects_List(self, lszSelectionMode = "", lszProjectsFilter = "*"):
        lszSelectionMode = lszSelectionMode.lower()
        if (lszSelectionMode == "build"):
            lcProjects = self.GetListOfBuildProjects(lszSelectionMode, lszProjectsFilter)
        elif (lszSelectionMode == "gen"):
            lcProjects = self.GetListOfGeneratedProjects(lszSelectionMode, lszProjectsFilter)
        elif (lszSelectionMode == "build_select"):
            lcProjects = self.GetListOfBuildProjects(lszSelectionMode, lszProjectsFilter)
        elif (lszSelectionMode == "gen_select"):
            lcProjects = self.GetListOfGeneratedProjects(lszSelectionMode, lszProjectsFilter)
        elif (lszSelectionMode == "select"):
            lcProjects = self.mcProjects
        else:
            lcProjects = self.mcProjects
        self.writeln(lcProjects.toString(lszProjectsFilter))

    # If project was build a CMakeCache.txt exists in hthe build-folder
    # Try to open it and read out some build/CMake information
    def Projects_Get_BuildInfo(self, lcProject):
        dir_build_project = lcProject["system/dir/project/build/cmake"]
        lszFileCMakeCache = os.path.join(dir_build_project, "CMakeCache.txt")
        if (os.path.exists(lszFileCMakeCache)):
            # --- read back values from CMakeCache.txt ---
            # this values must be set by the cmakelist.txt
            dir_compiler = ""
            elf_file = ""
            dir_debugger = ""
            dir_build_project_bin = ""
            cmake_system_cfg = ""
            file = open(lszFileCMakeCache).readlines()
            for lines in file:
                if lines.startswith("PCB_CompilerPath:"):  dir_compiler = lines.split("=")[-1].strip()
                if lines.startswith("PCB_DebuggerPath:"): dir_debugger = lines.split("=")[-1].strip()
                if lines.startswith("PCB_BinaryFilename:"): elf_file = lines.split("=")[-1].strip()
                if lines.startswith("EXECUTABLE_OUTPUT_PATH:"): dir_build_project_bin = lines.split("=")[-1].strip()
                if lines.startswith("PCB_ProjectCfg:"): cmake_system_cfg = lines.split("=")[-1].strip()

            if (dir_compiler == ""):
                self.warn("", 1, "Link to Compiler-Tool (PCB_CompilerPath) is missing in " + lszFileCMakeCache)
            if (dir_debugger == ""):
                self.warn("", 1, "Link to Debug-Tool (PCB_DebuggerPath) is missing in " + lszFileCMakeCache)
            if (elf_file == ""):
                self.warn("", 1, "Binary filename (PCB_BinaryFilename) is missing in " + lszFileCMakeCache)
            if (dir_build_project_bin == ""):
                self.warn("", 1, "Binary directory (EXECUTABLE_OUTPUT_PATH) is missing in " + lszFileCMakeCache)
            if (cmake_system_cfg == ""):
                self.warn("", 1, "System configuration (PCB_ProjectCfg) is missing in " + lszFileCMakeCache)


            # ELF file to debug
            elf_file = os.path.join(dir_build_project_bin, elf_file)

            # replace the placeholders
            lcProject["system/dir/debugger"]          = dir_debugger
            lcProject["system/dir/compiler"]          = dir_compiler
            lcProject["system/dir/project/build/bin"] = dir_build_project_bin
            lcProject["system/file/elf"]              = elf_file
            lcProject["system/config"]                = cmake_system_cfg

    def Project_ShowConfig(self, lcProject):
        # Configuration of the Project is already loaded by the toolcaller
        # Therefore, just output the configuration
        if (lcProject != None):
            self.write(lcProject.GetFullLabel() + "'s config:\n" + lcProject.toStringConfig() + "\n\n")

    def Project_ExportConfigCm(self, lcProject):
        # Export the configuration to the given file located by [maketool/exportcfg/file_out]
        if (lcProject != None):
            lszExportFile = lcProject.isKeyOrDie("maketool/exportcfg/file_out")
            lcProject.Export_CMake(lszExportFile)


    def Project_Load(self, lszFullLabel) ->tcProject:
        self.writeln("Load configuration of " + lszFullLabel)
        lcProject = None
        try:
            # Load the configuration for the projects from the SysVarList.configs
            lcProject = self.mcProjects.LoadProject(lszFullLabel, self.mDctProjectConfigs)

            # replace the placeholders in the SysVarList
            lcProject["system/dir/root"]  = str(lcProject["system/dir/root"]).replace("[dir_root]", self.mszRootFolder)
            lcProject["system/dir/source"]  = str(lcProject["system/dir/source"]).replace("[dir_source]", self.mszSourceFolder)
            lcProject["system/dir/build"] = str(lcProject["system/dir/build"]).replace("[dir_build]", self.mszBuildFolder)
            lcProject["system/dir/tools"] = str(lcProject["system/dir/tools"]).replace("[dir_tools]", self.mszToolsFolder)

            # Join the build folder of the build project
            dir_build_project = os.path.join(self.mszBuildFolder, lcProject.GetName())
            lcProject["system/dir/project/build/root"] = str(lcProject["system/dir/project/build/root"]).replace("[dir_build_project]", dir_build_project)

            # Try to get additional CMake Cache Data
            self.Projects_Get_BuildInfo(lcProject)
        except:
            self.error("", 0, "Could not load configuration", '\n', True)
            pass

        self.mcProject = lcProject
        return lcProject

    def Project_Get(self) ->tcProject:
        return self.mcProject


    # Activated(True) /Deactivate(False) the execution of subscripts
    def SubScripts(self, lbState):
        self.mbSubScripts = lbState

    def isValid(self) ->bool:
        return self.mboValid

    def write(self, lszString):
        if (self.mcLog != None):
            self.mcLog.write(lszString)
        else:
            print(lszString, end='')

    def writeln(self, lszString):
        if (self.mcLog != None):
            self.mcLog.writeln(lszString)
        else:
            print(lszString)

    def error(self, lszPrompt, liErrorNumber, lszErrorText, end='\n', lbAddTraceback = False):
        if (self.mcErrHdl != None):
            self.mcErrHdl.error(lszPrompt, liErrorNumber, lszErrorText, end, lbAddTraceback)
        else:
            print(lszPrompt + "Error " + str(liErrorNumber) + ": " + lszErrorText)

    def warn(self, lszPrompt, liWarnNumber, lszWarnText):
        if (self.mcErrHdl != None):
            self.mcErrHdl.warn(lszPrompt, liWarnNumber, lszWarnText)
        else:
            print(lszPrompt + "Warning " + str(liWarnNumber) + ": " + lszWarnText)


#Test
if __name__ == "__main__":
    isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ
    if isRunningInPyCharm:
        dir_root  = os.path.dirname(os.path.abspath(os.path.join(__file__, "../../..")))
        dir_source  = os.path.abspath(os.path.join(dir_root, "source"))
        dir_build = os.path.abspath(os.path.join(dir_root, "bin"))

        lcSys = tcSystem(dir_root, dir_source, dir_build)
        print(lcSys.mcProjects.toString())


