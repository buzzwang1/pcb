#!/usr/local/bin/python
#    Filename : PyClass_Env.py
# Description : Class to manage Environment variables


import os
import sys
import argparse
import re
import fileinput
import ConfigParser # to parse .ini files
import subprocess # batch handling
import itertools
import traceback
import collections

from PyClass_FileStatus import tcFileStatus
from PyClass_Log import tcLog
from PyClass_ErrorHandler import tcErrorHandler

from PyClass_SetEnv import tcSetEnv

"""
  Class to manage Environment variables(like paths, files, data) by key and value in a dictionary.
    e.g. self.mcEnv[Base] returns the system base folder

"""

# Environment variables, mainly used to organize folder and files by a key

class tcEnv:
    def __init__(self, lszSysBaseFolder, lcLog = None, lcErrHdl = None):
        self.mcDictEnv = dict()
        self.miValid = 0

        self.mcLog = lcLog
        self.mcErrHdl = lcErrHdl

        if (self.CheckAndUpdatePath("Base", lszSysBaseFolder) == 0):
            self.miValid = 1


    def CheckAndUpdatePath(self, lszKey, lszPath):
        lcStatus = tcFileStatus(lszPath)
        if (lcStatus.isExisting()):
            self.UpdatePath(lszKey, lszPath)
            return 0
        else:
            return 1


    def UpdatePath(self, lszKey, lszPath):
        lszPath = lszPath.replace("/", "\\") + "\\"
        lszPath = lszPath.replace("\\\\", "\\")
        self.mcDictEnv[lszKey] = lszPath

    def UpdatePathRelTo(self, lszRefernceKey, lszKey, lszPathRel):
        lszPath = self.Get(lszRefernceKey)+lszPathRel
        lszPath = lszPath.replace("/", "\\") + "\\"
        lszPath = lszPath.replace("\\\\", "\\")
        self.mcDictEnv[lszKey] = lszPath

    def CheckAndUpdateFile(self, lszKey, lszFile):
        lcStatus = tcFileStatus(lszFile)
        if (lcStatus.isExisting()):
            self.UpdateFile(lszKey, lszFile)
            return 0
        else:
            return 1

    def UpdateFile(self, lszKey, lszFile):
        lszFile = lszFile.replace("/", "\\")
        lszFile = lszFile.replace("\\\\", "\\")
        self.mcDictEnv[lszKey] = lszFile

    def UpdateFileRelTo(self, lszRefernceKey, lszKey, lszFile):
        lszFile = self.Get(lszRefernceKey) + lszFile
        lszFile = lszFile.replace("/", "\\")
        lszFile = lszFile.replace("\\\\", "\\")
        self.mcDictEnv[lszKey] = lszFile


    def Remove(self, lszKey):
        if (lszKey in self.mcDictEnv):
            del self.mcDictEnv[lszKey]


    def Get(self, lszKey):
        return self[lszKey]

    def __setitem__(self, lszKey, lszValue):
        self.Remove(lszKey)
        self.mcDictEnv[lszKey] = lszValue


    def __getitem__(self, lszKey):
        if (lszKey in self.mcDictEnv):
            return self.mcDictEnv[lszKey]
        else:
            return ""


    def LoadWindowsEnvVariables(self):
        for lszKey in os.environ:
            self["OS_" + lszKey.strip()] = os.environ[lszKey].strip()


    def LoadKeyFile(self, lszKeyFile):
        # Check if File existing
        if (lszKeyFile != None):

            self.writeln("System Ini: Open File " + lszKeyFile)
            lcFileStatus = tcFileStatus(lszKeyFile)
            if (lcFileStatus.isExisting() == 1):

                try:
                    self.writeln("  => Read data")

                    CfgParser = ConfigParser.RawConfigParser()
                    #To keep upper/lower case
                    CfgParser.optionxform = str

                    CfgParser.read(lszKeyFile)

                    lcDict = dict(CfgParser.items("SystemEnvironment"))

                    for lszKey, lcValues in lcDict.items():
                        if ("[Base]" in lcValues):
                            lcValues = lcValues.replace("[Base]", self.mcDictEnv["Base"])
                            self.UpdateFile(lszKey.strip(),  lcValues.replace("\"", "").strip())
                        else:
                            self[lszKey.strip()] = lcValues.replace("\"", "").strip()

                    self.writeln("  => OK")
                    return 0

                except:
                    self.error("  => ", 2, "Unexpected error! ".join(traceback.format_exception(*sys.exc_info())[-2:]).strip().replace('\n', ': '))
                    self.mszValidate = ""
                    return 1
            else:
                self.error("  => ", 1, "File not found")
                return 1
        else:
            self.error("  => ", 1, "File not found")
            return 1



    def Load_SetEnvHost_Bat(self, lszFile, lszPath, lszKeyPreFix, initial=None):
        # http://stackoverflow.com/questions/1214496/how-to-get-environment-from-a-subprocess-in-python
        def validate_pair(ob):
            try:
                if not (len(ob) == 2):
                    # print("Unexpected result:", ob, file=sys.stderr)
                    raise ValueError
            except:
                return False
            return True

        def consume(iter):
            try:
                while True: next(iter)
            except StopIteration:
                pass

        """
        Take a command (either a single command or list of arguments)
        and return the environment created after running that command.
        Note that if the command must be a batch file or .cmd file, or the
        changes to the environment will not be captured.

        If initial is supplied, it is used as the initial environment passed
        to the child process.
        """
        self.writeln("Load: " + lszFile)

        if not isinstance(lszFile, (list, tuple)):
            lszFile = [lszFile]
        # construct the command that will alter the environment
            lszFile = subprocess.list2cmdline(lszFile)
        # create a tag so we can tell in the output when the proc is done
        tag = 'Done running command'
        # construct a cmd.exe command to do accomplish this
        cmd = 'cmd.exe /s /c "{lszFile} && echo "{tag}" && set"'.format(**vars())
        # launch the process
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, cwd=lszPath, shell=True, env=initial)
        # parse the output sent to stdout
        lines = proc.stdout
        # consume whatever output occurs until the tag is reached
        consume(itertools.takewhile(lambda l: tag not in l, lines))
        # define a way to handle each KEY=VALUE line
        handle_line = lambda l: l.rstrip().split('=', 1)
        # parse key/values into pairs
        pairs = map(handle_line, lines)
        # make sure the pairs are valid
        valid_pairs = filter(validate_pair, pairs)
        # construct a dictionary of the pairs
        result = dict(valid_pairs)
        # let the process finish
        proc.communicate()

        for lszKey, lszValues in result.items():
            self[lszKeyPreFix + lszKey] = lszValues

    def toString(self):
        lszString = ""
        for lszKeys, lcValues in self.mcDictEnv.items():
            lszString += (lszKeys+":").ljust(32) + lcValues + "\n"
        return lszString

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
# lcLog = tcLog(None, "w")
# lcErr = tcErrorHandler(lcLog)
#
# lszCurrentDir = os.path.dirname(os.path.realpath(__file__))
#
# lcEnv = tcEnv(lszCurrentDir, lcLog, lcErr)
# lcEnv["MySetEnv"] = "setenv_host.bat"
#
# lcEnv.LoadWindowsEnvVariables() # Load Windows environment variables
# lcEnv.LoadKeyFile(lcEnv["Base"] + "TestKeyFile.ini")
# lcEnv.Load_SetEnvHost_Bat(lcEnv["MySetEnv"],lcEnv["Base"], "SetEnvHost_")
#
# print lcEnv.toString()

