#!/usr/local/bin/python
#    Filename : PyClass_FileDep.py
# Description : Class to handle time based dependencies of a input and a target file list


from PyClass_Log import tcLog
from PyClass_ErrorHandler import tcErrorHandler
from PyClass_FileList import tcFileStatus

class tcFileDep():
    def __init__(self, lcInputFileList = None, lcTargetFileList = None, lcLog = None, lcErrHdl = None):
        self.mcInputFileList  = lcInputFileList
        self.mcTargetFileList = lcTargetFileList
        self.liDepState = 0

    def Refresh(self, lszMatchString = "*.*"):
        self.mcInputFileList.Refresh(lszMatchString)
        self.mcTargetFileList.Refresh(lszMatchString)

    # Return an integer
    # 0: No issue
    # 1: Any input file is missing
    # 2: Any target file is missing
    # 4: At least one input file is newer then one of the target files
    def CheckDependencies(self) ->int:
        liReturn = 0

        if (self.mcInputFileList != None):
            if (not self.mcInputFileList.CheckIfAllExists()):
                liReturn += 1

        if (self.mcTargetFileList != None):
            if (not self.mcTargetFileList.CheckIfAllExists()):
                liReturn += 2

        if (self.mcInputFileList != None):
            if (self.mcTargetFileList != None):
                loInputTime  = self.mcInputFileList.GetLatestDateTime()
                loTargetTime = self.mcTargetFileList.GetLatestDateTime()

                if (loInputTime > loTargetTime):
                    liReturn += 4

        self.liDepState = liReturn
        return liReturn

    def IsTargetMissing(self) ->bool:
        if bool(self.liDepState&2):
            return True
        return False

    def IsInputMissing(self) ->bool:
        if bool(self.liDepState&1):
            return True
        return False

    def IsTargetUptodate(self) ->bool:
        if bool(self.liDepState&4):
            return False
        return True

    def IsAllFine(self) ->bool:
        if self.liDepState == 0:
            return True
        return False


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