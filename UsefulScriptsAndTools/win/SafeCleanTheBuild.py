#
# this will clean the current (!) directory
#	- you can define exceptions, which files / directories will kept untouched
#	- and also directories which will be kept and only their content will be deleted
#
# ! calibrated to Windows build
#
# usage:
#	- simply execute it in the desired directory, or specify the target (root) directory as a parameter: python SafeCleanBuild.py "the target directory"
#

import os
import shutil
import sys

uvDoNotDeleteTheseDirectoriesAndFiles = [ 'SafeCleanTheBuild.py', 'apps', 'Settings', 'debug\\plugins', 'release\\plugins' ]
uvDeleteOnlyItsContent = [ 'release', 'debug' ]

def deleteThisPath (thePath) :
	if os.path.isdir(thePath):
		shutil.rmtree(thePath)
	elif os.path.isfile(thePath):
		os.remove(thePath)
	else:
		print("The given path is not a directory and it's not a file as well!")

		
		
rootTargetDirPath = "./"
if len(sys.argv) > 1:
	rootTargetDirPath = sys.argv[1]
	
print("Root target dir: {0}".format(rootTargetDirPath))
os.chdir(rootTargetDirPath)
	
for filename in os.listdir("./") :
	#currJoinedPath = os.path.join(rootTargetDirPath, filename)
	currJoinedPath = filename
	print(currJoinedPath)
	if filename in uvDoNotDeleteTheseDirectoriesAndFiles:
		print(" defined exception, won't be deleted")
	else:
		if filename in uvDeleteOnlyItsContent:
			print(" ! delete only it's content")
			for subname in os.listdir(currJoinedPath):
				subPath = os.path.join(currJoinedPath, subname)
				print("SubPath: {0}".format(subPath))
				if subPath in uvDoNotDeleteTheseDirectoriesAndFiles:
					print("{0} defined exception, won't be deleted".format(subPath))
				else:
					deleteThisPath(subPath)
		else:
			print(" ! delete")
			deleteThisPath(currJoinedPath)