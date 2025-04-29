#!/usr/bin/python

"""
  toolcaller.py: Calls the given commands based on the given/available project in the given folders
        press -h for help
"""

import os
import os.path
import sys
import copy

def Commands_Get() ->dict:
    lDctCmds = {"Projects/Packages/UpdateGenLists" : "Creates/Updates the genlists.cmake of the project's packages",
                "Projects/List" : "                   Shows all projects",
                "Projects/ShowCfg" : "                Shows the project's configurations",
                "Projects/ExportCfgCm" : "            Exports the project's configurations to CMake\n",
                }
    return lDctCmds

def Commands_toString(lszIdent = ""):
    lDctCmds = Commands_Get()
    for lszCmd, lszCmdText in lDctCmds.items():
        print(lszIdent + lszCmd + ": " + lszCmdText)


def print_help(lszStr = ""):
    if (lszStr != ""):
        print(lszStr)
    print("-project=      " + "[project]: any valid project direct by name")
    print("               " + "'*' can be used to match projects\n")
    print("-selectionmode=" + "all:          All projects, match the project")
    print("               " + "first:        First project, match the project")
    print("               " + "select_all:   User can select from all projects")
    #print("               " + "all_gen:      All generated projects, match the project") # FLKa: Not Implemented
    #print("               " + "first_gen:    First generated project, match the project") # FLKa: Not Implemented
    #print("               " + "select_gen:   User can select from generated projects") # FLKa: Not Implemented
    print("               " + "all_build:    All build projects, match the project")
    print("               " + "first_build:  First build project, match the project")
    print("               " + "select_build: User can select from build projects\n")
    print("-cmds=         " + "Commands from SysVarList, that should be called.\n" +
          "               "   "Multiple commands can be separated by \";\"\n\n" +
          "               " + "Hardcoded commands:")
    Commands_toString("               ")
    print("-root_dir=     " + "Location of the root folder. Absolute or relative to the command line path")
    print("-build_dir=    " + "Location of the build folder. Absolute or relative to the root path")
    print("-source_dir=   " + "Location of the source folder. Absolute or relative to the root path\n")
    print("-tools_dir=    " + "Location of the tools folder. Absolute or relative to the root path\n")
    print("-h             " + "Shows help")
    sys.exit(1)

