# Will be called by toolcaller via the project's configuration
# System class is passed, to get the project to get access to the projects's configuration


import os, shutil, sys, re, collections, glob, ntpath, fnmatch

class MyClass:
    def __init__(self, lcSystem):
        self.mcSystem  = lcSystem
        self.mcProject = self.mcSystem.Project_Get()
        self.mszCopyDst = ""

        self.lbDebug = True


    def GernerateFileGenList_File(self, lcPackage, lszDestFolder, lDctFiles, lLstExcludedFiles):
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

        def CheckForLongPath(lcPackage, lLstSource, liWarnMaxLength):
            # Check for files with to long path
            for lszFile in lLstSource:
                if len(lcPackage["root_folder"] + lszFile) > liWarnMaxLength:
                    self.warn("", 1, "Path length (" + str(liWarnMaxLength) +"): " + str(len(lszFile)) + ": " + self["root_folder"] + lszFile)

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
            CheckForLongPath(lcPackage, lLstFileList, liWarnMaxLength)

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


        lszSelfRoot = lcPackage["root_folder"]
        for lszExclude in lLstExcludedFiles:
            if (lszExclude.strip() != ""):
                # check, if it is a existing file
                lbExcluded = False
                lszCheckForFile = lcPackage["root_folder"] + '/' + lszExclude
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

        for lszName, lLstFiles in lDctFiles.items():
            if lszName.find(lszLabel_SourceFiles) >= 0:
                for lszItem in lLstFiles:
                    lszSrcFile = os.path.abspath(os.path.join(lcPackage["root_folder"], lszItem))
                    lszPgk = lcPackage["original_str"].replace(":"+lcPackage["variant"],"")
                    lszDstFile = os.path.abspath(os.path.join(self.mszCopyDst, lszPgk, lszItem))
                    self.mcSystem.mcFile.Copy(lszSrcFile, lszDstFile)

        for lszName, lLstFiles in lDctFiles.items():
            if lszName.find(lszLabel_HeaderFiles) >= 0:
                for lszItem in lLstFiles:
                    lszSrcFile = os.path.abspath(os.path.join(lcPackage["root_folder"], lszItem))
                    lszPgk = lcPackage["original_str"].replace(":"+lcPackage["variant"],"")
                    lszDstFile = os.path.abspath(os.path.join(self.mszCopyDst, lszPgk, lszItem))
                    self.mcSystem.mcFile.Copy(lszSrcFile, lszDstFile)

        for lszName, lLstFiles in lDctFiles.items():
            if lszName.find(lszLabel_AsmFiles) >= 0:
                for lszItem in lLstFiles:
                    lszSrcFile = os.path.abspath(os.path.join(lcPackage["root_folder"], lszItem))
                    lszPgk = lcPackage["original_str"].replace(":"+lcPackage["variant"],"")
                    lszDstFile = os.path.abspath(os.path.join(self.mszCopyDst, lszPgk, lszItem))
                    self.mcSystem.mcFile.Copy(lszSrcFile, lszDstFile)



    # Generate a "genlists.cmake" in the package root folder, in case a "CmakeLists.txt" exists in the same folder or if forced (lbForceCreate = True)
    def GernerateFileGenList(self, lcPackage):

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


        lszPackageRootFolder = lcPackage["root_folder"]
        # check if there is a 'CmakeLists.txt' inside the root folder
        lbCallSubscript = False
        lbGenerate = True
        if (os.path.exists(os.path.join(lszPackageRootFolder, "CmakeLists.txt"))):
            lbGenerate = True

        # Optional subscript. Can be usedto overwrite genlist generation
        if (os.path.exists(os.path.join(lszPackageRootFolder, "package.py"))):
            lbGenerate = False
            lbCallSubscript = True

        if lcPackage["variant"] == "":
            lcPackage.writeln("  Package: " + lcPackage.mszName)
        else:
            self.writeln("  Package: " + lcPackage.mszName + ":" + lcPackage["variant"])

        if (lbGenerate == True):
            # generate the main package file
            lDctPackageFiles = lcPackage.GernerateFileGenList_ScanFolder(lszPackageRootFolder, "")
            lLstExcludedFiles = list()
            lLstExcludedFiles = lcPackage.GernerateFileGenList_CollectExcluded(lszPackageRootFolder, lLstExcludedFiles)


            # scan all sub variants
            if (lcPackage.Exists("variant_folder")):
                if os.path.exists(lcPackage["variant_folder"]):
                    lLstVariants = lcPackage["variant"].split(":")

                    for liIdx, lszVariant in enumerate(lLstVariants):
                        # print("    " + lszVariant + "  " +  GetVariantFolderbyIndex(lLstVariants, liIdx))
                        lszVarFolder  = GetVariantFolderbyIndex(lLstVariants, liIdx)
                        lszScanFolder = os.path.join(lszPackageRootFolder, lszVarFolder)
                        lszName       = GetVariantNamebyIndex(lLstVariants, liIdx)
                        lDctSubPackageFiles = lcPackage.GernerateFileGenList_ScanFolder(lszScanFolder, lszName)
                        lDctPackageFiles.update(lDctSubPackageFiles)
                        lLstExcludedFiles = lcPackage.GernerateFileGenList_CollectExcluded(lszScanFolder, lLstExcludedFiles)

                    self.GernerateFileGenList_File(lcPackage, lcPackage["variant_folder"], lDctPackageFiles, lLstExcludedFiles)
                else:
                    self.warn("", 1, "Package variant path is missing: " + lcPackage["variant_folder"])
            else: # no variant, only root
                self.GernerateFileGenList_File(lcPackage, lszPackageRootFolder, lDctPackageFiles, lLstExcludedFiles)


    def run(self, lszParameter):

        # Get the data from the configuration
        dir_root =        self.mcProject.isKeyOrDie("system/dir/root")
        dir_cfg_project = self.mcProject.isKeyOrDie("system/dir/project/cfg/root")
        lszProjectName  = self.mcProject.GetName()
        self.debug(lszProjectName)

        lszStrip_src                 = self.mcProject.isKeyOrDie("system/dir/source")
        lszStrip_build_gen           = self.mcProject.isKeyOrDie("system/dir/project/build/gen")
        lszStrip_build_gen_strip     = os.path.join(lszStrip_build_gen, "strip")
        lszStrip_PkgLst              = self.mcProject.isKeyOrDie("codegen/CreatedPkgList")
        self.mszCopyDst              = lszStrip_build_gen_strip

        self.mcSystem.Call(self.mcProject.isKeyOrDie("codegen/cmake_create_pkg_list/call"), "")

        self.mcSystem.mcFile.CreateDirForFile(lszStrip_build_gen_strip)


        self.debug("")
        self.debug("Copying project files: " + dir_cfg_project)
        self.mcSystem.mcFile.CopyTree(dir_cfg_project, lszStrip_build_gen_strip)

        self.debug("")
        self.debug("Copying source files")


        from PyClass_Packages import tcPackages
        lcPackages = tcPackages(lszStrip_src, self.mcSystem, self.mcSystem.mcLog, self.mcSystem.mcErrHdl)
        lcPackages.LoadPackageList(lszStrip_PkgLst)

        #self.debug(lcPackages.toString())
        for lcPackage in lcPackages.mlstPackages:
            if (lcPackage.mboEnable):
                #self.debug(lcPackage.toString())
                self.GernerateFileGenList(lcPackage)


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

