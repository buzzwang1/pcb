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
                with open(os.path.join(root, file), 'r') as original, open(os.path.join(root, "PkgCfg.cmake"), 'w') as new:
                    for line in original:
                        line = line.strip()
                        writing = False

                        if line.find("set(this_module_3rd_party") >= 0:
                            line = line + "\n"
                            line = line.replace("this_module_3rd_party", "PkgCfg_3rd_party")
                            writing = True

                        if line.find("set(lListLinkLibraries") >= 0:
                            line = line.replace("#s", "# s")
                            line = line + "\n"
                            line = line.replace("lListLinkLibraries", "PkgCfg_ListLinkLibraries")
                            writing = True

                        if line.find("# Uncomment that line, in case of 3rd party package") >= 0:
                            writing = True

                        if line.find("# Uncomment for link libraries") >= 0:
                            writing = True


                        if line.find("set(PCB_ListTargetExeFiles") >= 0:
                            line = line.replace("PCB_ListTargetExeFiles \"${PCB_ListTargetExeFiles};", "PkgCfg_ListTargetExeFiles \"")
                            line = line.replace(" PARENT_SCOPE", "")
                            writing = True


                        if writing:
                            new.write(line+ "\n")




