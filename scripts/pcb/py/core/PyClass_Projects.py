#!/usr/local/bin/python
#    Filename : PyClass_Projects.py
# Description : Class to manage all of the projects


import copy # for deepcopy
import os      # Needed to set paths OS specific and to read environment variables
import fnmatch # For matching project labels
import sys

from PyClass_Log import tcLog
from PyClass_ErrorHandler import tcErrorHandler
from PyClass_FileHandler import  tcFileHandler

# tcStringCMakeParser
# Class to decode CMake-like Syntax for Text replacement
# Supported is
#    ${...}: Resolve the text in brackets
#    $path{...}: Resolve the text in brackets and converts the text to OS specific path syntax
#                e.g. '$path{/Test/abc.h}' will' be converted '\Test\abc.h' on Win OS
#    $env{...}: Will try to load the value of the given environment variable
class tcStringCMakeParser:
    def __init__(self, lszStringToParse):
        self.mszStringToParse = lszStringToParse
        self.mbFinished = bool()

        self.miParserPos = int()
        self.miPosClose = int()
        self.miPosOpen = int()
        self.miPosStart = int()

        self.mszResultCmd = str()
        self.mszResultText = str()
        self.Reset()

    def Reset(self):
        self.mbFinished = False

        self.miParserPos = 0
        self.miPosClose = -1
        self.miPosOpen = -1
        self.miPosStart = -1

        self.mszResultCmd = ""
        self.mszResultText = ""

    # Checks, if the current found positions are plausible
    def CheckPos(self) ->bool:
        if ((self.miPosStart < self.miPosOpen) and
            (self.miPosOpen < self.miPosClose) and
            (self.miPosStart >= 0) and
            (self.miPosOpen >= 0) and
            (self.miPosClose >= 0)):
            return True
        return False

    # search for '}' in $[...]{[...]}
    def FindClose(self, liPos) ->int:
        if (liPos >= 0):
            return self.mszStringToParse.find("}", liPos)
        return -1

    # search for '{' in $[...]{[...]}
    def FindOpen(self, liPos) ->int:
        if (liPos > len(self.mszStringToParse)):
            liPos = len(self.mszStringToParse)-1

        while (liPos>=0):
            if (self.mszStringToParse[liPos] == "{"):
                return liPos
            liPos -= 1
        return -1

    # search for '$' in $[...]{[...]}
    def FindStart(self, liPos) ->int:
        if (liPos > len(self.mszStringToParse)):
            liPos = len(self.mszStringToParse) - 1

        while (liPos >= 0):
            if (self.mszStringToParse[liPos] == "$"):
                return liPos
            liPos -= 1
        return -1

    # Decodes the set string(self.mszStringToParse)
    # return False, if nothing done
    #        True, when something was done and therefore probably not finished
    def Decode(self) ->bool:
        self.mszResultCmd = ""
        self.mszResultText = ""

        if (not self.mbFinished):
            self.mbFinished = True

            self.miPosClose = self.FindClose(self.miParserPos)
            self.miPosOpen = self.FindOpen(self.miPosClose)
            self.miPosStart = self.FindStart(self.miPosOpen)

            if self.CheckPos():
                self.mbFinished = False
                self.mszResultCmd = self.mszStringToParse[self.miPosStart + 1: self.miPosOpen]
                self.mszResultText = self.mszStringToParse[self.miPosOpen + 1: self.miPosClose]
                return True

        return False

    #returns the actual result
    def Get(self) ->str:
        return self.mszStringToParse

    # Replaces the last found ${...} - pattern by the given text
    # Make sure, that decode was called before
    def Replace(self, lszStringToReplace):
        if self.CheckPos():
            lszValuesNew = self.mszStringToParse[:self.miPosStart] + lszStringToReplace + self.mszStringToParse[self.miPosClose + 1:]
            self.mszStringToParse = lszValuesNew
            self.miPosClose = self.miPosStart + len(lszStringToReplace) - 1

    # Increases the internal index to parse the set string
    def UpdatePos(self):
        self.miParserPos = self.miPosClose + 1

if __name__ == "__main__":
    isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ
    if isRunningInPyCharm:
        print("\n\n----- Test tcStringCMakeParser ----\n")
        lcParser = tcStringCMakeParser("$path{fgr/gfr/fsr/abc.txt}")
        lcParser.Decode()
        print ("$path{fgr/gfr/fsr/abc.txt}"+ ": " + lcParser.mszResultCmd + " " + lcParser.mszResultText)


