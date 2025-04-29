#!/usr/local/bin/python
#    Filename : PyClass_FileList.py
# Description : Class to manage a list of files
# 					e.g.
#   					- reference folder
#  						- date time
#   					- Check latest modification date of all files
#   					- Check if all files exists
#   					- attributes like read, write, execute, exists
#   					- save and load to files


import os
import sys
import fnmatch
import time
import datetime

from PyClass_Log import tcLog
from PyClass_ErrorHandler import tcErrorHandler
from PyClass_FileStatus import tcFileStatus

class tcFileListEntry():
    def __init__(self, lszFile):
         # Convert to OS
        lszFile = os.path.normpath(lszFile)
        self.mszName   = lszFile

        lcFileStatus = tcFileStatus(self.mszName)

        self.mbExists  = lcFileStatus.isExisting()
        self.mszStatus = lcFileStatus.GetStatus()
        self.moTime    = lcFileStatus.GetTimeModified()

        self.mszDrive, self.mszPath    = os.path.splitdrive(lszFile)
        self.mszPath,  self.mszFile    = os.path.split(self.mszPath)
        self.mszFile,  self.mszFileExt = os.path.splitext(self.mszFile)

    def Refresh(self):
        lcFileStatus = tcFileStatus(self.mszName)

        self.mbExists  = lcFileStatus.isExisting()
        self.mszStatus = lcFileStatus.GetStatus()
        self.moTime    = datetime.datetime.fromtimestamp(lcFileStatus.GetTimeModified())

        self.mszDrive, self.mszPath    = os.path.splitdrive(lszFile)
        self.mszPath,  self.mszFile    = os.path.split(self.mszPath)
        self.mszFile,  self.mszFileExt = os.path.splitext(self.mszFile)


    def toString(self) ->str:
        if self.mbExists:
            lszTime = self.moTime.strftime('%Y-%m-%d %H:%M:%S')
            return "[" + self.mszName + "][" + self.mszStatus + "][" + lszTime + "]"
        return self.mszName + "/" + self.mszStatus



