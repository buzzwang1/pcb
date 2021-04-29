#!/usr/local/bin/python
#    Filename : PyClass_Log.py
# Description : Class to output strings to the console and optional log them to a given text file.
#               The string is written to a given text file, in case it is not set to None
#               whereby call Init with "a" append the text to a given file
#                                      "w" creates a new file
#               lszMode: "silent": No text is output to the console
#               lbColor: True: Will activate colored output (Windows only)


import sys
import os
import stat
import datetime

from PyClass_FileStatus import tcFileStatus

if os.name == 'nt': # Windows ?
    from ctypes import windll, Structure, c_short, c_ushort, byref

    SHORT = c_short
    WORD = c_ushort

    class COORD(Structure):
        """struct in wincon.h."""
        _fields_ = [
            ("X", SHORT),
            ("Y", SHORT)]


    class SMALL_RECT(Structure):
        """struct in wincon.h."""
        _fields_ = [
            ("Left", SHORT),
            ("Top", SHORT),
            ("Right", SHORT),
            ("Bottom", SHORT)]


    class CONSOLE_SCREEN_BUFFER_INFO(Structure):
        """struct in wincon.h."""
        _fields_ = [
            ("dwSize", COORD),
            ("dwCursorPosition", COORD),
            ("wAttributes", WORD),
            ("srWindow", SMALL_RECT),
            ("dwMaximumWindowSize", COORD)]


class tcColorConsole:
    def __init__(self):
        self.FOREGROUND_BLACK     = 0
        self.FOREGROUND_BLUE      = 0
        self.FOREGROUND_GREEN     = 0
        self.FOREGROUND_CYAN      = 0
        self.FOREGROUND_RED       = 0
        self.FOREGROUND_MAGENTA   = 0
        self.FOREGROUND_YELLOW    = 0
        self.FOREGROUND_GREY      = 0
        self.FOREGROUND_INTENSITY = 0

        self.BACKGROUND_BLACK     = 0
        self.BACKGROUND_BLUE      = 0
        self.BACKGROUND_GREEN     = 0
        self.BACKGROUND_CYAN      = 0
        self.BACKGROUND_RED       = 0
        self.BACKGROUND_MAGENTA   = 0
        self.BACKGROUND_YELLOW    = 0
        self.BACKGROUND_GREY      = 0
        self.BACKGROUND_INTENSITY = 0

        self.miDefaultCol = 0

        if os.name == 'nt': # Windows ?
            self.isWindows = True
            # winbase.h
            STD_OUTPUT_HANDLE = -11

            # wincon.h
            self.FOREGROUND_BLACK     = 0x0000
            self.FOREGROUND_BLUE      = 0x0001
            self.FOREGROUND_GREEN     = 0x0002
            self.FOREGROUND_CYAN      = 0x0003
            self.FOREGROUND_RED       = 0x0004
            self.FOREGROUND_MAGENTA   = 0x0005
            self.FOREGROUND_YELLOW    = 0x0006
            self.FOREGROUND_GREY      = 0x0007
            self.FOREGROUND_INTENSITY = 0x0008  # foreground color is intensified.

            self.BACKGROUND_BLACK     = 0x0000
            self.BACKGROUND_BLUE      = 0x0010
            self.BACKGROUND_GREEN     = 0x0020
            self.BACKGROUND_CYAN      = 0x0030
            self.BACKGROUND_RED       = 0x0040
            self.BACKGROUND_MAGENTA   = 0x0050
            self.BACKGROUND_YELLOW    = 0x0060
            self.BACKGROUND_GREY      = 0x0070
            self.BACKGROUND_INTENSITY = 0x0080  # background color is intensified.

            self.stdout_handle = windll.kernel32.GetStdHandle(STD_OUTPUT_HANDLE)
            self.SetConsoleTextAttribute = windll.kernel32.SetConsoleTextAttribute
            self.GetConsoleScreenBufferInfo = windll.kernel32.GetConsoleScreenBufferInfo

            self.miDefaultCol = self.FOREGROUND_GREY | self.FOREGROUND_INTENSITY

    def get_text_attr(self):
        """Returns the character attributes (colors) of the console screen
        buffer."""
        if (self.isWindows == True):  # Windows ?
            csbi = CONSOLE_SCREEN_BUFFER_INFO()
            self.GetConsoleScreenBufferInfo(self.stdout_handle, byref(csbi))
            return csbi.wAttributes
        return 0

    def set_text_attr(self, color):
        """Sets the character attributes (colors) of the console screen
        buffer. Color is a combination of foreground and background color,
        foreground and background intensity."""
        if (self.isWindows == True):  # Windows ?
            self.SetConsoleTextAttribute(self.stdout_handle, color)

    def write(self, lszString, liColor = 15):
        if (self.isWindows == True):  # Windows ?
            self.set_text_attr(liColor)

        print(lszString, end='')

        if (self.isWindows == True):  # Windows ?
            self.set_text_attr(self.miDefaultCol)


