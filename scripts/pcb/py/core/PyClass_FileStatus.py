#!/usr/local/bin/python
#    Filename : PyClass_FileStatus.py
# Description : Class to return the status of the given file


import sys
import os
import stat
import datetime

class tcFileStatus:
    def __init__(self, lszFilename):
        self.mszStatus = ""
        self.mbWriteProtected = True
        self.mbExists = True
        self.moTimeModified = datetime.datetime.min

        if not os.access(lszFilename, os.F_OK):
            self.mszStatus += "NOEXISTS"
            self.mbExists = False

        if (self.mbExists):
            self.moTimeModified = datetime.datetime.fromtimestamp(os.path.getmtime(lszFilename))

            if (os.access(lszFilename, os.R_OK)):
                self.mszStatus += "READ,"
            if (os.access(lszFilename, os.W_OK)):
                self.mszStatus += "WRITE,"
                self.mbWriteProtected = False
            if (os.access(lszFilename, os.X_OK)):
                self.mszStatus += "EXECUTE"
            elif os.access(lszFilename, os.F_OK) and not \
                    (os.access(lszFilename, os.R_OK)) and not \
                    (os.access(lszFilename, os.W_OK)) and not \
                    (os.access(lszFilename, os.X_OK)):
                self.mszStatus = "NOACCESS"

    def isWriteProtected(self):
        return self.mbWriteProtected

    def isExisting(self):
        return self.mbExists

    def GetTimeModified(self):
        return self.moTimeModified

    def GetStatus(self) ->str:
        return self.mszStatus

    def toString(self):
        return self.mszStatus