def toolcaller(lszSelectionMode, lszProject, lszCmds, dir_root, dir_build, dir_source, dir_tools):

    lszPythonPath = os.path.abspath(os.path.join(dir_root, "scripts", "pcb", "py", "core"))

    # Load the scripts.
    # Note: tcSystem is some case of base class to access the other classes. e.g. the class for projects handling (tcProjects)
    sys.path.insert(0, lszPythonPath)
    from PyClass_System import tcSystem
    from PyClass_Log import tcLog
    from PyClass_ErrorHandler import tcErrorHandler
    from PyClass_FileHandler import tcFileHandler
    from PyClass_Cmd import tcCmd


    lcLog = tcLog(None, "w")
    lcErr = tcErrorHandler(lcLog)

    lszThisFile = os.path.split(__file__)[1]


    lcSystem = tcSystem(dir_root, dir_source, dir_build, dir_tools, lcLog, lcErr)
    lcCmd = tcCmd(lcLog, lcErr)

    if not lcSystem.isValid():
        sys.exit(1)

    lLstProjects = lcSystem.Projects_GetSelectionList(lszSelectionMode , lszProject)

    if (lLstProjects == None):
        lcErr.error(lszThisFile + ": ", 1, "No project fits/given")
        sys.exit(1)
    else:
        if len(lLstProjects) == 0:
            lcErr.error(lszThisFile + ": ", 1, "No project fits/given")
            sys.exit(1)

    lcLog.writeln("\nStart processing : " + str(lLstProjects))
    lcLog.writeln("")

    # Make a copy of the logs
    lcLogCopy = copy.deepcopy(lcLog)

    # Process all projects
    for lszProjectLabel in lLstProjects:


        lcProject = lcSystem.Project_Load(lszProjectLabel)

        if (lcProject == None):
            lcErr.error("", 1, "Failed to load project's configuration\n")
        else:
            # Create the build/[project] folder to write the log file
            lcFileHandler = tcFileHandler(lcLog, lcErr)
            lcFileHandler.CreateDir(lcProject['system/dir/project/build/root'])
            lcLogPre = copy.deepcopy(lcLogCopy)
            lcLogPre.open(os.path.join(lcProject['system/dir/project/build/root'], lcProject.GetName() + "_log.txt"), "a")
            lcLogPre.close()
            lcLog.open(os.path.join(lcProject['system/dir/project/build/root'], lcProject.GetName() + "_log.txt"), "a")

            # Check whether all the given commands are defined in the project,
            # otherwise do nothing and exit unsuccessfully.
            lcLog.write("Check given commands: ")
            lDctCmds = Commands_Get()
            lLstCmds = lszCmds.split(";")
            for lszCmdToCall in lLstCmds:
                lszCmdToCallLower = lszCmdToCall.lower()
                lbCmdValid = False
                # Check inside the projects configuration
                if lcProject.Exists(lszCmdToCall):
                    lbCmdValid = True
                # Check inside the hardcoded commands
                for lszKey, lszValue in lDctCmds.items():
                    if (lszKey.lower() == lszCmdToCallLower):
                        lbCmdValid = True

                if not lbCmdValid:
                    lcErr.error("\n" + lszThisFile + ": ", 1, "Given command " + lszCmdToCall + " is not defined for " + lszProjectLabel)
                    sys.exit(1)

            lcLog.writeln("OK\n")

            lcLog.write("Process given commands\n")
            for lszCmdToCall in lLstCmds:
                lcProject = lcSystem.Project_Load(lszProjectLabel)
                lcSystem.SubScripts(False)

                lcLog.writeln("\nCalling " + lszProjectLabel + "->" + lszCmdToCall + "\n", lcLog.CYAN);
                lszCmdToCallLower = lszCmdToCall.lower()

                # Hardcoded Commands
                if (lszCmdToCallLower == "Projects/Packages/UpdateGenLists".lower()):
                    # special 'hardcoded' command
                    lcLog.writeln("Starting Projects/Packages/UpdateGenLists\n")
                    lcSystem.CreatePackageGenLists(lcProject)
                    lcLog.writeln("Finished Projects/Packages/UpdateGenLists\n")

                elif (lszCmdToCallLower == "Projects/List".lower()):
                    # special 'hardcoded' command
                    lcLog.writeln("Starting Projects/List\n")
                    lcLog.writeln(lcSystem.mcProjects.toString())
                    lcLog.writeln("Finished Projects/List\n")

                elif (lszCmdToCallLower == "Projects/ShowCfg".lower()):
                    # special 'hardcoded' command
                    lcLog.writeln("Starting Projects/ShowCfg\n")
                    lcSystem.Project_ShowConfig(lcProject)
                    lcLog.writeln("Finished Projects/ShowCfg\n")

                elif (lszCmdToCallLower == "Projects/ExportCfgCm".lower()):
                    # special 'hardcoded' command
                    lcLog.writeln("Starting Projects/ExportCfgCm\n")
                    lcSystem.Project_ExportConfigCm(lcProject)
                    lcLog.writeln("Finished Projects/ExportCfgCm\n")
                else:
                    lcSystem.SubScripts(True)
                    lszProjectCmdToCall = lcProject[lszCmdToCall]
                    if (not lcProject.WasSubScriptExecuted()):
                        lcCmd.Clear()
                        if (lcCmd.Call(lszProjectCmdToCall, "") != 0):
                            # call from a SysvarList entry
                            lcErr.error(lszThisFile + ": ", 1, "Programm call returned error")
                            sys.exit(1)

    lcLog.writeln("\nFinish", lcLog.GREEN)
    sys.exit(0)