class tcFileList():
    def __init__(self, lszRefFolder = "", lcLog = None, lcErrHdl = None):
        self.mLstFiles = list()
        self.mszRefFolder = os.path.normpath(lszRefFolder)

        self.mcLog = lcLog
        self.mcErrHdl = lcErrHdl
        self.mcMatchedFile = None

    def isMatch(self, lcFile, lszMatchString = "*.*") ->bool:
        self.mcMatchedFile = None
        try:
            # Convert to to OS
            lszMatchString = os.path.normpath(lszMatchString)
            if fnmatch.fnmatch(lcFile.mszName, lszMatchString):
                self.mcMatchedFile = lcFile
                return True
        except:
            pass
        return False

    def GetLatestDateTime(self, lszMatchString = "*.*") ->datetime.datetime:
        self.mcMatchedFile = None
        try:
            # https://stackoverflow.com/questions/1310740/what-is-the-oldest-time-that-can-be-represented-in-python
            loDateTime  = datetime.datetime.min
            # Scan all files, use the match
            for lcFile in self.mLstFiles:
                if self.isMatch(lcFile, lszMatchString):
                    if (lcFile.mbExists):
                        if (lcFile.moTime > loDateTime):
                            self.mcMatchedFile = lcFile
                            loDateTime = lcFile.moTime
            return loDateTime
        except:
            pass
        return None

    def CheckIfAllExists(self, lszMatchString = "*.*") ->bool:
        try:
            # Scan all files, use the match
            for lcFile in self.mLstFiles:
                if self.isMatch(lcFile, lszMatchString):
                    if (not lcFile.mbExists):
                        return False
            return loDateTime
        except:
            pass
        return True

    def Refresh(self, lszMatchString = "*.*"):
        try:
            # Scan all files, use the match
            for lcFile in self.mLstFiles:
                if self.isMatch(lcFile, lszMatchString):
                    lcFile.Refresh()
        except:
            pass

    def FindFirst(self, lszMatchString = "*.*") ->tcFileListEntry:
        try:
            # Scan all files, use the match
            for lcFile in self.mLstFiles:
                if self.isMatch(lcFile, lszMatchString):
                    return lcFile
        except:
            pass
        return None

    def Exists(self, lszMatchString = "*.*") ->bool:
        if (self.FindFirst(lszMatchString) != None):
            return True
        return False

    def GetListMatchedFiles(self, lszMatchString = "*.*") ->list:
        lLstMatchedFiles = list()
        try:
            # Scan all files, use the match
            for lcFile in self.mLstFiles:
                if self.isMatch(lcFile, lszMatchString):
                    lLstMatchedFiles.append(lcFile)
        except:
            pass
        return lLstMatchedFiles

    def GetListRelFiles(self, lszMatchString = "*.*") ->list:
        lLstMatchedFiles = list()
        try:
            # Scan all files, use the match
            for lcFile in self.mLstFiles:
                if self.isMatch(lcFile, lszMatchString):
                    lszRelFile = os.path.relpath(lcFile.mszName, self.mszRefFolder)
                    lLstMatchedFiles.append(lszRelFile)
        except:
            pass
        return lLstMatchedFiles

    def GetSize(self) ->int:
        try:
            return len(self.mLstFiles)
        except:
            return 0

    def append(self, lxFiles):
        lLstFiles = list()
        if isinstance(lxFiles, str):
            lLstFiles.append(lxFiles)
        elif isinstance(lxFiles, list):
            lLstFiles += lxFiles

        for lxFile in lLstFiles:
            if isinstance(lxFile, str):
                lcFile = tcFileListEntry(lxFile)
                self.mLstFiles.append(lcFile)
            elif isinstance(lxFile, tcFileListEntry):
                self.mLstFiles.append(lxFile)

    def remove(self, lszMatchString = "*.*"):
        if lszMatchString == "*.*":
            self.Clear()
        else:
            lLstTemp = list()
            for lcFile in self.mLstFiles:
                if not self.isMatch(lcFile, lszMatchString):
                    lLstTemp.append(lcFile)
            self.mLstFiles = lLstTemp

    def Clear(self):
        self.mLstFiles.clear()

    def Save(self, lszFilename, lszHeaderText = "") ->bool:
        try:
            self.writeln("Writing Filelist-File: " + lszFilename)
            # https://stackoverflow.com/questions/899103/writing-a-list-to-a-file-with-python
            with open(lszFilename, 'w') as loTextFile:
                if (lszHeaderText != ""):
                    loTextFile.write("# " + lszHeaderText + "\n")
                lLstFile = self.GetListRelFiles()
                for lszFile in lLstFile:
                    loTextFile.write("%s\n" % lszFile)
        except:
            self.error("  ", 1, "could not be loaded: " + lszFilename)
            return False

    def Load(self, lszFilename) ->bool:
        try:
            self.writeln("Loading Filelist-File: " + lszFilename)
            if os.path.exists(lszFilename):
                for lszLine in open(lszFilename):
                    lszLine=lszLine.strip()
                    if not lszLine.startswith("#"):
                        if lszLine != "":
                            lszLine = os.path.normpath(lszLine)
                            lszLine = os.path.abspath(os.path.join(self.mszRefFolder, lszLine))
                            lcFile = tcFileListEntry(lszLine)
                            self.mLstFiles.append(lcFile)
                return True
            else:
                self.error("  ", 1, "not exists: " + lszFilename)
                return False
        except:
            self.error("  ", 1, "could not be loaded: " + lszFilename)
            return False

    def toString(self, lszMatchString = "*.*"):
        lszString = ""
        for lcFile in self.mLstFiles:
            if self.isMatch(lcFile, lszMatchString):
                lszString += lcFile.toString() + "\n"
        return lszString


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
        dir_this  = os.path.dirname(os.path.abspath(__file__))
        dir_any   = os.path.join(dir_this, "../")
        lcFileList = tcFileList(dir_any)
        lcFileList.append(__file__)
        print(lcFileList.toString())

        lLstFiles = lcFileList.GetListRelFiles()
        print(lLstFiles)

        lszTextFile = os.path.abspath(os.path.join(dir_any, "MyFileList.txt"))
        lcFileList.Save(lszTextFile, "Generated file. Do not change it manually")
        lcFileList.Load(lszTextFile)
        print(lcFileList.toString())

        os.remove(lszTextFile)

