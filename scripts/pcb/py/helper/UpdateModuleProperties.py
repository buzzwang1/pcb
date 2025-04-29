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
        self.mcFh.ListTree(self.mszDirSource, lLstFiles, "module.properties")

        for lszFileSource in lLstFiles:

            lszFileDest = lszFileSource.lower().replace("module.properties", "package.properties")

            with open(lszFileSource, "rt") as fin:
                with open(lszFileDest, "wt") as fout:
                    for line in fin:
                        if (line.lower().find("modulename") >= 0):
                            fout.write("PackageName=" + line[len("modulename="):])
                        else:
                            fout.write(line)

            self.mcFh.Delete(lszFileSource)


if __name__ == "__main__":
    lcUpdateModuleProperties = cUpdateModuleProperties()
    lcUpdateModuleProperties.run()

