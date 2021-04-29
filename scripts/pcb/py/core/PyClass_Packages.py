#!/usr/local/bin/python
#    Filename : PyClass_Packages.py
# Description : Class to manage all of the packages in the given source
#               folder in a list, that are given by a package list.


import os
import sys
import configparser # to parse .ini files
import collections
import re
import glob
import ntpath
import fnmatch

from PyClass_FileHandler import tcFileHandler
from PyClass_FileStatus import tcFileStatus
from PyClass_Log import tcLog
from PyClass_ErrorHandler import tcErrorHandler

"""
  Class to manage all of the packages in the given source folder in a list, that are given by a package list.
    The Class scans all given packages and collect its informations in a dict, e.g. if the package
       - has a '_PkgCfg.txt' file
       - has a variant/configuration


  Package list must be like follows:
      - One package per line
      - package variant is separated by a ':', e.g. dio:var1
      - subfolder can be used e.g. driver/dio.var1
           - => package name is dio
           - => package variant is var1
           - => package location is [lszPackageFolder]/driver/rh850/dio
           - => package variant location is [lszPackageFolder]/driver/dio/_var/var1
"""

# Package information, mainly used to organize folder and files by a key
# Currently supported
#   self.mszName => Package name
#   self.mcPackage["original_str"] => original string read from package list, this string will be interpreted to path, name, variant
#   self.mcPackage["variant"] => packages variant. No exists, if no variant in given
#   self.mcPackage["root_folder"] => absolute root folder of the package
#   self.mcPackage["variant_folder"] => absolute location  of the folder of the package variant. No exists, if no variant in given
#   self.mcPackage["file_config"] => absolute location of the package's config file. No exists, if the file not exists
#   self.mcPackage["data_config"] => dict of the content of file_config. No exists, if the file not exists
#   self.mcPackage["package_name"] => name of the package taken from the config 'package version'. No exists, if the config not exists
#