# tcConfig
# Class to handle a configuration given in a dictionary and sub-dictionaries
# Supported is a CMake like replacement of text, so that configuration values can be reused
#    e.g. see below
#
#   Access the values by '/' splitted keys
#   Note: relative access is also possible, like '../'
#
#
class tcConfig():
    def __init__(self, lDctDict, lcSystem = None, lcLog = None, lcErrHdl = None):
        self.mDctConfig = lDctDict
        self.msztoString = ""
        self.mcSystem = lcSystem
        self.mcLog = lcLog
        self.mcErrHdl = lcErrHdl
        self.mbScriptExecuted = False

    def CreateValue(self, lszKey, lszValue):
        try:
            mydict = self.mDctConfig
            lLstKeys = lszKey.split("/")
            for lszKey in lLstKeys:
                if lszKey in mydict:
                    if isinstance(mydict[lszKey], dict):
                        mydict = mydict[lszKey]
                else:
                    # Last element is list ?
                    if (lszKey == lLstKeys[-1]):
                        # Yes, set the value
                        mydict[lszKey] = lszValue
                    else:
                        # Create, sub dict
                        mydict[lszKey] = dict()
                        mydict = mydict[lszKey]

        except:
            pass


    def SetValue(self, lszKey, lszValue):
        if self.Exists(lszKey):
            try:
                mydict = self.mDctConfig
                for lszKey in lszKey.split("/"):
                    if lszKey in mydict:
                        if isinstance(mydict[lszKey], dict):
                            mydict = mydict[lszKey]
                        else:
                            mydict[lszKey] = lszValue

            except:
                pass
        else:
            self.CreateValue(lszKey, lszValue)

    # Note: Exists takes only absolute keys
    def Exists(self, lszKey) ->bool:
        try:
            mydict = self.mDctConfig
            for lszPos in lszKey.split("/"):
                if lszPos in mydict:
                    if isinstance(mydict[lszPos], dict):
                        mydict = mydict[lszPos]
                else:
                    return False
        except:
            pass

        return True

    # Read the value from the given Key
    # In case the key contains cmake syntax,
    # it is decoded and replaced
    # Note: This is done recursive
    def GetValue(self, lszKey, lszLocation = "", liRecursionLvl = 0):
        loReturn = None
        lszKey2 = lszKey.strip("/")

        if (liRecursionLvl == 0):
            self.mbScriptExecuted = False

        if not self.Exists(lszKey2):
            # try to resolve relative keys
            if (lszLocation != ""):
                while (lszKey2.find("//") >= 0):
                    lszKey2 = lszKey2.replace("//", "/")
                lszKey2 = lszKey2.strip("/")
                lszKey2 = self.JoinRelativePath(lszLocation, lszKey2)

        try:
            mydict = self.mDctConfig
            for lszPos in lszKey2.split("/"):
                if lszPos in mydict:
                    loReturn = mydict[lszPos]
                    if isinstance(mydict[lszPos], dict):
                        mydict = mydict[lszPos]
                else:
                    loReturn = None
                    break

        except:
            pass

        if (loReturn == None):
            return ""

        lszReturn = str(loReturn)
        lszLocation = self.JoinRelativePath(lszKey2, "../")

        # Check if a 'cmake'-like replace must be done
        lcCMakeParser = tcStringCMakeParser(lszReturn)

        while (lcCMakeParser.Decode()):
            lszCmd  = lcCMakeParser.mszResultCmd.lower()
            lszText = lcCMakeParser.mszResultText

            if (lszCmd == ""):
                lszNewText = self.GetValue(lszText, lszLocation, liRecursionLvl + 1)
                lcCMakeParser.Replace(lszNewText)

            elif ((lszCmd == "path") or (lszCmd ==  "os.path")):
                if self.Exists(lszText): # Check if key existing
                    lszNewText = self.GetValue(lszText, lszLocation, liRecursionLvl + 1)
                else:
                    # if not, try to resolve relative keys
                    lszText2 = self.JoinRelativePath(lszLocation, lszText)
                    if self.Exists(lszText2):
                        lszNewText = self.GetValue(lszText2, lszLocation, liRecursionLvl + 1)
                    else: # if not, return given unresolved key
                        lszNewText = lszText
                lszNewText = os.path.normpath(lszNewText)
                lcCMakeParser.Replace(lszNewText)

            elif (lszCmd == "slash"):
                if self.Exists(lszText): # Check if key existing
                    lszNewText = self.GetValue(lszText, lszLocation, liRecursionLvl + 1)
                else:
                    # if not, try to resolve relative keys
                    lszText2 = self.JoinRelativePath(lszLocation, lszText)
                    if self.Exists(lszText2):
                        lszNewText = self.GetValue(lszText2, lszLocation, liRecursionLvl + 1)
                    else: # if not, return given unresolved key
                        lszNewText = lszText
                lszNewText = lszNewText.replace('\\','/')
                lcCMakeParser.Replace(lszNewText)

            elif (lszCmd == "bslash"):
                if self.Exists(lszText): # Check if key existing
                    lszNewText = self.GetValue(lszText, lszLocation, liRecursionLvl + 1)
                else:
                    # if not, try to resolve relative keys
                    lszText2 = self.JoinRelativePath(lszLocation, lszText)
                    if self.Exists(lszText2):
                        lszNewText = self.GetValue(lszText2, lszLocation, liRecursionLvl + 1)
                    else: # if not, return given unresolved key
                        lszNewText = lszText
                lszNewText = lszNewText.replace('/','\\')
                lcCMakeParser.Replace(lszNewText)

            elif (lszCmd == "os.exe"):
                if self.Exists(lszText): # Check if key existing
                    lszNewText = self.GetValue(lszText, lszLocation, liRecursionLvl + 1)
                else:
                    # if not, try to resolve relative keys
                    lszText2 = self.JoinRelativePath(lszLocation, lszText)
                    if self.Exists(lszText2):
                        lszNewText = self.GetValue(lszText2, lszLocation, liRecursionLvl + 1)
                    else: # if not, return given unresolved key
                        lszNewText = lszText

                # https://stackoverflow.com/questions/1325581/how-do-i-check-if-im-running-on-windows-in-python'
                if ((sys.platform == "linux") or (sys.platform == "linux2")):
                    pass
                elif (sys.platform == "darwin"):
                    pass
                elif ((sys.platform == "win32") or (sys.platform == "win64")):
                    lszNewText += '.exe'

                lcCMakeParser.Replace(lszNewText)

            elif (lszCmd == "os.type"):
                if self.Exists(lszText): # Check if key existing
                    lszNewText = self.GetValue(lszText, lszLocation, liRecursionLvl + 1)
                else:
                    # if not, try to resolve relative keys
                    lszText2 = self.JoinRelativePath(lszLocation, lszText)
                    if self.Exists(lszText2):
                        lszNewText = self.GetValue(lszText2, lszLocation, liRecursionLvl + 1)
                    else: # if not, return given unresolved key
                        lszNewText = lszText

                if ((sys.platform == "linux") or (sys.platform == "linux2")):
                    lszNewText = 'linux'
                elif (sys.platform == "darwin"):
                    lszNewText = 'osx'
                elif ((sys.platform == "win32") or (sys.platform == "win64")):
                    lszNewText = 'windows'

                lcCMakeParser.Replace(lszNewText)


            elif (lszCmd == "python"):
                if self.Exists(lszText): # Check if key existing
                    lszNewText = self.GetValue(lszText, lszLocation, liRecursionLvl + 1)
                else:
                    # if not, try to resolve relative keys
                    lszText2 = self.JoinRelativePath(lszLocation, lszText)
                    if self.Exists(lszText2):
                        lszNewText = self.GetValue(lszText2, lszLocation, liRecursionLvl + 1)
                    else: # if not, return given unresolved key
                        lszNewText = lszText

                try:
                    if (self.mcSystem != None):
                        if (self.mcSystem.mbSubScripts == True):
                            lszNewText = lszNewText.strip()
                            # Find the python file, by finding the extention '.py'
                            liPosExtPy = lszNewText.find(".py")
                            if (liPosExtPy >= 0):
                                lszPythonDirAndFile = lszNewText[:liPosExtPy + len('.py')]
                                lszCallParameters   = lszNewText[liPosExtPy + len('.py') + 1:]

                                # split folder from the file name
                                # https://stackoverflow.com/questions/3167154/how-to-split-a-dos-path-into-its-components-in-python
                                lszPythonDir, lszPythonFile = os.path.split(lszPythonDirAndFile)

                                # Extracting extension from filename
                                # https://stackoverflow.com/questions/541390/extracting-extension-from-filename-in-python
                                lszPythonFileNoExt, lszPythonFileExt = os.path.splitext(lszPythonFile)

                                if (os.path.exists(lszPythonDirAndFile)):
                                    self.writeln("Starting user script: " + lszPythonDirAndFile + " " + lszCallParameters + "\n")

                                    # https://stackoverflow.com/questions/547829/how-to-dynamically-load-a-python-class
                                    sys.path.insert(0, lszPythonDir)
                                    loSubScript = __import__(lszPythonFileNoExt)
                                    import inspect

                                    loSubScriptClassDefinition = None
                                    for lszSubScriptName, loSubScriptObj in inspect.getmembers(loSubScript):
                                        if inspect.isclass(loSubScriptObj):
                                            loSubScriptClassDefinition = loSubScriptObj

                                    if (loSubScriptClassDefinition != None):

                                        # Save, this in case the subscript changed it
                                        lszWorkingDirSave = os.getcwd()

                                        loSubScriptClass = loSubScriptClassDefinition(self.mcSystem)
                                        loReturnValue = loSubScriptClass.run(lszCallParameters)

                                        if (lszWorkingDirSave != os.getcwd()):
                                            os.chdir(lszWorkingDirSave)

                                        self.mbScriptExecuted = True
                                        if isinstance(loReturnValue, str):
                                            lszNewText = loReturnValue
                                        else:
                                            lszNewText = ""
                                    self.writeln("\nFinished user script: " + lszPythonDirAndFile + " " + lszCallParameters + "\n")

                except:
                    self.error("", 1, "Could not run user script", '\n', True)
                    pass

                lcCMakeParser.Replace(lszNewText)


            elif (lszCmd == "env"):
                if (lszText in os.environ):
                    lszNewText = os.environ[lszText].strip()
                    lcCMakeParser.Replace(lszNewText)

            lcCMakeParser.UpdatePos()

        return lcCMakeParser.Get()


    def __setitem__(self, lszKey, lszValue):
        self.SetValue(lszKey, lszValue)

    def __getitem__(self, lszKey):
        return self.GetValue(lszKey)

    # e.g. '/Ingreds/Fluid' + '../Solid/In1' => '/Ingreds/Solid/In1'
    #      '/Ingreds/Fluid' + 'In1' => '/Ingreds/Fluid/In1'
    def JoinRelativePath(self, lszLocation, lszPathToJoin) ->str:
        lszReturn = lszPathToJoin

        liFolderCnt = 0
        lbFinished = False
        liStartIndex = 0

        lszStr = lszPathToJoin

        # Count number of "../"
        while not lbFinished:
            lbFinished = True
            liPosStart = lszStr.find("../", liStartIndex)
            if (liPosStart >= 0):
                liFolderCnt += 1
                lbFinished = False
                lszStr = lszStr[liPosStart + len("../"):]

        # Trace back the number of "../" in the location
        if (liFolderCnt > 0):
            liStartIndex = len(lszLocation)

            while (liFolderCnt > 0) and (liStartIndex >= 0):
                liStartIndex = lszLocation.rfind("/", 0, liStartIndex - 1)
                liFolderCnt -= 1


            if (liStartIndex >= 0):
                lszReturn = lszLocation[:liStartIndex + 1] + lszStr
        else:
            lszReturn = lszLocation.strip('/') + "/" + lszPathToJoin

        return lszReturn

    # returns the given config in a tree
    def Print_Pretty(self, lDctWorking, location="", indent=0):
        for key, value in lDctWorking.items():
            if isinstance(value, dict):
                if (location == ""):
                    location = str(key)
                else:
                    location = location + "/" + str(key)

                self.msztoString += "  " * indent + str(key) + ":\n"
                self.Print_Pretty(value, location, indent + 1)
                location = location[:location.rfind(str(key))].strip("/")
            else:
                if (location == ""):
                    self.msztoString += "  " * indent + str(key) + " = " + self.GetValue(str(key)) + "\n"
                else:
                    self.msztoString += "  " * indent + location + "/" + str(key) + " = " + self.GetValue(location + "/" + str(key)) + "\n"

    # returns the given config cmake style
    def Print_CMake(self, lDctWorking, location="", indent=0):

        if (indent == 0):
            self.msztoString += "# Note:\n"
            self.msztoString += "#   - \\ replaced by \\\\\n"
            self.msztoString += "#   - \" replaced by ''\n"
            self.msztoString += "#   - ${ replaced by prjref{\n\n\n"

        for key, value in lDctWorking.items():
            if isinstance(value, dict):
                if (location == ""):
                    location = str(key)
                else:
                    location = location + "/" + str(key)

                self.Print_CMake(value, location, indent + 1)
                location = location[:location.rfind(str(key))].strip("/")
            else:
                if (location == ""):
                    # insert a backspace before cmake special characters
                    lszKey = self.GetValue(str(key))
                    # https://cmake.org/cmake/help/latest/manual/cmake-language.7.html
                    lszKey = lszKey.replace('\\', '\\\\')  # '\' to '\\'
                    lszKey = lszKey.replace('"', "''")  # '"' to "''"
                    lszKey = lszKey.replace('${', 'prjref{')  # '"' to '\"'
                    self.msztoString += "set(PCB_ProjCfg_"  + str(key) + ' "' + lszKey + '")\n'
                else:
                    # insert a backspace before cmake special characters
                    lszKey = self.GetValue(location + "/" + str(key))
                    # https://cmake.org/cmake/help/latest/manual/cmake-language.7.html
                    lszKey = lszKey.replace('\\', '\\\\')  # '\' to '\\'
                    lszKey = lszKey.replace('"', "''")  # '"' to "''"
                    lszKey = lszKey.replace('${', 'prjref{')  # '"' to '\"'
                    self.msztoString += "set(PCB_ProjCfg_"  + (location + "/").replace("/","_") + str(key) + ' "' + lszKey + '")\n'



    def toString(self) ->str:
        self.msztoString = ""
        self.Print_Pretty(self.mDctConfig)
        return self.msztoString

    def toCMake(self) ->str:
        self.msztoString = ""
        self.Print_CMake(self.mDctConfig)
        return self.msztoString

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



