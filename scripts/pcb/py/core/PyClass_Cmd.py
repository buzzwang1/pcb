#!/usr/local/bin/python
#    Filename : PyClass_Cmd.py
# Description : Simple class to call other programs
#               There are still problems with the encoding,
#                 e.g. german Umlaute 'äöü' are not correct displayed


import os
import sys
import traceback
import asyncio
from asyncio.subprocess import PIPE

from PyClass_Log import tcLog
from PyClass_ErrorHandler import tcErrorHandler

class tcCmd():
    def __init__(self, lcLog = None, lcErrHdl = None):
        self.mcLog = lcLog
        self.mcErrHdl  = lcErrHdl
        self.Clear()

    def Clear(self):
        self.mszCallOutput = ""

    def Get(self) ->str:
        return self.mszCallOutput

    async def DisplayStream(self, loStream, liMode):

        lLstOutput = []

        # https://www.python-forum.de/viewtopic.php?t=5095
        try:
            loEncoding = sys.stdout.encoding or sys.getfilesystemencoding()
        except:
            try:
                loEncoding = sys.stdout.encoding
            except:
                loEncoding = "utf-8"

        while True:
            lbaLine = await loStream.readline()
            if not lbaLine:
                break

            lLstOutput.append(lbaLine)
            try:
                lszLine = lbaLine.decode(loEncoding, "ignore")
                lszLine = lszLine.replace("\r\n", "\n")
                self.mszCallOutput += lszLine

                if liMode == 1:
                    if (lszLine.strip() != ""):
                        self.error("", 1, lszLine)
                else:
                    self.write_grey(lszLine)
            except:
                lszLine = "Could not decode"
                self.error("", 1, lszLine)

        return b''.join(lLstOutput)

    async def StartCmdInShell(self, lszCmd, lszPath):

        if (lszPath.strip()==""):
            loProcess = await asyncio.create_subprocess_shell(lszCmd, stdout=PIPE, stderr=PIPE)
        else:
            loProcess = await asyncio.create_subprocess_shell(lszCmd, cwd=lszPath, stdout=PIPE, stderr=PIPE)

        try:
            stdout, stderr = await asyncio.gather(self.DisplayStream(loProcess.stdout, 0), self.DisplayStream(loProcess.stderr, 1))
        except Exception:
            loProcess.kill()
            raise
        finally:
            # wait for the process to exit
            liReturnCode = await loProcess.wait()

        return liReturnCode

    def Call(self, lszExec, lszPath = ""):
        #if needed, add separator
        lszPath = lszPath.strip()
        if (lszPath != ""):
            lszPath = lszPath.strip(os.path.sep)
            lszPath += os.path.sep

        self.writeln("\nStarting: " + lszPath + lszExec)
        self.writeln("")

        try:
            # run the event loop
            if os.name == 'nt':
                loLoop = asyncio.ProactorEventLoop()  # for subprocess' pipes on Windows
                asyncio.set_event_loop(loLoop)
            else:
                loLoop = asyncio.get_event_loop()

            liReturnCode = loLoop.run_until_complete(self.StartCmdInShell(lszExec, lszPath))
            loLoop.close()

            self.writeln("\nFinished: " + lszPath + lszExec)

            return liReturnCode


        except:
            self.error("", 1, "Error when calling " + lszPath + lszExec + " ".join(traceback.format_exception(*sys.exc_info())[-2:]).strip().replace('\n', ': '))
            self.writeln("\nFinished: " + lszPath + lszExec)
            return -1

    def write_grey(self, lszString):
        if (self.mcLog != None):
            self.mcLog.write(lszString, self.mcLog.GREY)
        else:
            print(lszString, end='')

    def write(self, lszString):
        if (self.mcLog != None):
            self.mcLog.write(lszString)
        else:
            print(lszString, end='')

    def writeln_grey(self, lszString):
        if (self.mcLog != None):
            self.mcLog.writeln(lszString, self.mcLog.GREY)
        else:
            print(lszString)

    def writeln(self, lszString):
        if (self.mcLog != None):
            self.mcLog.writeln(lszString)
        else:
            print(lszString)

    def error(self, lszPrompt, liErrorNumber, lszErrorText, end='', lbAddTraceback = False):
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
if __name__ == "__main__":
    isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ
    if isRunningInPyCharm:
        dir_this  = os.path.dirname(os.path.abspath(os.path.join(__file__, "../../..")))
        lcCmd = tcCmd()

        #execute win/Dos Dir-Cmd
        lcCmd.Clear()
        lcCmd.Call("dir")

        print("\n\n")
        print(lcCmd.Get())
