import os
import fnmatch
import json
import glob
import tempfile
import shutil

def findProjectFiles():

    projectFolderList = []

    matches = []
    for root, dirnames, filenames in os.walk("."):
        for filename in fnmatch.filter(filenames, ".project"):
            matches.append(os.path.dirname(os.path.join(root, filename))[2:])

    return matches


if __name__ == "__main__":

    tmpDir = 'artifacts'

    if os.path.exists(tmpDir):
        shutil.rmtree(tmpDir) 

    os.makedirs(tmpDir)

    projectFolderList = findProjectFiles()

    outputDict = {"examples": [], "groups": []}
    for proj in projectFolderList:

        binPath =  glob.glob( os.path.join(proj,'*-Debug_QSPI','*.bin'))

        if len(binPath) != 1:
            raise FileExistsError(binPath)

        if not os.path.dirname(proj) in outputDict["groups"]:
            outputDict["groups"].append(os.path.dirname(proj))

        outputDict["examples"].append(
            {
                "path": proj,
                "group": os.path.dirname(proj),
                "title": os.path.basename(proj),
                "readmePath": os.path.join(proj, "Readme.md"),
                "binPath": binPath[0]
            }
        )

    json_object = json.dumps(outputDict, indent=4)
    with open(os.path.join(tmpDir,"projectData.json"), "w") as fp:
        fp.write(json_object)

    for f in outputDict['examples']:
        os.makedirs(os.path.join(tmpDir,os.path.dirname(f['binPath'])),  exist_ok=True)
        shutil.copyfile(f['binPath'],os.path.join(tmpDir,f['binPath']))

    #upload to AWS