class tcPackage:
    def __init__(self, lszName, lcSystem = None, lcLog = None, lcErrHdl = None):
        self.mcPackage =  collections.OrderedDict()
        self.mszName = lszName
        self.mboEnable = True

        self.mcLog = lcLog
        self.mcErrHdl = lcErrHdl

        self.mcSystem  = lcSystem
        self.mcProject = None
        if (self.mcSystem != None):
            self.mcProject = self.mcSystem.Project_Get()


    def Set(self, lszKey, lszValue):
        lszKey = lszKey.lower()
        self.mcPackage[lszKey] = lszValue

    def Remove(self, lszKey):
        lszKey = lszKey.lower()
        if (lszKey in self.mcPackage):
            del self.mcPackage[lszKey]

    def Exists(self, lszGivenKey) ->bool:
        for lszKeys,loValue in self.mcPackage.items():
            if (lszKeys.lower() == lszGivenKey.lower()):
                return True
        return False


    def Get_Config(self, lszConfig) ->str:
        # if the _PkgCfg.txt was load, the information can be read like follows
        #      [section].[key] => value
        #
        liDot = lszConfig.find('.')

        if (liDot > 0):
            lszSection = lszConfig[:liDot].strip()
            lszKey = lszConfig[liDot+1:].strip()

        if (lszSection != "") and (lszKey != ""):
            if (self.Exists("data_config")):
                ldctPackageConfig = dict()
                ldctPackageConfig = self["data_config"]

                # Search dict and sub dict, everthing to lower case
                for lszSectionFound, ldictValuesFound in ldctPackageConfig.items():
                    if (lszSection.lower() == lszSectionFound.lower()):
                        for lszKeyFound, lszValuesFound in ldictValuesFound.items():
                            if (lszKey.lower() == lszKeyFound.lower()):
                                return lszValuesFound

        return ""   # Nothing found

    def Config_Exists(self, lszConfig) ->bool:
        liDot = lszConfig.find('.')

        if (liDot > 0):
            lszSection = lszConfig[:liDot].strip()
            lszKey = lszConfig[liDot+1:].strip()

        if (lszSection != "") and (lszKey != ""):
            if (self.Exists("data_config")):
                ldctPackageConfig = dict()
                ldctPackageConfig = self["data_config"]

                # Search dict and sub dict, everthing to lower case
                for lszSectionFound, ldictValuesFound in ldctPackageConfig.items():
                    if (lszSection.lower() == lszSectionFound.lower()):
                        for lszKeyFound, lszValuesFound in ldictValuesFound.items():
                            if (lszKey.lower() == lszKeyFound.lower()):
                                return True

        return False   # Nothing found


    def Get(self, lszKey):
        lszKey = lszKey.lower()
        return self[lszKey]

    def __setitem__(self, lszKey, lszValue):
        lszKey = lszKey.lower()
        self.Remove(lszKey)
        self.mcPackage[lszKey] = lszValue


    def __getitem__(self, lszKey):
        lszKey = lszKey.lower()
        if (lszKey in self.mcPackage):
            return self.mcPackage[lszKey]
        else:
            return ""


    # Scans the given folder
    # Return a dict. The dict contains 4 keys. (files, asmfiles, header, include_directories).
    #  The value of each key is a list with files/folders.
    def GernerateFileGenList_ScanFolder(self, lszScanFolder, lszName = "", lbScanSubFolders = True) -> dict:
        def ReplaceInList(lLstListm, lszStringToReplace, lszStringReplaceWith):
            # https://stackoverflow.com/questions/6130211/python-modify-item-in-list-save-back-in-list
            for liIdx, lszItem in enumerate(lLstListm):
                if lszStringToReplace in lszItem:
                    lszItem = lszItem.replace(lszStringToReplace, lszStringReplaceWith)
                    lLstListm[liIdx] = lszItem

        def RemoveVariant_UnderscoreFolders(lLstSource) ->list:
            lLstDest = list()
            for lszItem in lLstSource:
                # if item not contains '/_var' take it
                if (lszItem.lower().find(os.path.join(lszScanFolder, "_var").lower()) == -1):
                    lLstDest.append(lszItem)
            return lLstDest

        lDictReturn = dict()

        lszName2 = lszName
        if lszName != "":
            lszName2 += "_"

        lszLabel_SourceFiles = lszName2 + "gen_files"
        lszLabel_AsmFiles    = lszName2 + "gen_asmfiles"
        lszLabel_HeaderFiles = lszName2 + "gen_headers"
        lszLabel_Dir_Header  = lszName2 + "gen_include_directories"

        lcFileHdl = self.mcSystem.mcFile

        # Prepare the file list

        # lLstSource_File: Disc-File-List for Source files, e.g. *.c, *.cpp
        lLstSource_File = list()
        if (lbScanSubFolders == True):
            lcFileHdl.ListTree(lszScanFolder, lLstSource_File, "*.c, *.cpp")
        else:
            lcFileHdl.ListFolder(lszScanFolder, lLstSource_File, "*.c, *.cpp")
        lLstSource_File = sorted(lLstSource_File)
        lLstSource_File = RemoveVariant_UnderscoreFolders(lLstSource_File)

        # lLstAsm_File: Disc-File-List for Assembler files, e.g. *.s,*.800,*.asm
        lLstAsm_File = list()
        if (lbScanSubFolders == True):
            lcFileHdl.ListTree(lszScanFolder, lLstAsm_File, "*.s,*.800,*.asm")
        else:
            lcFileHdl.ListFolder(lszScanFolder, lLstAsm_File, "*.s,*.800,*.asm")
        lLstAsm_File = sorted(lLstAsm_File)
        lLstAsm_File = RemoveVariant_UnderscoreFolders(lLstAsm_File)

        # lLstHeader_File: Disc-Header-List for Header files, e.g *.h, *.hpp
        lLstHeader_File = list()
        if (lbScanSubFolders == True):
            lcFileHdl.ListTree(lszScanFolder, lLstHeader_File, "*.h,*.hpp")
        else:
            lcFileHdl.ListFolder(lszScanFolder, lLstHeader_File, "*.h,*.hpp")
        lLstHeader_File = sorted(lLstHeader_File)
        lLstHeader_File = RemoveVariant_UnderscoreFolders(lLstHeader_File)

        # lLstIncludesDirs: Disc-Folder-List of the Disc-Header-List
        # Uses the lLstHeader_File as Input, removes the file names => Folders only
        # and avoid duplicates
        lLstIncludesDirs = list()
        for lszHeader_File in lLstHeader_File:
            lszDir_Header_File = os.path.dirname(lszHeader_File)
            if (not lszDir_Header_File in lLstIncludesDirs):
                lLstIncludesDirs.append(lszDir_Header_File)

        # Replace the root-folder in all Lists to get a relative path
        ReplaceInList(lLstSource_File,  self["root_folder"], ".")
        ReplaceInList(lLstAsm_File,     self["root_folder"], ".")
        ReplaceInList(lLstHeader_File,  self["root_folder"], ".")
        ReplaceInList(lLstIncludesDirs, self["root_folder"], ".")

        # Correct '\' to '/' for Cmake Syntax
        ReplaceInList(lLstSource_File,   "\\", "/")
        ReplaceInList(lLstAsm_File,      "\\", "/")
        ReplaceInList(lLstHeader_File,   "\\", "/")
        ReplaceInList(lLstIncludesDirs,  "\\", "/")

        lDictReturn[lszLabel_SourceFiles] = lLstSource_File
        lDictReturn[lszLabel_AsmFiles]    = lLstAsm_File
        lDictReturn[lszLabel_HeaderFiles] = lLstHeader_File
        lDictReturn[lszLabel_Dir_Header]  = lLstIncludesDirs

        return lDictReturn


    def GernerateFileGenList_CollectExcluded(self, lszDestFolder, lLstPassedExcludedFiles) ->list:
        def ReplaceInList(lLstListm, lszStringToReplace, lszStringReplaceWith):
            # https://stackoverflow.com/questions/6130211/python-modify-item-in-list-save-back-in-list
            for liIdx, lszItem in enumerate(lLstListm):
                if lszStringToReplace in lszItem:
                    lszItem = lszItem.replace(lszStringToReplace, lszStringReplaceWith)
                    lLstListm[liIdx] = lszItem

        def StripList(lLstListm):
            # https://stackoverflow.com/questions/6130211/python-modify-item-in-list-save-back-in-list
            for liIdx, lszItem in enumerate(lLstListm):
                lLstListm[liIdx] = lszItem.strip()


        # Check if there are exclude lists are defined
        lLstExcludedFilesFromFile = list()
        # Check root folder
        if os.path.exists(os.path.join(lszDestFolder, "_PkgExclude.txt")):
            with open(os.path.join(lszDestFolder, "_PkgExclude.txt"), "r") as loExcludedFilesFormFile:
                lLstExcludedFilesFromFile = loExcludedFilesFormFile.readlines()

        # Create an exclude list from all input lists
        lLstExcludedFiles = lLstPassedExcludedFiles + lLstExcludedFilesFromFile

        # Correct '\' to '/' for Cmake Syntax
        ReplaceInList(lLstExcludedFiles, "\\", "/")

        StripList(lLstExcludedFiles)
        lLstExcludedFiles = list(set(lLstExcludedFiles))  # sort and removes duplicates

        return lLstExcludedFiles


    def GernerateFileGenList_File(self, lszDestFolder, lDctFiles, lLstExcludedFiles, lbForceWrite = False):
        def ReplaceInList(lLstListm, lszStringToReplace, lszStringReplaceWith):
            # https://stackoverflow.com/questions/6130211/python-modify-item-in-list-save-back-in-list
            for liIdx, lszItem in enumerate(lLstListm):
                if lszStringToReplace in lszItem:
                    lszItem = lszItem.replace(lszStringToReplace, lszStringReplaceWith)
                    lLstListm[liIdx] = lszItem

        def StripList(lLstListm):
            # https://stackoverflow.com/questions/6130211/python-modify-item-in-list-save-back-in-list
            for liIdx, lszItem in enumerate(lLstListm):
                lLstListm[liIdx] = lszItem.strip()

        def ListToCmake(lLstListm, lszListName) ->str:
            lszCMakeList = "set (" + lszListName + " \n"
            for lszItem in lLstListm:
                lszCMakeList += "\t" + lszItem + "\n"
            lszCMakeList += ")\n"
            return lszCMakeList

        def ListToCmake2(lszListName) ->str:
            lszCMakeList = "set (" + lszListName + " \n"

            for lszName, lLstFiles in lDctFiles.items():
                if lszName.find(lszListName) >= 0:
                    lszCMakeList += "\t# List of all source files found in " + lszName + "\n"
                    for lszItem in lLstFiles:
                        lszCMakeList += "\t" + lszItem + "\n"

            lszCMakeList += ")\n"
            return lszCMakeList

        def CheckForLongPath(lLstSource, liWarnMaxLength):
            # Check for files with to long path
            for lszFile in lLstSource:
                if len(self["root_folder"] + lszFile) > liWarnMaxLength:
                    self.warn("", 1, "Path length (" + str(liWarnMaxLength) +"): " + str(len(lszFile)) + ": " + self["root_folder"] + lszFile)

        lbWrite = lbForceWrite

        lszLabel_PkgName             = "gen_pkg_name"
        lszLabel_SourceFiles         = "gen_files"
        lszLabel_AsmFiles            = "gen_asmfiles"
        lszLabel_HeaderFiles         = "gen_headers"
        lszLabel_Dir_Header          = "gen_include_directories"
        lszLabel_ExcludedSourceFiles = "gen_excluded_sourcefiles"
        lszLabel_ExcludedHeaderFiles = "gen_excluded_headerfiles"
        lszLabel_ExcludedIncludes    = "gen_excluded_includes"

        liWarnMaxLength        = 200
        try:
            lszWarnMaxLength       = self.mcProject['system/dir/check/warn_threshold'].strip()
            if (lszWarnMaxLength != ""):
                liWarnMaxLength    = int(lszWarnMaxLength)
        except:
            pass

        for lszKeys, lLstFileList in lDctFiles.items():
            CheckForLongPath(lLstFileList, liWarnMaxLength)

        # Make an exclude list, based on the excluded files, that matches any of the input files
        lLstFilesAll = list()
        for lszKeys, lLstFileList in lDctFiles.items():
            if (lszKeys.find("gen_files") >= 0):
                lLstFilesAll = lLstFilesAll + lLstFileList
            if (lszKeys.find("gen_asmfiles") >= 0):
                lLstFilesAll = lLstFilesAll + lLstFileList
            if (lszKeys.find("gen_headers") >= 0):
                lLstFilesAll = lLstFilesAll + lLstFileList

        # lLstIncludesDirs: Disc-Folder-List of the Disc-Header-List
        # Uses the lLstHeader_File as Input, removes the file names => Folders only
        # and avoid duplicates
        lLstIncludesDirs = list()
        for lszKeys, lLstFileList in lDctFiles.items():
            if (lszKeys.find("gen_include_directories") >= 0):
                lLstIncludesDirs = lLstIncludesDirs + lLstFileList


        lLstExcludedFilesMatched    = list()
        lLstExcludedFilesNotMatched = list()
        lLstExcludedIncludesInput   = list()


        lszSelfRoot = self["root_folder"]
        for lszExclude in lLstExcludedFiles:
            if (lszExclude.strip() != ""):
                # check, if it is a existing file
                lbExcluded = False
                lszCheckForFile = self["root_folder"] + '/' + lszExclude
                lszCheckForFile = lszCheckForFile.strip()

                # Exclude Files
                if (os.path.isfile(lszCheckForFile)):
                    lLstExcludedFilesMatched.append('.' + lszCheckForFile.replace(lszSelfRoot, ""))
                    lbExcluded = True
                else:
                    # Exclude Folder and all their containing files
                    # Store the excluded folder in an extra list
                    lLstExcludedIncludesInput.append(lszExclude)

                    lszExclude = ('*' + lszExclude + '*').replace('**', '*')
                    if (lszExclude.strip() != ""):
                        for lszFilesAll in lLstFilesAll:
                            if fnmatch.fnmatch(lszFilesAll.lower(), lszExclude.lower()):
                                lLstExcludedFilesMatched.append(lszFilesAll)

                                lszDir_Header_File = os.path.dirname(lszFilesAll)
                                if (not lszDir_Header_File in lLstIncludesDirs):
                                    lLstIncludesDirs.append(lszDir_Header_File)

                                lbExcluded = True

                    if (lbExcluded == False):
                        lszExclude = (lszCheckForFile + '*').replace('**', '*')
                        if glob.glob(lszExclude):
                            lbExcluded = True

                if (lbExcluded == False):
                    lLstExcludedFilesNotMatched.append(lszCheckForFile)

        # Countercheck the included directories against the excluded directories
        lLstExcludedIncludes = list()
        for lszExcludedInclude in lLstExcludedIncludesInput:
            for lszIncludes in lLstIncludesDirs:
                lszExclude = ('*' + lszExcludedInclude + '*').replace('**', '*')
                if fnmatch.fnmatch(lszIncludes.lower(), lszExclude.lower()):
                    lLstExcludedIncludes.append(lszIncludes)

        lLstExcludedFilesNotMatched = sorted(list(set(lLstExcludedFilesNotMatched)))
        for lszExcludeNotMatched in lLstExcludedFilesNotMatched:
            self.warn("", 1, "Excluded file not existing: " + lszExcludeNotMatched)

        # sort and remove duplicates
        # https://stackoverflow.com/questions/7961363/removing-duplicates-in-lists
        lLstExcludedFilesMatched = sorted(list(set(lLstExcludedFilesMatched)))

        # Split lLstExcludedFilesMatched in source and headers
        lLstExcludedSourceFilesMatched = list()
        lLstExcludedHeaderFilesMatched = list()
        for lszEntry in lLstExcludedFilesMatched:
            if ((fnmatch.fnmatch(lszEntry.lower(), "*.h")) or
                    (fnmatch.fnmatch(lszEntry.lower(), "*.hpp"))):
                lLstExcludedHeaderFilesMatched.append(lszEntry)
            else:
                lLstExcludedSourceFilesMatched.append(lszEntry)



        # Create the genlists.cmake in a string to be compareable with a existing file
        lszGenlists = "# PyClass_Packages.py: This is a generated file. Do not modify it.\n"
        lszGenlists += "\n"
        lszGenlists += "set(" + lszLabel_PkgName + " \"" + self.mszName + "\")\n"
        lszGenlists += "\n"
        lszGenlists += "# List of all source files found in the packages filetree\n"
        lszGenlists += ListToCmake2(lszLabel_SourceFiles)
        lszGenlists += (2 * "\n")
        lszGenlists += "# List of all header files found in the packages filetree\n"
        lszGenlists += ListToCmake2(lszLabel_HeaderFiles)
        lszGenlists += (2 * "\n")
        lszGenlists += "# List of all assembler files found in the packages filetree\n"
        lszGenlists += ListToCmake2(lszLabel_AsmFiles)
        lszGenlists += (2 * "\n")
        lszGenlists += "# List of all folders containing any header in files the packages filetree\n"
        lszGenlists += ListToCmake2(lszLabel_Dir_Header)
        lszGenlists += (2 * "\n")
        lszGenlists += "# List of all source files excluded by the user\n"
        lszGenlists += ListToCmake(lLstExcludedSourceFilesMatched, lszLabel_ExcludedSourceFiles)
        lszGenlists += (2 * "\n")
        lszGenlists += "# List of all header files excluded by the user\n"
        lszGenlists += ListToCmake(lLstExcludedHeaderFilesMatched, lszLabel_ExcludedHeaderFiles)
        lszGenlists += (2 * "\n")
        lszGenlists += "# List of all include folders excluded by the user\n"
        lszGenlists += ListToCmake(lLstExcludedIncludes, lszLabel_ExcludedIncludes)
        lszGenlists += (2 * "\n")
        lszGenlists += "# Final list of source files. allfiles minus excluded files\n"
        lszGenlists += "if (" + lszLabel_SourceFiles + " AND " + lszLabel_ExcludedSourceFiles + ")\n"
        lszGenlists += "    list(REMOVE_ITEM " + lszLabel_SourceFiles + " ${" + lszLabel_ExcludedSourceFiles + "})\n"
        lszGenlists += "endif()\n"
        lszGenlists += (1 * "\n")
        lszGenlists += "# Final list of source files. allfiles minus excluded files\n"
        lszGenlists += "if (" + lszLabel_AsmFiles + " AND " + lszLabel_ExcludedSourceFiles + ")\n"
        lszGenlists += "    list(REMOVE_ITEM " + lszLabel_AsmFiles + " ${" + lszLabel_ExcludedSourceFiles + "})\n"
        lszGenlists += "endif()\n"
        lszGenlists += (1 * "\n")
        lszGenlists += "# Final list of header files. allfiles minus excluded headers\n"
        lszGenlists += "if (" + lszLabel_HeaderFiles + " AND " + lszLabel_ExcludedHeaderFiles + ")\n"
        lszGenlists += "    list(REMOVE_ITEM " + lszLabel_HeaderFiles + " ${" + lszLabel_ExcludedHeaderFiles + "})\n"
        lszGenlists += "endif()\n"
        lszGenlists += (1 * "\n")
        lszGenlists += "# Final list of included folders. allfolders minus excluded folders\n"
        lszGenlists += "if (" + lszLabel_Dir_Header + " AND " + lszLabel_ExcludedIncludes + ")\n"
        lszGenlists += "    list(REMOVE_ITEM " + lszLabel_Dir_Header + " ${" + lszLabel_ExcludedIncludes + "})\n"
        lszGenlists += "endif()\n"
        lszGenlists += (1 * "\n")

        lszDestFile = os.path.join(lszDestFolder, "_PkgGenLists.cmake")

        if (lbWrite == False):
            # Check if a existing file is different to the new one
            if (os.path.exists(lszDestFile)):
                with open(lszDestFile, "r") as loExistingGenList:
                    lszExistingGenList = loExistingGenList.read()

                    if (lszExistingGenList != lszGenlists):
                        lbWrite = True
            else:
                lbWrite = True

        if (lbWrite == True):
            with open(lszDestFile, "w") as text_file:
                text_file.write(lszGenlists)


    # Generate a "genlists.cmake" in the package root folder, in case a "CmakeLists.txt" exists in the same folder or if forced (lbForceCreate = True)
    # Overwrite a existing "genlists.cmake" only on change or if forced (lbForceWrite = True)
    def GernerateFileGenList(self, lbForceWrite = False):

        def GetVariantFolderbyIndex(lLstVariants, liIndex):
            lszFolder = ""
            for liIdx, lszItem in enumerate(lLstVariants):
                if liIdx == 0:
                    lszFolder = lszFolder + "_var/" + lLstVariants[liIdx]
                else:
                    lszFolder = lszFolder + "/_var/" + lLstVariants[liIdx]

                if (liIdx == liIndex):
                    lszFolder = os.path.normpath(lszFolder)
                    return lszFolder
            return lszFolder

        def GetVariantNamebyIndex(lLstVariants, liIndex):
            lszName = ""
            for liIdx, lszItem in enumerate(lLstVariants):
                if liIdx == 0:
                    lszName = lszItem
                else:
                    lszName = lszName + "_" + lszItem
                if (liIdx == liIndex):
                    return lszName
            return lszName


        lszPackageRootFolder = self["root_folder"]
        # check if there is a 'CmakeLists.txt' inside the root folder
        lbGenerate = lbForceWrite
        lbCallSubscript = False
        if (os.path.exists(os.path.join(lszPackageRootFolder, "CmakeLists.txt"))):
            lbGenerate = True

        # Optional subscript. Can be usedto overwrite genlist generation
        if (os.path.exists(os.path.join(lszPackageRootFolder, "package.py"))):
            lbGenerate = False
            lbCallSubscript = True

        if self["variant"] == "":
            self.writeln("  Package: " + self.mszName)
        else:
            self.writeln("  Package: " + self.mszName + ":" + self["variant"])

        if (lbGenerate == True):
            # generate the main package file
            lDctPackageFiles = self.GernerateFileGenList_ScanFolder(lszPackageRootFolder, "")
            lLstExcludedFiles = list()
            lLstExcludedFiles = self.GernerateFileGenList_CollectExcluded(lszPackageRootFolder, lLstExcludedFiles)


            # scan all sub variants
            if (self.Exists("variant_folder")):
                if os.path.exists(self["variant_folder"]):
                    lLstVariants = self["variant"].split(":")

                    for liIdx, lszVariant in enumerate(lLstVariants):
                        # print("    " + lszVariant + "  " +  GetVariantFolderbyIndex(lLstVariants, liIdx))
                        lszVarFolder  = GetVariantFolderbyIndex(lLstVariants, liIdx)
                        lszScanFolder = os.path.join(lszPackageRootFolder, lszVarFolder)
                        lszName       = GetVariantNamebyIndex(lLstVariants, liIdx)
                        lDctSubPackageFiles = self.GernerateFileGenList_ScanFolder(lszScanFolder, lszName)
                        lDctPackageFiles.update(lDctSubPackageFiles)
                        lLstExcludedFiles = self.GernerateFileGenList_CollectExcluded(lszScanFolder, lLstExcludedFiles)

                    self.GernerateFileGenList_File(self["variant_folder"], lDctPackageFiles, lLstExcludedFiles, True)
                else:
                    self.warn("", 1, "Package variant path is missing: " + self["variant_folder"])
            else: # no variant, only root
                self.GernerateFileGenList_File(lszPackageRootFolder, lDctPackageFiles, lLstExcludedFiles, True)

        if (lbCallSubscript == True):
            try:
                if (self.mcSystem != None):
                    lszPythonDirAndFile = os.path.join(lszPackageRootFolder, "package.py")
                    lszPythonDir        = lszPackageRootFolder
                    lszPythonFile       = "package.py"
                    lszPythonFileNoExt  = "package"

                    self.writeln("Starting package script: " + lszPythonDirAndFile + "\n")

                    #https://stackoverflow.com/questions/17937402/loading-packages-that-change-sys-path
                    lszSysPath = sys.path[:]

                    sys.path.insert(0, lszPythonDir)
                    loSubScript = __import__(lszPythonFileNoExt)
                    import inspect

                    loSubScriptClassDefinition = None
                    for lszSubScriptName, loSubScriptObj in inspect.getmembers(loSubScript):
                        if inspect.isclass(loSubScriptObj):
                            loSubScriptClassDefinition = loSubScriptObj

                    if (loSubScriptClassDefinition != None):
                        loSubScriptClass = loSubScriptClassDefinition(self.mcSystem, self)
                        loSubScriptClass.run()
                    self.writeln("\nFinished user script: " + lszPythonDirAndFile + "\n")

                    sys.path = lszSysPath

            except:
                self.error("", 1, "Could not run user script", '\n', True)
                pass


    def Enable(self):
        self.mboEnable = True

    def Disable(self):
        self.mboEnable = False
    def IsEnable(self):
        return self.mboEnable

    def toString(self, lszIdent = "    "):
        lszString = self.mszName + "\n"
        for lszKeys, lcValues in self.mcPackage.items():
            lszString += lszIdent + (lszKeys+":").ljust(32) + str(lcValues) + "\n"
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


    def error(self, lszPrompt, liErrorNumber, lszErrorText, end='\n', lbAddTraceback=False):
        if (self.mcErrHdl != None):
            self.mcErrHdl.error(lszPrompt, liErrorNumber, lszErrorText, end, lbAddTraceback)
        else:
            print(lszPrompt + "Error " + str(liErrorNumber) + ": " + lszErrorText)


    def warn(self, lszPrompt, liWarnNumber, lszWarnText):
        if (self.mcErrHdl != None):
            self.mcErrHdl.warn(lszPrompt, liWarnNumber, lszWarnText)
        else:
            print(lszPrompt + "Warning " + str(liWarnNumber) + ": " + lszWarnText)



