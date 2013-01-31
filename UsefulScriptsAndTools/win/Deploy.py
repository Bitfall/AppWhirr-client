# coding: utf-8

#
# deploys everything needed to run the AppWhirr client
#	- Qt DLLs
#	- Qt Plugin DLLs
#	- AppWhirr specific files
#
#	usage:
#		customize the settings variables (beginning with uv) here, in the beginning of the code
#		then simply run: python Deploy.py
#
#	notes:
#		- cannot handle sub-directories
#

import os
import shutil

# -- customize these - User Variables

# [!] config mode
conf_builtWithMingw = False
conf_builtWithMsvc2008 = True

#
uvIsLoggingEnabled = True

# 
uvAppWhirrReleaseDirectory = "E:/Programming/EasyPortableApps/bitbucket/Source/EasyAppsClientProject/EasyAppsClientProject-build-desktop/BIN/release/"
uvExcludeTheseFilesFromCopyingFromTheReleaseDirectory = ["libquazip.a", "plugins", "apps", "Settings"]


if(conf_builtWithMingw):
	# MinGW config
	uvRequiredQtFilesRootDir = "E:/QT_SDKS/QtSDK/Desktop/Qt/4.7.3/mingw"
	uvRequiredQtFilesRelativePath = ["bin/QtCore4.dll", "bin/libgcc_s_dw2-1.dll", "bin/mingwm10.dll", "bin/QtGui4.dll", "bin/QtNetwork4.dll", "bin/QtScript4.dll", "bin/QtSql4.dll", "bin/QtSvg4.dll", "bin/QtXml4.dll", "bin/QtWebKit4.dll", "bin/phonon4.dll"]
if(conf_builtWithMsvc2008):
	# MS Visual Studio 2008 config
	uvRequiredQtFilesRootDir = "E:/QT_SDKS/QtSDK/Desktop/Qt/4.7.3/msvc2008/"
	uvRequiredQtFilesRelativePath = ["bin/QtCore4.dll", "bin/QtGui4.dll", "bin/QtNetwork4.dll", "bin/QtScript4.dll", "bin/QtSql4.dll", "bin/QtSvg4.dll", "bin/QtXml4.dll", "bin/QtWebKit4.dll", "bin/phonon4.dll"]
	uvExcludeTheseFilesFromCopyingFromTheReleaseDirectory += ['quazip.exp', 'quazip.lib']
	
uvRequiredQtPluginFileBaseDirectory = "plugins/"
uvRequiredQtPluginFiles = ["sqldrivers/qsqlite4.dll", "imageformats/qjpeg4.dll", "imageformats/qico4.dll", "imageformats/qgif4.dll", "imageformats/qsvg4.dll"]

#
uvDeploymentTargetDirectory = "E:/AppWhirr/"
uvQtPluginsDeploymentTargetDirectory = "E:/AppWhirr/plugins/"



#
# this accepts files with extension as well directories as destinationPath
def copyFileToDirectory (filePathToCopy, destinationPath) :
	if uvIsLoggingEnabled :
		print("Copy file: [ {0} ] to directory: [ {1} ]".format(filePathToCopy, destinationPath))
		
	destDirName = destinationPath
	if len(os.path.splitext(destDirName)[1]) > 0 :
		destDirName = os.path.dirname(destinationPath)
		
	if not os.path.exists(destDirName) :
		# create the destination directory if it does not exists
		os.makedirs(destDirName, exist_ok=True)
			
	shutil.copy(filePathToCopy, destinationPath)

def copyEveryFileFromDirectoryToTargetDirectoryExceptSpecifiedExcludedFiles(sourceDirectory, destinationDirectory, excludedFileList) :
	for filename in os.listdir(sourceDirectory):
		if filename in excludedFileList:
			if uvIsLoggingEnabled :
				print("File: [ {0} ] is defined as exception, won't be copied".format(os.path.join(sourceDirectory, filename)))
		else:
			fullSourceFilePath = os.path.join(sourceDirectory, filename)
			copyFileToDirectory(fullSourceFilePath, destinationDirectory)
	
# --
# main

if __name__ == "__main__":
	print(" -- starting main")
	
	# copy Qt files
	for filePath in uvRequiredQtFilesRelativePath :
		copyFileToDirectory(os.path.join(uvRequiredQtFilesRootDir, filePath), uvDeploymentTargetDirectory)
	
	# copy Qt plugins
	qtPluginsRootDir = os.path.join(uvRequiredQtFilesRootDir, uvRequiredQtPluginFileBaseDirectory)
	for relativePluginFilePath in uvRequiredQtPluginFiles :
		copyFileToDirectory(os.path.join(qtPluginsRootDir, relativePluginFilePath), os.path.join(uvQtPluginsDeploymentTargetDirectory, relativePluginFilePath))
		
	# copy AppWhirr files
	copyEveryFileFromDirectoryToTargetDirectoryExceptSpecifiedExcludedFiles(uvAppWhirrReleaseDirectory, uvDeploymentTargetDirectory, uvExcludeTheseFilesFromCopyingFromTheReleaseDirectory)
		
	print(" -- finished main")