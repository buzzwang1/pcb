import os
import shutil


lszCurrentPath = os.path.dirname(os.path.abspath(__file__))


for root, dirs, files in os.walk(lszCurrentPath):
    for file in files:
        if file.lower().endswith("cmakelists.txt"):

            #Check, if it starts with a ID line
            with open(os.path.join(root, file), 'r') as infile:
                first_line = infile.readline()

            lbFound = False
            if len(first_line) > 10:
                if first_line.find("# PCB: 20210330") >= 0:
                    lbFound = True

            print(str(lbFound) + ": " + os.path.join(root, file))
            if (lbFound):
                # --------------------- Set package name -------------------------------------
                shutil.copy2(os.path.join(lszCurrentPath, "_Template/CmakeLists.20210330_txt"), os.path.join(root, file))