class tcPackages:
    def __init__(self, lszPackageFolder, lcSystem = None, lcLog = None, lcErrHdl = None):

        self.mlstPackages = list()
        self.mlstPackages.clear()

        self.miValid = 0

        self.mcLog = lcLog
        self.mcErrHdl = lcErrHdl

        self.mszPackageFolder = lszPackageFolder

        self.lszDir_Variant   = "_var"
        self.lszXdm_Extension = ".xdm"

        self.mcSystem = lcSystem


    # Adds the given package to the list of packages
    # lszPackageLabel, e.g.
    #     Dio
    #     Driver/Dio
    #     Driver/Dio:Var1
    # must be located in the [root]/source folder
    def AddPackage(self, lszPackageLabel):

        # Cut Build Options
        liBuildOptionsPos = lszPackageLabel.find("|")
        if liBuildOptionsPos >= 0:
            lszPackageLabel = lszPackageLabel[0:liBuildOptionsPos]

        #print(lszPackageLabel)

        lszPackageLabel = lszPackageLabel.replace("\\", "/")

        liMostLeftDDot  = lszPackageLabel.find(':')
        liMostRightSlash = lszPackageLabel.rfind('/')

        lszPackageRelDir = ""

        # check if there is a ':' in the given package string, to split the variant
        # in case, use the most left ':' to split
        # e.g Driver/Dio.Var1 => Driver/Dio
        if (liMostLeftDDot >= 0):
            lszPackageRelDir = lszPackageLabel[:liMostLeftDDot]
        else:
            lszPackageRelDir = lszPackageLabel

        # check if there is a '/' in the given package string, to split the name from the path
        # in case, use the most right '/' to split
        # e.g Driver/Dio => Dio
        if (liMostRightSlash >= 0):
            lszPackageName = lszPackageRelDir[liMostRightSlash + 1:]
        else:
            lszPackageName = lszPackageRelDir

        lcPackage = tcPackage(lszPackageName, self.mcSystem, self.mcLog, self.mcErrHdl)

        lcPackage["Original_Str"] = lszPackageLabel

        # check if there is a ':' in the given package string, to split the variant
        # in case, use the most left ':' to split
        # e.g Driver/Dio.Var1 => Var1
        if (liMostLeftDDot >= 0):
            lcPackage["variant"] = lszPackageLabel[liMostLeftDDot + 1:]

        lcPackage["Root_Folder"] = os.path.abspath(os.path.join(self.mszPackageFolder, lszPackageRelDir))

        # if the package has a variant defined, _var/[variant] folder
        lszVariantFolder = ""
        if (lcPackage.Exists("variant")):
            lszVariant = lcPackage["variant"]
            lszVariantFolder = lszVariant.replace(":", "/" + self.lszDir_Variant + "/")
            lszVariantFolder = os.path.normpath(lszVariantFolder)
            lszVariantFolder = os.path.join(lcPackage["Root_Folder"], self.lszDir_Variant, lszVariantFolder)
            lcPackage["variant_folder"] = lszVariantFolder

        # check if there is a '_PkgCfg.txt' variant file in the root or _var/[variant] folder
        # Note: Config of _var/[variant] would overwrite root config
        lszPkgCfgRootFolder = os.path.abspath(os.path.join(lcPackage["Root_Folder"], "_PkgCfg.txt"))
        lszPkgCfgVarFolder  = os.path.abspath(os.path.join(lcPackage["variant_folder"], "_PkgCfg.txt"))

        if os.path.exists(lszPkgCfgVarFolder) and (lszVariantFolder != ""):
            lcPackage["File_Config"] = lszPkgCfgVarFolder

            # try to open the config files
            try:
                loPackageConfig = configparser.ConfigParser()
                loPackageConfig.read(lszPkgCfgVarFolder)

                loPackageConfigDict = dict()
                for loSect in loPackageConfig.sections():
                    loPackageConfigDict[loSect] = {}
                    for name, value in loPackageConfig.items(loSect):
                        loPackageConfigDict[loSect][name] = value

                lcPackage["data_config"] = loPackageConfigDict
            except:
                pass
        else:
            if os.path.exists(lszPkgCfgRootFolder):
                lcPackage["File_Config"] = lszPkgCfgRootFolder

                # try to open the config files
                try:
                    loPackageConfig = configparser.ConfigParser()
                    loPackageConfig.read(lszPkgCfgRootFolder)

                    loPackageConfigDict = dict()
                    for loSect in loPackageConfig.sections():
                        loPackageConfigDict[loSect] = {}
                        for name, value in loPackageConfig.items(loSect):
                            loPackageConfigDict[loSect][name] = value

                    lcPackage["data_config"] = loPackageConfigDict
                except:
                    pass


        # Check, if package name is given
        lszPackageName = lcPackage.Get_Config("Common.PackageName")

        lcPackage["package_name"] = lszPackageName
        lcPackage.mszName = lszPackageName

        self.mlstPackages.append(lcPackage)


    def LoadPackageList(self, lszPackageListFile):
        try:

            # Open the PackageListFile
            self.writeln("Open Package List: " + lszPackageListFile)
            if (os.path.isfile(lszPackageListFile)):
                # Just read to list
                self.mlstPackages.clear()

                with open(lszPackageListFile) as loPackageListFile:
                    for lszLine in loPackageListFile:
                        lszLine = lszLine.strip()
                        if (lszLine != ""):
                            self.AddPackage(lszLine)

                self.miValid = 1

            else:
                self.error("Error:", 1 ,"File not found: " + lszPackageListFile)

            # sort the list by package name
            self.mlstPackages.sort(key=lambda tcPackage: tcPackage.mszName.lower())

        except (Exception, ArithmeticError) as e:
            template = "An exception of type {0} occurred. Arguments:\n{1!r}"
            message = template.format(type(e).__name__, e.args)
            self.writeln(message)


    def CreateGenLists(self, lbForceWrite = False):
        for lcPackage in self.mlstPackages:
            lcPackage.GernerateFileGenList(lbForceWrite)

    def __getitem__(self, lszPackage):
        for lcPackage in self.mlstPackages:
            if (lcPackage.mszName.lower() == lszPackage.lower()):
                return lcPackage
        return None


    def toString(self):
        lszString = ""
        if (self.isValid() == 1):
            lszString = "Packages:\n"
            lszIndex = 0
            for lcPackage in self.mlstPackages:
                if (lcPackage.mboEnable):
                    lszString += "  " + lcPackage.toString()
        return lszString



    def Exists(self, lszPackage):
        for lcPackage in self.mlstPackages:
            if (lcPackage.mszName.lower() == lszPackage.lower()):
                return True
        return False

    def isValid(self):
        return self.miValid

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

# Test
#Test
if __name__ == "__main__":
    isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ
    if isRunningInPyCharm:

        from PyClass_System import tcSystem
        lcSystem = tcSystem("","","","")

        lcPackages = tcPackages("U:\\E2\\Projekte\\SW\\Code\\pcb\\source", lcSystem)
        lcPackages.LoadPackageList("U:\\E2\\Projekte\\SW\\Code\\pcb\\bin\\STM\\F4\\STM32F429ZIT6\\STM32F429IDISCOVERY\\LedBlink\\PG13T\\Debug\\gen\\PkgList.txt")
        print(lcPackages["std"].mszName)
        lcPackages["std"].GernerateFileGenList()