class tcLog:
    def __init__(self, lszFilename="", lszModeCreation="a", lszMode="", lbColor=True):

        self.mszFilename = lszFilename
        if (self.mszFilename==None):
            self.mszFilename = ""


        self.moLogFile = None

        self.mlstLogList = list()  # if the log file does not exit, mlstLogList will keep the log data till the log file is created by the user.

        lszMode = lszMode.lower()
        self.mbSilent = False

        if (lszMode.find("silent") >= 0):
            self.mbSilent = True

        self.open(lszFilename, lszModeCreation)

        self.mcCons  = None
        self.RED     = 0
        self.GREY    = 0
        self.WHITE   = 0
        self.YELLOW  = 0
        self.DEFAULT = 0
        self.GREEN   = 0
        self.CYAN    = 0

        if (lbColor):
            self.mcCons = tcColorConsole()
            self.RED   = self.mcCons.FOREGROUND_RED | self.mcCons.FOREGROUND_INTENSITY
            self.GREY =  self.mcCons.FOREGROUND_GREY
            self.WHITE = self.mcCons.FOREGROUND_GREY | self.mcCons.FOREGROUND_INTENSITY
            self.DEFAULT = self.WHITE
            self.GREEN = self.mcCons.FOREGROUND_GREEN | self.mcCons.FOREGROUND_INTENSITY
            self.CYAN = self.mcCons.FOREGROUND_CYAN
            self.YELLOW = self.mcCons.FOREGROUND_YELLOW | self.mcCons.FOREGROUND_INTENSITY

    def isLogFileOpen(self):
        if (self.moLogFile != None):
            if (self.moLogFile.closed != True):
                return True
        return False

    def open(self, lszFilename="", lszModeCreation="a"):

        self.mszFilename = lszFilename
        if (self.mszFilename==None):
            self.mszFilename = ""

        if (self.isLogFileOpen()):
            self.close()

        if (lszModeCreation != "a"):
            lszModeCreation = "w"

        if (self.mszFilename != ""):

            lcFileStatus = tcFileStatus(self.mszFilename)

            if (lcFileStatus.isExisting() == 1):
                if (lcFileStatus.isWriteProtected() == 1):
                    os.chmod(self.mszFilename, stat.S_IWRITE)

                lcFileStatus2 = tcFileStatus(self.mszFilename)

                if (lcFileStatus2.isWriteProtected() == 0):
                    self.moLogFile = open(self.mszFilename, lszModeCreation)
                    if (lszModeCreation == "a"):
                        self.moLogFile.write('\n\n---------------------------------------\n')
            else:
                self.moLogFile = open(self.mszFilename, "w")
            if (self.isLogFileOpen()):
                self.timestamp("Log started")
                self.copyLogListToLogFile()

    def timestamp(self, lszString):
        self.moLogFile.write(lszString + ': %s\n' % (datetime.datetime.now()))

    def write(self, lszString, liColor = 15):
        if not self.mbSilent:
            if (self.mcCons == None):
                print(lszString, end='')
            else:
                self.mcCons.write(lszString, liColor)
        if (self.isLogFileOpen()):
            self.moLogFile.write(lszString)
        else:
            self.mlstLogList.append(lszString)

    def writeln(self, lszString, liColor = 15):
        self.write(lszString + "\n", liColor)

    def copyLogListToLogFile(self):
        if len(self.mlstLogList) > 0:
            for x in self.mlstLogList:
                self.moLogFile.write(x)
            self.mlstLogList.clear()

    # https://stackoverflow.com/questions/33533148/how-do-i-specify-that-the-return-type-of-a-method-is-the-same-as-the-class-itsel
    def clone(self) -> 'tcLog':
        lcLog = tcLog()

        lcLog.mszFilename   = self.mszFilename
        lcLog.moLogFile     = self.moLogFile

        lcLog.mlstLogList = self.mlstLogList.copy()
        lcLog.mbSilent    = self.mbSilent

        lcLog.mcCons  = self.mcCons
        lcLog.RED     = self.RED
        lcLog.GREY    = self.GREY
        lcLog.WHITE   = self.WHITE
        lcLog.DEFAULT = self.DEFAULT
        lcLog.GREEN   = self.GREEN
        lcLog.YELLOW  = self.YELLOW
        return lcLog

    def __copy__(self):
        pass

    def __deepcopy__(self, memo):
        return self.clone()

    def close(self):
        if (self.isLogFileOpen()):
            self.timestamp("Log stopped")
            self.moLogFile.close()

    def __del__(self):
        self.close()


# Test
if __name__ == "__main__":
    isRunningInPyCharm = "PYCHARM_HOSTED" in os.environ
    if isRunningInPyCharm:
        lcLog = tcLog(None, "w")

        if os.path.exists("test.txt"):
            os.remove("test.txt")
            print("test.txt deleted")
        else:
            print("The file does not exist")

        lcLog.writeln("This is the a log1", lcLog.RED)
        lcLog.mbSilent = True
        lcLog.writeln("This is the a log2")
        lcLog.mbSilent = False
        lcLog.writeln("Variant selection here")

        lcLog.open("test.txt", "w")
        if os.path.exists("test.txt"):
            print("test.txt created")

        lcLog.writeln("This is the a log4")
        lcLog.writeln("This is the a log5")
        lcLog.writeln("This is the a log6")

        import copy

        lcLog2 = copy.deepcopy(lcLog)

        lcLog2.close()

        lcLog.close()



