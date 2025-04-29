# Will be called by toolcaller via the project's configuration
# System class is passed, to get the project to get access to the projects's configuration


import os, shutil, sys, re, json

class MyClass:
    def __init__(self, lcSystem):
        self.mcSystem  = lcSystem
        self.mcProject = self.mcSystem.Project_Get()

        self.lbDebug = False

    def run(self, lszParameter):

        # Get the data from the configuration

        dir_root = self.mcProject.isKeyOrDie("system/dir/root")


        lszProjectName = self.mcProject.GetFullLabel()
        self.debug(lszProjectName)

        lszVs2017JsonFile = os.path.join(dir_root, "CMakeSettings.json")

        self.writeln("Open existing " + lszVs2017JsonFile + "\n")

        lbProjectAlreadyInJson = False
        lLstJsonConfigurations = None

        ldctVs2017JsonFileData = dict()

        #https://stackabuse.com/reading-and-writing-json-to-a-file-in-python/
        if (os.path.exists(lszVs2017JsonFile)):
            with open(lszVs2017JsonFile) as json_file:
                ldctVs2017JsonFileData = json.load(json_file)

                # Check if the project is already listed in the json file
                for lszKey, loValue in ldctVs2017JsonFileData.items():
                    if (lszKey.lower().strip() == "configurations"):
                        lLstJsonConfigurations = loValue
                        if isinstance(loValue, list):
                            for loConfData in loValue:
                                if isinstance(loConfData, dict):
                                    for lszConfDataKey, loConfDataValue in loConfData.items():
                                        if (lszConfDataKey.lower().strip() == "name"):
                                            if (loConfDataValue.lower().strip() == lszProjectName.lower().strip()):
                                                lbProjectAlreadyInJson = True
                                                lLstJsonConfigurations.remove(loConfData)

        if (lbProjectAlreadyInJson):
            self.writeln("Updating " + lszProjectName + "\n")
        else:
            self.writeln("Adding " + lszProjectName + "\n")

        # Get Maketool from the maketool string
        # e.g. cmake -G Ninja ....
        # search for -G
        lszMakeToolCallString = self.mcProject.isKeyOrDie("maketool/generate/call")
        lszMakeToolCallString = lszMakeToolCallString.replace(" -", "|")
        llstMakeToolCallStringOptions = lszMakeToolCallString.split("|")

        lszMakeTool = "Ninja" #default
        lszBuildDir = "${workspaceRoot}\\bin\\${name}"

        for lszStr in llstMakeToolCallStringOptions:
            if (lszStr[0] == "g") or (lszStr[0] == "G"):
                lszMakeTool = lszStr[1:].strip()
            if (lszStr[0] == "b") or (lszStr[0] == "B"):
                lszBuildDir = lszStr[1:].strip()

        dir_root2 = dir_root
        if (os.path.exists(dir_root2)):
            dir_root2 = dir_root2.replace("/", "\\")

        # adapt the slashes
        lszBuildDir = lszBuildDir.replace("/", "\\")

        # Try to replace root by ${workspaceRoot}
        lszBuildDir = lszBuildDir.replace(dir_root2, "${workspaceRoot}")


        self.debug(lszMakeTool)
        self.debug(lszBuildDir)

        ldctJSonProject = {}

        ldctJSonProject['ProjectName'] = self.mcProject.isKeyOrDie("project/name")
        ldctJSonProject['ProjectLabelName'] = self.mcProject.GetLabel()
        ldctJSonProject['ProjectFullLabelName'] = self.mcProject.GetFullLabel()

        ldctJSonProject['name'] = self.mcProject.GetFullLabel()
        ldctJSonProject['generator'] = lszMakeTool
        ldctJSonProject['configurationType'] = self.mcProject.isKeyOrDie("project/config")
        ldctJSonProject['inheritEnvironments'] = ["msvc_x86"] # "msvc_x86"
        ldctJSonProject['buildRoot'] = lszBuildDir
        ldctJSonProject['installRoot'] = lszBuildDir
        ldctJSonProject['cmakeCommandArgs'] = ""
        ldctJSonProject['buildCommandArgs'] = "-v"
        ldctJSonProject['ctestCommandArgs'] = ""

        ldctJSonProject['variables'] = []
        for lszStr in llstMakeToolCallStringOptions:
            if (lszStr[0] == "d") or (lszStr[0] == "D"):
                lszVariable = lszStr[1:].strip()
                lstVariable = lszVariable.split("=")

                lszVariableName  = lstVariable[0].strip().strip("\"")
                lszVariableValue = lstVariable[1].strip().strip("\"")

                # Check, if it a path or a file. If yes, adapt the slashes
                if (os.path.exists(lszVariableValue)):
                    lszVariableValue = lszVariableValue.replace("/", "\\")

                # Try to replace root by ${workspaceRoot}
                lszVariableValue = lszVariableValue.replace(dir_root2, "${workspaceRoot}")

                self.debug(lszVariableName + " : " + lszVariableValue)

                ldctJSonProject['variables'].append({
                    'name': lszVariableName,
                    'value': lszVariableValue
                })

        if not 'configurations' in ldctVs2017JsonFileData:
            ldctVs2017JsonFileData['configurations'] = []
        ldctVs2017JsonFileData['configurations'].append(ldctJSonProject)

        if (lLstJsonConfigurations != None):
            # https://stackoverflow.com/questions/72899/how-do-i-sort-a-list-of-dictionaries-by-a-value-of-the-dictionary
            lLstJsonConfigurations = sorted(lLstJsonConfigurations, key=lambda k: k['name'])

        with open(lszVs2017JsonFile, 'w') as outfile:
            json.dump(ldctVs2017JsonFileData, outfile, indent=2)




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