if __name__ == "__main__":
    isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ
    if isRunningInPyCharm:
        print("\n\n----- Test tcConfig ----\n")

        TestConfig1 = { # Root
           'Ingreds': { # Sub-Dict
               'Fluid': {  # Sub-Dict
                  'In1': 'water',   # Key:Value
                  'In2': 'milk',
               },
               'Solid': {  # Sub-Dict
                  'In1' : 'flour',
                  'In2' : 'salt',
                  'In3' : 'sauce',
                  'In4' : 'cheese',
                  'In5' : 'bacon',
                  'In6' : 'eggs',
                  'In7' : 'salami',
                  'In8' : 'tomoato',
               },
           },
           'Pastes': {
                'Pizza'   : '${Ingreds/Solid/In1} + ${Ingreds/Fluid/In1} + ${Ingreds/Solid/In2}',
                'Pancake' : '${Ingreds/Solid/In1} + ${Ingreds/Fluid/In2} + ${Ingreds/Solid/In2} + ${Ingreds/Solid/In6}',
           },
           'Pizzas': {
                'Magaritha' : '${Pastes/Pizza} + ${Ingreds/In3} + ${Ingreds/Solid/In8} + ${Ingreds/Solid/In4}',
                'Salami' : '${Magaritha} + ${Ingreds/Solid/In7}',
           },
           'Pancakes': {
                'Bacon'   : '${Pastes/Pancake} + ${Ingreds/Solid/In5}',
                'Special': {
                    'Bacon'   : '$path{../Bacon} + Garlic',
                }
           },
        }


        lcConfig = tcConfig(TestConfig1)
        print(lcConfig.toString())
        print("\nlcConfig['Pancakes/Special/Bacon'] = "  + lcConfig['Pancakes/Special/Bacon'] + "\n")
        print("\n\n")

        TestConfig2 = {
          'File' : 'Test.exe',
          'Path' : 'This/is/my/folder/',
          'Join' : '$path{c:/${Path}${File}}',
        }
        lcConfig = tcConfig(TestConfig2)
        print(lcConfig.toString())
        print("\n\n")


