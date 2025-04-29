#!/usr/local/bin/python
#    Filename : PyClass_ErrorHandler.py
# Description : Class for log errors
# 				Class to write a Error in the given Log class
# 				This is used to have an certain syntax.


import sys
import traceback

from PyClass_Log import tcLog

class tcErrorHandler:
    def __init__(self, lcLog = None):
        self.mcLog = lcLog

    def error(self, lszPrompt, liErrorNumber, lszErrorText, end='\n', lbAddTraceback = False):

        lszTraceback = ""
        if (lbAddTraceback):
            lszTraceback = "\nError details:\n" + traceback.format_exc()

        if (self.mcLog != None):
            self.mcLog.write(lszPrompt + "Error " + str(liErrorNumber) + ": " + lszErrorText + lszTraceback + end, self.mcLog.RED)
        else:
            print(lszPrompt + "Error " + str(liErrorNumber) + ": " + lszErrorText + lszTraceback)

    def write(self, lszPrompt, liErrorNumber, lszErrorText, end='\n', lbAddTraceback=False):
        self.error(lszPrompt, liErrorNumber, lszErrorText, end, lbAddTraceback)

    def warn(self, lszPrompt, liWarnNumber, lszWarnText, end='\n'):
        if (self.mcLog != None):
            self.mcLog.write(lszPrompt + "Warning " + str(liWarnNumber) + ": " + lszWarnText + end,
                             self.mcLog.YELLOW)
        else:
            print(lszPrompt + "Warning " + str(liWarnNumber) + ": " + lszWarnText)


# Test
if __name__ == "__main__":
    isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ
    if isRunningInPyCharm:
        lcLog = tcLog("Log.txt", "w", "")
        lcErr = tcErrorHandler(lcLog)
        lcErr.error("  => ", 123, "Shit happen's")
        lcErr.warn("  => ", 123, "Shit happen's")

        del lcErr
        lcErr = tcErrorHandler()
        try:
            raise TypeError("Oups!")
        except:
            lcErr.error("  => ", 123, "Shit happen's", '\n', True)

        del lcErr
        import os
        # os.remove("Log.txt")









