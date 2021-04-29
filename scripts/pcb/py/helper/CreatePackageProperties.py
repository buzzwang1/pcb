# Helper Scripts
# Will rename module.properties to package.properties and his contents

import os, shutil, sys, re, json

class cUpdateModuleProperties:
    def __init__(self):

        self.mszDirFile    = os.path.dirname(os.path.realpath(__file__))
        self.mszDirRoot    = os.path.abspath(os.path.join(self.mszDirFile, "..", "..", "..", ".."))
        self.mszDirSource  = os.path.abspath(os.path.join(self.mszDirRoot, "source"))
        self.mszDirScripts = os.path.abspath(os.path.join(self.mszDirRoot, "scripts", "pcb", "py", "core"))

        sys.path.insert(0, self.mszDirScripts)
        from PyClass_FileHandler import tcFileHandler

        self.mcFh = tcFileHandler()

    def run(self):
        lLstFiles = list()
        self.mcFh.ListTree(self.mszDirSource, lLstFiles,  "cmakelists.txt")

        # Remove the first Item from the list
        lLstFiles = lLstFiles[1:]

        for lszFileSource in lLstFiles:
            lszFileDest = lszFileSource.lower().replace("cmakelists.txt", "")
            lszFileDest = os.path.abspath(os.path.join(lszFileDest, "adapt", "package.properties"))

            if (not os.path.exists(lszFileDest)):
                self.mcFh.CreateDirForFile(lszFileDest)

                with open(lszFileDest, "wt") as fout:
                    fout.write("[Common]\n")
                    fout.write("PackageName=")



if __name__ == "__main__":
    lcUpdateModuleProperties = cUpdateModuleProperties()
    lcUpdateModuleProperties.run()

