import os


def ListFindAndReplace(lLstList, lszSearchString, lszNewString):
    for x in range(0, len(lLstList)-1):
        lLstList[x] = lLstList[x].replace(lszSearchString, lszNewString)

def StringFindAndReplace(lszString, lszSearchString, lszNewString):
    return lszString.replace(lszSearchString, lszNewString)


lszCurrentPath = os.path.dirname(os.path.abspath(__file__))


for root, dirs, files in os.walk(lszCurrentPath):
    for file in files:
        if file.lower().endswith("cmakelists.txt"):

            with open(os.path.join(root, file), 'r') as infile:
                data = infile.read()

            lbFound = False

            if len(data) > 50:
                if data.find("# --------------------- Set package name") >= 0:
                    lbFound = True
                    print(os.path.join(root, file))

            if lbFound:
                with open(os.path.join(root, "PkgCfg.cmake"), 'r') as old:
                    t = 0
                    lszPkgCfgHead = ""

                    for line in old:
                        t+=1
                        lszPkgCfgHead+=line
                        if t==7: break
                lszPkgCfgHead+="\n"


                with open(os.path.join(root, file), 'r') as original, open(os.path.join(root, "PkgCfg.cmake"), 'w') as new:
                    for line in original:
                        line = line.strip()
                        writing = False

                        #if line.find("set(this_module_3rd_party") >= 0:
                            #line = line + "\n"
                            #line = line.replace("this_module_3rd_party", "PkgCfg_3rd_party")
                            #writing = True


                        if writing:
                            new.write(line+ "\n")

                    new.write(lszPkgCfgHead +
                              "# Optional set package specific compile options\n" +
                              "# e.g. Enable the following line in case you need NO-Optimizations for debugging\n" +
                              "# set(PkgCfg_TargetCompileOption -Og)\n" +
                              "\n" +
                              "# 3rd party. Turn off all warnings\n" +
                              "if (PkgCfg_3rd_party AND (NOT PCB_ShowWarnings))\n" +
                              "  message(STATUS \"  3rd party => warnings off\")\n" +
                              "  if (PkgCfg_TargetCompileOption)\n" +
                              "    set(PkgCfg_TargetCompileOption -w)\n" +
                              "  else()\n" +
                              "    list(APPEND PkgCfg_TargetCompileOption -w)\n" +
                              "  endif()\n" +
                              "endif()\n")