# Project class
# Load and access a project
# e.g. "rh850/bmw/gen5/BasisClar.debug
#
class tcProject():
    def __init__(self, lszFullLabel, lcSystem = None, lcLog = None, lcErrHdl = None):
        self.mszFullLabel = lszFullLabel # e.g. "rh850/bmw/gen5/BasisClar.debug
        self.mcConfig = None
        self.mDctProject = None
        self.mcSystem = lcSystem
        self.mcLog = lcLog
        self.mcErrHdl = lcErrHdl

    def GetLabel(self):
        # Check, if the label contains a configuration
        liPointPos = self.mszFullLabel.rfind(".")
        if liPointPos >= 0:
            return self.mszFullLabel[:liPointPos].strip()
        return ""

    def GetConfig(self):
        # Check, if the label contains a configuration
        liPointPos = self.mszFullLabel.rfind(".")
        if liPointPos >= 0:
            return self.mszFullLabel[liPointPos + 1:].strip()
        return ""

    def SetFullLabel(self, lszFullLabel):
        self.mszFullLabel = lszFullLabel

    def GetFullLabel(self) ->str:
        return self.mszFullLabel


    def Exists(self, lszKey) ->bool:
        if (self.mcConfig != None):
            return self.mcConfig.Exists(lszKey)
        return ""



    def SetValue(self, lszKey, lszValue):
        self[lszKey] = lszValue

    def GetValue(self, lszKey):
        return self[lszKey]

    def __setitem__(self, lszKey, lszValue):
        if (self.mcConfig != None):
            self.mcConfig[lszKey] = lszValue

    def __getitem__(self, lszKey):
        if (self.mcConfig != None):
            return self.mcConfig[lszKey]
        return ""

    def Load(self, lDctProject, lDctConfig):
        if (self.mszFullLabel != ""):
            try:
                # Merge project's data and the project's configuration data to single dict
                lszProjectLabel  = self.GetLabel()
                lszProjectConfig = self.GetConfig()

                lDctVarPlusConfig = copy.deepcopy(lDctConfig)
                lDctVarPlusConfig['project'] = copy.deepcopy(lDctProject)
                lDctVarPlusConfig['project']['label'] = lszProjectLabel
                lDctVarPlusConfig['project']['config'] = lszProjectConfig

                self.mDctProject = lDctProject
                self.mcConfig = tcConfig(lDctVarPlusConfig, self.mcSystem, self.mcLog, self.mcErrHdl)
            except:
                pass

    def Export_CMake(self, lszDestCMakeFile):
        # Create the data in a string to be compare-able with a existing file

        self.writeln("Generate project configuration file for CMake: " + lszDestCMakeFile)

        lszGenlists = "# PyClass_Projects.py: This is a generated file. Do not modify it.\n"
        lszGenlists += "# Exported configuration for " + self["project/name"] + ".\n"

        lszGenlists += "\n"

        lbScriptSave = self.mcSystem.mbSubScripts
        self.mcSystem.mbSubScripts = False
        lszGenlists += self.mcConfig.toCMake()
        self.mcSystem.mbSubScripts = lbScriptSave

        try:
            lbWrite = False
            # Check if a existing file is different to the new one
            if (os.path.exists(lszDestCMakeFile)):
                with open(lszDestCMakeFile, "r") as loExistingGenList:
                    lszExistingGenList = loExistingGenList.read()

                    if (lszExistingGenList != lszGenlists):
                        lbWrite = True
                    else:
                        self.writeln("  => Nothing todo")
            else:
                lbWrite = True

            if (lbWrite == True):
                lcFh = tcFileHandler(self.mcLog, self.mcErrHdl)
                lcFh.CreateDirForFile(lszDestCMakeFile)

                with open(lszDestCMakeFile, "w") as text_file:
                    text_file.write(lszGenlists)
                    self.writeln("  => Ok")
        except:
            self.error("", 1, "Failed", '\n', True)
            pass


    def Enable(self):
        try:
            self.mDctProject['enable'] = True
        except:
            pass
    def Disable(self):
        try:
            self.mDctProject['enable'] = False
        except:
            pass

    def IsEnable(self) ->bool:
        try:
            return self.mDctProject['enable']
        except:
            pass
        return False

    def GetName(self):
        try:
            return self.mDctProject['name']
        except:
            pass
        return False

    def WasSubScriptExecuted(self) -> bool:
        return self.mcConfig.mbScriptExecuted


    def toStringConfig(self)->str:
        if (self.mcConfig == None):
            return ""
        else:
            return self.mcConfig.toString()

    def toString(self):
        lsztoString = ""
        if (self.mszFullLabel != ""):
            lsztoString  = self.mszFullLabel + ":\n"
            lsztoString += self.toStringConfig()
        return lsztoString

    # Helper function: Check, if the given Key is set in the configuration
    # returns it when exists, else exit(1)
    def isKeyOrDie(self, lszKey) -> str:
        lszValue = str(self.GetValue(lszKey)).strip()
        if (lszValue != ""):
            return lszValue
        else:
            self.error("", 1, "'"+ lszKey + "' is not set in the configuration of " + self.mszFullLabel + "\n")
            sys.exit(1)
        return ""

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

