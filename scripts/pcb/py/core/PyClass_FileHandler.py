#!/usr/local/bin/python
#    Filename : PyClass_FileHandler.py
# Description : Collection of methodes to work with files and folders


import sys
import shutil
import os
import stat
import traceback
import glob
import fnmatch

from PyClass_FileStatus import tcFileStatus
from PyClass_Log import tcLog
from PyClass_ErrorHandler import tcErrorHandler

class tcFileHandler:
    def __init__(self, lcLog = None, lcErrHdl = None):
        self.mcLog = lcLog
        self.mcErr = lcErrHdl

    def CheckAndRemove_WriteProtection(self, lszFile):
        lszDestFilename = os.path.basename(lszFile)

        lcFileStatus = tcFileStatus(lszFile)

        self.writeln("  => " + lszDestFilename + " mode: " + lcFileStatus.toString())

        if (lcFileStatus.isExisting() == 1) and (lcFileStatus.isWriteProtected() == 1):
            self.writeln("  => Remove write protection")
            os.chmod(lszFile, stat.S_IWRITE)

            lcFileStatus2 = tcFileStatus(lszFile)
            if (lcFileStatus2.isWriteProtected() == 0):
                self.writeln("    => OK")

    def Copy(self, lszSource, lszDest):
        try:
            self.writeln("\nCopy " + lszSource + " to " + lszDest)

            # Check, if Source File is existing
            lcFileStatus = tcFileStatus(lszSource)
            if (lcFileStatus.isExisting() == 0):
                self.error("  => ", 1, "Source file missing")
                return 1

            # Check, if Destination-File can be written
            self.CheckAndRemove_WriteProtection(lszDest)

            # Create destination folder
            lszDestFolder = lszDest.replace(os.path.basename(lszDest), "")
            lcStatus = tcFileStatus(lszDestFolder)
            if (lcStatus.isExisting() == 0):
                os.makedirs(lszDestFolder)

            # Copy file
            shutil.copy2(lszSource, lszDest)

        except:
            self.error("  => ", 2, "Unexpected error! ".join(traceback.format_exception(*sys.exc_info())[-2:]).strip().replace('\n',': '))
            return 1

        # Check if file was copied
        if os.access(lszDest, os.F_OK):
            self.writeln("  => File was successful copied")
        else:
            self.error("  => ", 3, "Destination missing.")
            return 1

        return 0


    def Delete(self, lszDest):
        try:
            self.writeln("\nDelete " + lszDest)

            # Check, if Dest File is existing
            lcFileStatus = tcFileStatus(lszDest)
            if (lcFileStatus.isExisting() == 0):
                self.writeln("  => Not existing. Nothing to delete")
                return 0

            # Check, if Destination-File can be written
            self.CheckAndRemove_WriteProtection(lszDest)

            # delete file
            os.remove(lszDest)

        except:
            self.error("  => ", 2, "Unexpected error! ".join(traceback.format_exception(*sys.exc_info())[-2:]).strip().replace('\n',': '))
            return 1

        # Check if file was copied
        if os.access(lszDest, os.F_OK):
            self.error("  => ", 3, "File was not deleted")
            return 1
        else:
            self.writeln("  => File was successful deleted")
            return 0


        return 0


    def CreateDir(self, lszFolder):
        # Check, if folder already existing
        lcFileStatus = tcFileStatus(lszFolder)
        if (lcFileStatus.isExisting() == 0):
            try:
                # if not create it
                self.writeln("\nCreate Folder: " + lszFolder)
                os.makedirs(lszFolder)
            except:
                self.error("  => ", 2, "Unexpected error! ".join(traceback.format_exception(*sys.exc_info())[-2:]).strip().replace('\n',': '))
                return 1

            # Check, if folder was finally created
            lcFileStatus = tcFileStatus(lszFolder)
            if (lcFileStatus.isExisting() == 1):
                self.writeln("  => Folder was successful created")
            else:
                self.error("  => ", 3, "Folder wasn't created.")
                return 1

        return 0

    def CreateDirForFile(self, lszFile):
        lszFolder = os.path.dirname(os.path.abspath(lszFile))
        return self.CreateDir(lszFolder)


    def Move(self, lszSource, lszDest):
        try:
            self.writeln("\nMove " + lszSource + " to " + lszDest)

            # Check, if Source File is existing
            lcFileStatus = tcFileStatus(lszSource)
            if (lcFileStatus.isExisting() == 0):
                self.error("  => ", 1, "Source file missing")
                return 1

            # Check, if Source-File can be moved
            self.CheckAndRemove_WriteProtection(lszSource)

            # Check, if Destination-File can be written
            self.CheckAndRemove_WriteProtection(lszDest)

            # Copy file
            shutil.move(lszSource, lszDest)

        except:
            self.error("  => ", 2, "Unexpected error! ".join(traceback.format_exception(*sys.exc_info())[-2:]).strip().replace('\n',': '))
            return 1

        # Check if file was copied
        if os.access(lszDest, os.F_OK):
            self.writeln("  => File was successful moved")
        else:
            self.error("  => ", 3, "Destination missing.")
            return 1

        return 0

    #copy from https://docs.python.org/2/library/shutil.html
    def CopyTree_org(self, src, dst, symlinks=False, ignore=None):
        liError = 0
        names = os.listdir(src)
        if ignore is not None:
            ignored_names = ignore(src, names)
        else:
            ignored_names = set()

        if not os.path.exists(dst):
            os.makedirs(dst)
        errors = []
        for name in names:
            if name in ignored_names:
                continue
            srcname = os.path.join(src, name)
            dstname = os.path.join(dst, name)
            try:
                if symlinks and os.path.islink(srcname):
                    linkto = os.readlink(srcname)
                    os.symlink(linkto, dstname)
                elif os.path.isdir(srcname):
                    liError += self.CopyTree_org(srcname, dstname, symlinks, ignore)
                else:
                    liError += self.Copy(srcname, dstname)
                    # XXX What about devices, sockets etc.?
            except (IOError, os.error) as why:
                errors.append((srcname, dstname, str(why)))
            # catch the Error from the recursive copytree so that we can
            # continue with other files
            except Error as err:
                errors.extend(err.args[0])
        try:
            shutil.copystat(src, dst)
        except WindowsError:
            # can't copy file access times on Windows
            pass
        except OSError as why:
            errors.extend((src, dst, str(why)))
        if errors:
            raise Error(errors)
        return liError

    # This limitation of the standard shutil.copytree seems arbitrary and annoying. Workaround
    def CopyTree(self, src, dst, symlinks=False, ignore=None):
        liError = 0
        for item in os.listdir(src):
            s = os.path.join(src, item)
            d = os.path.join(dst, item)
            if os.path.isdir(s):
                liError += self.CopyTree_org(s, d, symlinks, ignore)
            else:
                liError += self.Copy(s, d)
        return liError

    def ListFolder(self, lszSrcDirectory, loDstFileList, lszAcceptedFiles):
        liError = 0
        """ This function will generate the file names in a directory
        tree by walking the tree either top-down or bottom-up. For each
        directory in the tree rooted at directory top (including top itself),
        it yields a 3-tuple (dirpath, dirnames, filenames).  """

        loAcceptedFiles = lszAcceptedFiles.split(",")
        loAcceptedFiles = [item.strip() for item in loAcceptedFiles]

        lLstFile = [lszFilename for lszFilename in os.listdir(lszSrcDirectory) if os.path.isfile(os.path.join(lszSrcDirectory, lszFilename))]
        for lszFilename in lLstFile:
            # Join the two strings in order to form the full filepath.
            for lszAcceptedFiles in loAcceptedFiles:
                # Check, if it is in the acceptance list
                if fnmatch.fnmatch(lszFilename, lszAcceptedFiles):
                    lszFilepath = os.path.join(lszSrcDirectory, lszFilename)
                    loDstFileList.append(lszFilepath)  # Add it to the list.

        return liError

    # This limitation of the standard shutil.copytree seems arbitrary and annoying. Workaround
    def ListTree(self, lszSrcDirectory, loDstFileList, lszAcceptedFiles):
        liError = 0
        """ This function will generate the file names in a directory
        tree by walking the tree either top-down or bottom-up. For each
        directory in the tree rooted at directory top (including top itself),
        it yields a 3-tuple (dirpath, dirnames, filenames).  """

        loAcceptedFiles = lszAcceptedFiles.split(",")
        loAcceptedFiles = [item.strip() for item in loAcceptedFiles]

        # Walk the tree.
        for lszRoot, loDirectories, loFiles in os.walk(lszSrcDirectory):
            for lszFilename in loFiles:
                # Join the two strings in order to form the full filepath.
                for lszAcceptedFiles in loAcceptedFiles:
                    # Check, if it is in the acceptance list
                    if fnmatch.fnmatch(lszFilename, lszAcceptedFiles):
                        lszFilepath = os.path.join(lszRoot, lszFilename)
                        loDstFileList.append(lszFilepath)  # Add it to the list.

        return liError

    def CopyFiles(self, lszSourcePath, lszDestPath):
        liError = 0
        for lcFile in glob.glob(lszSourcePath):
            s = os.path.join(os.path.dirname(lcFile), os.path.basename(lcFile))
            d = os.path.join(lszDestPath, os.path.basename(lcFile))
            liError += self.Copy(s, d)
        return liError

    def MoveFiles(self, lszSourcePath, lszDestPath):
        liError = 0
        for lcFile in glob.glob(lszSourcePath):
            s = os.path.join(os.path.dirname(lcFile), os.path.basename(lcFile))
            d = os.path.join(lszDestPath, os.path.basename(lcFile))
            liError += self.Move(s, d)
        return liError

    def RenameFiles(self, lszSourcePath, lszSearchString, lszReplaceString):
        liError = 0

        for lcFile in glob.glob(lszSourcePath):
            lszFilename = os.path.basename(lcFile)
            if (lszReplaceString not in lszFilename):
                if (lszSearchString in lszFilename):
                    lszSrcDest = os.path.join(os.path.dirname(lcFile), lszFilename.replace(lszSearchString, lszReplaceString))
                    liError += self.Move(lcFile, lszSrcDest)
        return liError

    def InsertFiles(self, lszSozurcePath, liPos, lszStringToInsert):
        liError = 0

        for lcFile in glob.glob(lszSozurcePath):
            lszFilename = os.path.basename(lcFile)
            if (lszStringToInsert not in lszFilename):
                lszFilename = lszFilename[:liPos] + lszStringToInsert + lszFilename[liPos:]
                lszSrcDest = os.path.join(os.path.dirname(lcFile), lszFilename)
                liError += self.Move(lcFile, lszSrcDest)
        return liError
    
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
        if (self.mcErr != None):
            self.mcErr.error(lszPrompt, liErrorNumber, lszErrorText, end, lbAddTraceback)
        else:
            print(lszPrompt + "Error " + str(liErrorNumber) + ": " + lszErrorText)

    def warn(self, lszPrompt, liWarnNumber, lszWarnText):
        if (self.mcErr != None):
            self.mcErr.warn(lszPrompt, liWarnNumber, lszWarnText)
        else:
            print(lszPrompt + "Warning " + str(liWarnNumber) + ": " + lszWarnText)

 # Test
if __name__ == "__main__":
    isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ
    if isRunningInPyCharm:
        lcLog = tcLog("Log.txt", "w")
        lcErr = tcErrorHandler(lcLog)
        lcSfh = tcFileHandler(lcLog, lcErr)

        szFileSource1 = os.getcwd()+"\\"+"Log.txt"
        szFileDest1   = os.getcwd()+"\\"+"Dest1.txt"
        szFileDest2   = os.getcwd()+"\\"+"Dest2.txt"

        lcSfh.Copy(szFileSource1, szFileDest1)
        lcSfh.Copy(szFileSource1, szFileDest1)
        lcSfh.Move(szFileDest1,   szFileDest2)
