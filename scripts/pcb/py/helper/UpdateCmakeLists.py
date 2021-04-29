# Helper Scripts
# Will Replace the Cmakelists below source by the given

import os, shutil, sys, re, json

class cUpdateCMakeLists:
    def __init__(self):

        self.mszDirFile    = os.path.dirname(os.path.realpath(__file__))
        self.mszDirRoot    = os.path.abspath(os.path.join(self.mszDirFile, "..", "..", "..", ".."))
        self.mszDirSource  = os.path.abspath(os.path.join(self.mszDirRoot, "source"))
        self.mszDirScripts = os.path.abspath(os.path.join(self.mszDirRoot, "scripts", "pcb", "py", "core"))

        self.mszCMakeListsTemplate = os.path.abspath(os.path.join(self.mszDirSource, "_Template", "CmakeLists.txt"))

        sys.path.insert(0, self.mszDirScripts)
        from PyClass_FileHandler import tcFileHandler

        self.mcFh = tcFileHandler()

    def run(self):
        lLstFiles = list()
        self.mcFh.ListTree(self.mszDirSource, lLstFiles, "cmakelists.txt")

        # Remove the first Item from the list
        lLstFiles = lLstFiles[1:]

        for lszFile in lLstFiles:
            if (self.mszCMakeListsTemplate.lower() != lszFile.lower()):
                self.mcFh.Copy(self.mszCMakeListsTemplate, lszFile)


if __name__ == "__main__":
    lcUpdateCMakeLists = cUpdateCMakeLists()
    lcUpdateCMakeLists.run()