class tcProjects():
    def __init__(self, lDctProjects, lcSystem = None, lcLog = None, lcErrHdl = None):
        self.miPrint_ProjectsCnt = 0
        self.mDctProjects = lDctProjects # List of projects
        self.mszMatchedProjectLabel = ""
        self.mcSystem = lcSystem
        self.mcLog = lcLog
        self.mcErrHdl = lcErrHdl

    def FindFirst(self, lszMatchString) ->str:
        try:
            # Scan all projects, use the match
            for lszProjectLabel, loProjectData in self.mDctProjects.items():
                if (loProjectData['enable'] == True):
                    if self.isMatch(lszProjectLabel, lszMatchString):
                        return self.mszMatchedProjectLabel
        except:
            pass
        return ""

    def Exists(self, lszMatchString) ->bool:
        if (self.FindFirst(lszMatchString) != ""):
            return True
        return False

    def isMatchConfig(self, lszProjectLabel, lszMatchString = "*.*") ->bool:
        try:
            #Check, if the label contains a configuration
            lszOption = ""
            lszMatchConfig = "*"
            liPointPos = lszProjectLabel.rfind(".")
            if liPointPos >= 0:
                lszOption = lszProjectLabel[liPointPos + 1:].strip()

            liPointPos = lszMatchString.rfind(".")
            if liPointPos >= 0:
                lszMatchConfig = lszMatchString[liPointPos + 1:].strip()


            if fnmatch.fnmatch(lszOption, lszMatchConfig):
                return True
        except:
            pass

        return False

    def isMatch(self, lszProjectLabel, lszMatchString = "*") ->bool:
        self.mszMatchedProjectLabel = ""
        try:
            #Check, if the label contains a configuration
            lszMatchConfig = ""
            liPointPos = lszMatchString.rfind(".")
            if liPointPos >= 0:
                lszMatchConfig = lszMatchString[liPointPos + 1:].strip()
                lszMatchString = lszMatchString[:liPointPos].strip()


            lLstKeys = lszProjectLabel.split("/")
            lLstKeys = [x.lower() for x in lLstKeys]

            lbMatchProject = False
            lbMatchProjectOption = True
            if fnmatch.fnmatch(lszProjectLabel, lszMatchString) or (lszMatchString.lower() in lLstKeys):
                lbMatchProject = True

                self.mszMatchedProjectLabel = lszProjectLabel

                if (lszMatchConfig != ""):
                    lbMatchProjectOption = False
                    for lszProjectsOptionKey, lszProjectsOptionValue in self.mDctProjects[lszProjectLabel]['configs'].items():
                        if fnmatch.fnmatch(lszProjectsOptionKey, lszMatchConfig):
                            lbMatchProjectOption = True
                            self.mszMatchedProjectLabel += "." + lszProjectsOptionKey
                            break
                else:
                    for lszProjectsOptionKey, lszProjectsOptionValue in self.mDctProjects[lszProjectLabel]['configs'].items():
                        self.mszMatchedProjectLabel += "." + lszProjectsOptionKey
                        break

            return lbMatchProject and lbMatchProjectOption
        except:
            pass
        return False

    def GetListMatchedProjects(self, lszMatchString = "*") ->list:
        lLstMatchedProjects = list()

        try:
            # Use the match
            for lszProjectLabel, loProjectData in self.mDctProjects.items():
                if (loProjectData['enable'] == True):
                    if self.isMatch(lszProjectLabel, lszMatchString):
                        lLstMatchedProjects.append(lszProjectLabel)
        except:
            pass
        return lLstMatchedProjects

    def GetListOfLabelsMatchedProjects(self, lszMatchString = "*") ->list:
        lLstMatchedProjects = list()

        try:
            # Use the match
            lLstMatchedProjectsNoFullLabel = self.GetListMatchedProjects(lszMatchString)
            for lszProjectLabel in lLstMatchedProjectsNoFullLabel:
                lDctOptions = self.mDctProjects[lszProjectLabel]['configs']
                for lszProjectsOptionKey, lszProjectsOptionValue in lDctOptions.items():
                    if self.isMatchConfig(lszProjectLabel + "." + lszProjectsOptionKey, lszMatchString):
                        lLstMatchedProjects.append(lszProjectLabel + "." + lszProjectsOptionKey)
        except:
            pass
        return lLstMatchedProjects


    def ProjectsLabels2Dict(self, lszMatchString = "*")->dict:

        # Convert project label string to a tree
        lDctProjectLabelTree = dict()

        try:
            lLstMatchedProjects = self.GetListMatchedProjects(lszMatchString)
            for lszProjectLabel in lLstMatchedProjects:
                lDctDict = lDctProjectLabelTree
                lLstKeys = lszProjectLabel.split("/")
                for lszKey in lLstKeys:
                    if lszKey in lDctDict:
                        if isinstance(lDctDict[lszKey], dict):
                            lDctDict = lDctDict[lszKey]
                    else:
                        # Last element is list ?
                        if (lszKey == lLstKeys[-1]):

                            lDctOptions = self.mDctProjects[lszProjectLabel]['configs']

                            if (len(lDctOptions) > 1):
                                for lszProjectsOptionKey, lszProjectsOptionValue in lDctOptions.items():
                                    if self.isMatchConfig(lszProjectLabel + "." + lszProjectsOptionKey, lszMatchString):
                                        lDctDict[lszKey + "." + lszProjectsOptionKey] =  lszProjectLabel + "." + lszProjectsOptionKey
                            else:
                                for lszProjectsOptionKey, lszProjectsOptionValue in lDctOptions.items():
                                    if self.isMatchConfig(lszProjectLabel + "." + lszProjectsOptionKey, lszMatchString):
                                        lDctDict[lszKey] =  lszProjectLabel + "." + lszProjectsOptionKey

                        else:
                            lDctDict[lszKey] = dict()
                            lDctDict = lDctDict[lszKey]
        except:
            pass
        return lDctProjectLabelTree


    def LoadProject(self, lszFullLabel, lDctConfigs) ->tcProject:
        try:
            lszFullLabel = self.FindFirst(lszFullLabel)

            if (lszFullLabel != ""):
                lcProject = tcProject(lszFullLabel, self.mcSystem, self.mcLog, self.mcErrHdl)

                # Merge project's data and the project's configuration data to single dict
                lszProjectLabel  = lcProject.GetLabel()
                lszProjectConfig = lcProject.GetConfig()

                lDctMyProject = self.mDctProjects[lszProjectLabel]
                lszMyProjectConfig = lDctMyProject['configs'][lszProjectConfig]

                lcProject.Load(lDctMyProject, lDctConfigs[lszMyProjectConfig])

                return lcProject

        except:
            return None

    def Print_Projects(self, lDctDict, indent=0):
        for key, value in lDctDict.items():
            if isinstance(value, dict):
                self.msztoString += "  " * indent + str(key) + ":\n"
                self.Print_Projects(value, indent + 1)
            else:
                self.miPrint_ProjectsCnt += 1

                lcProject = tcProject(str(value))
                lszProjectLabel = lcProject.GetLabel()

                if len(self.mDctProjects[lszProjectLabel]['configs']) > 1:
                    self.msztoString += "{:<35}".format("  " * indent  + str(self.miPrint_ProjectsCnt) + ": " + str(key)) +  "('" + str(value) + "')" + "\n"
                else:
                    self.msztoString += "  " * indent  + str(self.miPrint_ProjectsCnt) + ": " + str(key) + "\n"


    def toString(self, lszMatchString = "*"):
        self.miPrint_ProjectsCnt = 0
        self.msztoString = ""
        self.Print_Projects(self.ProjectsLabels2Dict(lszMatchString))
        return self.msztoString

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
        MyProject = lambda name, enable, mode, configs: {'name':name, 'enable':enable, 'mode':mode, 'configs':configs}
        MyProjects = {
            #label                                name                   enable    mode   configs
            'ARM/Project1'     : MyProject('ARM_Project1',    True,     '',   {'deb' : 'Config_ARM_deb',   'rel' : 'Config_ARM_rel'}),
            'ARM/Project2'     : MyProject('ARM_Project2',    True,     '',   {'deb' : 'Config_ARM_deb',   'rel' : 'Config_ARM_rel'}),
            'rh850/Project1'   : MyProject('rh850_Project1',  True,     '',   {'deb' : 'Config_rh850_deb', 'rel' : 'Config_rh850_rel'}),
            'rh850/Project2'   : MyProject('rh850_Project2',  True,     '',   {'deb' : 'Config_rh850_deb', 'rel' : 'Config_rh850_rel'}),
            'SPC/Proj1/Var1'   : MyProject('Var1',            True,     '',   {'default' : 'Config_SPC2_default'}),
            'SPC/Proj1/Var2'   : MyProject('Var2',            True,     '',   {'default' : 'Config_SPC2_default'}),
            'SPC/Proj2/Var1'   : MyProject('Var1',            True,     '',   {'default' : 'Config_SPC2_default'}),
            'SPC/Proj2/Var2'   : MyProject('Var2',            True,     '',   {'default' : 'Config_SPC2_default'}),
        }

        my_default_system = {
            'dir' : {
                'root': '[dir_root]',
                'repo': '[dir_repo]',
                'build': '[dir_build]',
            },
            'file': {
                    'elf' : '[elf_file]',
                    'filters' : {
                        'source' : '*.c,*.cpp',
                        'asm' :    '*.s,*.800,*.asm',
                        'header' : '*.h,*.hpp',
                        'bin':     '*.elf,*.bin,*.hex,*.srec',
                    },
            },
        }

        my_default_buildtool = {
            'name' : 'needs to be set project specific',

            'target' : {
                'device_file' : 'needs to be set project specific',
            },

            'build' : {
                'type': 'debug',
            },

            'debug' : {
                'bin': '${../name}.exe',
                'connection':'log=$path{${system/dir/build}/${project/name}/debug.log}',
                'args':'-nosplash -connect="${connection}" ${system/file/elf}',
                'call': '${bin} ${args}',
            },
        }

        # rh850
        my_rh850_debug_buildtool = copy.deepcopy(my_default_buildtool) # inherit default and overwrite the release type
        my_rh850_debug_buildtool['name']                  = '[My rh850  buildtool]'
        my_rh850_debug_buildtool['target']['device_file'] = '[7403.df]'
        my_rh850_release_buildtool = copy.deepcopy(my_rh850_debug_buildtool)
        my_rh850_release_buildtool['build']['type'] = 'release'

        # ARM
        my_ARM_debug_buildtool = copy.deepcopy(my_default_buildtool) # inherit default and overwrite the release type
        my_ARM_debug_buildtool['name']                  = '[My ARM  buildtool]'
        my_ARM_debug_buildtool['target']['device_file'] = '[STM32F4.df]'
        my_ARM_release_buildtool = copy.deepcopy(my_ARM_debug_buildtool)
        my_ARM_release_buildtool['build']['type'] = 'release'

        # SPC
        my_SPC_default_buildtool = copy.deepcopy(my_default_buildtool) # inherit default and overwrite the release type
        my_SPC_default_buildtool['name']                  = '[My SPC  buildtool]'
        my_SPC_default_buildtool['target']['device_file'] = '[SPC584.df]'

        my_Config_rh850_deb    = {'system' : my_default_system, 'buildtool': my_rh850_debug_buildtool}
        my_Config_rh850_rel    = {'system' : my_default_system, 'buildtool': my_rh850_release_buildtool}
        my_Config_ARM_deb      = {'system' : my_default_system, 'buildtool': my_ARM_debug_buildtool}
        my_Config_ARM_rel      = {'system' : my_default_system, 'buildtool': my_ARM_release_buildtool}
        my_Config_SPC2_default = {'system' : my_default_system, 'buildtool': my_SPC_default_buildtool}

        myConfigs = {
            'Config_ARM_deb': my_Config_ARM_deb,
            'Config_ARM_rel': my_Config_ARM_rel,
            'Config_rh850_deb': my_Config_rh850_deb,
            'Config_rh850_rel': my_Config_rh850_rel,
            'Config_SPC2_default': my_Config_SPC2_default,
        }

        lcProjects = tcProjects(MyProjects)
        print(lcProjects.toString())
        print("\n")
        print("FindFirst('Project1'): " + lcProjects.FindFirst("Project1"))
        print("FindFirst('Project1.rel'): " + lcProjects.FindFirst("Project1.rel"))
        print("FindFirst('Proj2'): " + lcProjects.FindFirst("Proj2"))
        print("\n")
        print("toString('*Var*'):\n" + lcProjects.toString("*Var*"))
        print("toString('*Project2*'):\n" + lcProjects.toString("*Project2*"))
        print("toString('*.deb*'):\n" + lcProjects.toString("*.deb*"))

        print("\n\n\n")
        lcProject = lcProjects.LoadProject("ICD", myConfigs)
        if (lcProject != None):
            print(lcProject.GetFullLabel() + "'s config:\n" + lcProject.toStringConfig())

        print("\n")
        lcProject = lcProjects.LoadProject("Project2", myConfigs)
        if (lcProject != None):
            print(lcProject.GetFullLabel() + "['buildtool/debug/call']: " + lcProject['buildtool/debug/call'])
        lcProject = lcProjects.LoadProject("Project1", myConfigs)
        if (lcProject != None):
            print(lcProject.GetFullLabel() + "['buildtool/debug/call']: " + lcProject['buildtool/debug/call'])

            lcProject.isKeyOrDie("xyz")
