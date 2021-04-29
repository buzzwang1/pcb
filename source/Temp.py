import os
import shutil

lszCurrentPath = os.path.dirname(os.path.abspath(__file__))


for root, dirs, files in os.walk(lszCurrentPath):
    # for file in files:
    #     if file.lower().endswith("package.properties"):
    #
    #         print(root + "***" + file)
    #
    #         lszDest = os.path.abspath(os.path.join(root, "..", "_cfg"))
    #         os.remove(lszDest)
    #         shutil.copyfile(os.path.join(root, file), lszDest+".txt")

    for file in files:
        if file.lower().endswith("genlists.cmake"):

            print(root + "***" + file)

            lszDest = os.path.abspath(os.path.join(root, "_PkgGenLists.cmake"))
            shutil.move(os.path.join(root, file), lszDest)


    # for dir in dirs:
    #     if dir.lower().endswith("_var"):
    #         if root.lower().endswith("adapt"):
    #
    #             print(root + "***" + dir)
    #
    #             lszDest = os.path.abspath(os.path.join(root, ".."))
    #
    #             shutil.move(os.path.join(root, dir), lszDest)

    # for dir in dirs:
    #     if dir.lower().endswith("adapt"):
    #         print(root + "***" + dir)
    #
    #         for filename in os.listdir(os.path.join(root, dir)):
    #             file_path = os.path.join(os.path.join(root, dir), filename)
    #             try:
    #                 if os.path.isfile(file_path) or os.path.islink(file_path):
    #                     os.unlink(file_path)
    #                 elif os.path.isdir(file_path):
    #                     shutil.rmtree(file_path)
    #             except Exception as e:
    #                 print('Failed to delete %s. Reason: %s' % (file_path, e))
    #
    #         shutil.rmtree(os.path.join(root, dir))