# https://stackoverflow.com/questions/29777737/how-to-check-if-python-unit-test-started-in-pycharm-or-not
isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ
if not isRunningInPyCharm:
    # Called from command line
    lszArgProject        = ""
    lszArgSelectionMode  = ""
    lszArgCmds           = ""
    lszArgRootDir        = ""
    lszArgBuildDir       = ""
    lszArgRepoDir        = ""
    lszArgToolsDir       = ""

    # Called from command line
    # check the given arguments
    for lszArgument in sys.argv:
        if (lszArgument.find("toolcaller.py")>=0):
            pass
        elif (lszArgument == "-h"):
            print_help()
        elif (lszArgument.find("-project=")==0):
            lszArgument = lszArgument[len("-project="):]
            lszArgProject = lszArgument.strip()
        elif (lszArgument.find("-selectionmode=")==0):
            lszArgument = lszArgument[len("-selectionmode="):]
            lszArgSelectionMode = lszArgument.strip()
        elif (lszArgument.find("-cmds=") == 0):
            lszArgument = lszArgument[len("-cmds="):]
            lszArgCmds = lszArgument.strip()
        elif (lszArgument.find("-root_dir=") == 0):
            lszArgument = lszArgument[len("-root_dir="):]
            lszArgRootDir = lszArgument.strip()
        elif (lszArgument.find("-build_dir=") == 0):
            lszArgument = lszArgument[len("-build_dir="):]
            lszArgBuildDir = lszArgument.strip()
        elif (lszArgument.find("-source_dir=") == 0):
            lszArgument = lszArgument[len("-source_dir="):]
            lszArgRepoDir = lszArgument.strip()
        elif (lszArgument.find("-tools_dir=") == 0):
            lszArgument = lszArgument[len("-tools_dir="):]
            lszArgToolsDir = lszArgument.strip()
        else:
            print_help(lszArgument + " invalid argument")

    # check the given arguments
    if (lszArgProject  == ""):       print_help("\"-project=\"" +       " missing")
    if (lszArgSelectionMode  == ""): print_help("\"-selectionmode=\"" + " missing")
    if (lszArgCmds == ""):           print_help("\"-cmds=\"" +          " missing")
    if (lszArgRootDir == ""):        print_help("\"-root_dir=\"" +      " missing")
    if (lszArgRepoDir == ""):        print_help("\"-source_dir=\"" +      " missing")
    if (lszArgBuildDir == ""):       print_help("\"-build_dir=\"" +     " missing")
    if (lszArgToolsDir == ""):       print_help("\"-tools_dir=\"" +     " missing")

    dir_root  = os.path.abspath(os.path.join(os.getcwd(), lszArgRootDir))
    dir_source  = os.path.abspath(os.path.join(dir_root, lszArgRepoDir))
    dir_build = os.path.abspath(os.path.join(dir_root, lszArgBuildDir))
    dir_tools = os.path.abspath(os.path.join(dir_root, lszArgToolsDir))

    if (not os.path.exists(dir_root)):
        print("root_dir is invalid/not exists: " + dir_root)
        sys.exit(1)

    if (not os.path.exists(dir_source)):
        print("source_dir is invalid/not exists: " + dir_source)
        sys.exit(1)
        
    if (not os.path.exists(dir_tools)):
        print("tools_dir is invalid/not exists: " + dir_tools)
        sys.exit(1)        

    toolcaller(lszArgSelectionMode, lszArgProject, lszArgCmds, dir_root, dir_build, dir_source, dir_tools)
else:
    # Test mode
    if __name__ == "__main__":
        print("!! Toolcaller Testmode !!")
        # This script should be located in [dir_root]/scripts
        dir_root  = os.path.dirname(os.path.abspath(os.path.join(__file__, "../")))
        dir_source  = os.path.abspath(os.path.join(dir_root, "source"))
        dir_build = os.path.abspath(os.path.join(dir_root, "bin"))
        dir_tools = os.path.abspath(os.path.join(dir_root, "../pcb_tools"))

        # Show all
        lszArgSelectionMode = "select_build"
        lszProjects = "*"
        lszCmds     = "Projects.List"

        toolcaller(lszArgSelectionMode, lszProjects, lszCmds, dir_root, dir_build, dir_source, dir_tools)



