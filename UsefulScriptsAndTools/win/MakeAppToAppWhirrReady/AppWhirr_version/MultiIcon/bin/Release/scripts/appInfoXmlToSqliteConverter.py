# coding: utf-8

#
# version: 0.9
# created by: Viktor Benei
# date: 2011.07
#

#
#	usage:
#		python appInfoXmlToSqliteConverter.py --sourceXmlFile="..." [--targetSQLiteFile="..."]
#

import sqlite3, os, sys, getopt, xml.parsers.expat
from xml.parsers.expat import ParserCreate, ExpatError, errors

#
# user configurable variables
#uv_sourceAppInfoXmlFileName = "app.xml"
uv_defaultTargetAppInfoDatabase = 'appInfo.sqlite'

#
# command line
requiredShortParams = ""
requiredLongParamList = ["sourceXmlFile="]
optionalShortParams = "h"
optionalLongParamList = ["help", "targetSQLiteFile="]

	
#
# AppInfo class
class AppInfo :
	def __init__(self):
		self.appId = -1
		self.shortName = ""
		self.svgIconPath = ""
		self.largeIconPath = ""
		self.mediumIconPath = ""
		self.smallIconPath = ""
		self.executablePath = ""
		self.executableWorkingDirectory = ""
		self.licencePath = ""
		
	def setApplicationId(self, appId):
		self.appId = appId
	def getApplicationId(self):
		return self.appId
	
	def setApplicationName(self, shortName):
		self.shortName = shortName
	def getApplicationName(self):
		return self.shortName
		
	def setSvgIconPath(self, svgIconPath):
		self.svgIconPath = svgIconPath
	def getSvgIconPath(self):
		return self.svgIconPath
		
	def setLargeIconPath(self, largeIconPath):
		self.largeIconPath = largeIconPath
	def getLargeIconPath(self):
		return self.largeIconPath
		
	def setMediumIconPath(self, mediumIconPath):
		self.mediumIconPath = mediumIconPath
	def getMediumIconPath(self):
		return self.mediumIconPath
		
	def setSmallIconPath(self, smallIconPath):
		self.smallIconPath = smallIconPath
	def getSmallIconPath(self):
		return self.smallIconPath
		
	def setExecutablePath(self, executablePath):
		self.executablePath = executablePath
	def getExecutablePath(self):
		return self.executablePath
		
	def setExecutableWorkingDirectory(self, executableWorkingDirectory):
		self.executableWorkingDirectory = executableWorkingDirectory
	def getExecutableWorkingDirectory(self):
		return self.executableWorkingDirectory
		
	def setLicencePath(self, licencePath):
		self.licencePath = licencePath
	def getLicencePath(self):
		return self.licencePath
		
#
# xml parsing

#
# Info xml parser class
#
#	delegate: InfoXmlParserDelegate object, which have to have a method: 
#		foundValueForElementsPath(valueText, elementNamesPath)
#
# 		elementNamesPath / currParsingElementsPath is the element names concatenated with ".", begins with a single "." . For example if there's an element <app> and an element in this element <ico> -> <app><ico>...</ico></app> -> then when parsing <ico> the elementNamesPath will be .app.ico
#
class InfoXmlParser:

	def __init__(self):
		self.delegate = None
		self.currParsingElementsPath = ""
		self.currParsingValue = ""
		
	# 3 handler functions
	def start_element(self, name, attrs):
		self.currParsingElementsPath += "."+name
		print("Elements path: ", self.currParsingElementsPath)
		self.currParsingValue = ""
	def end_element(self, name):
		self.delegate.foundValueForElementsPath(self.currParsingValue, self.currParsingElementsPath)
		self.currParsingElementsPath = self.currParsingElementsPath[0:self.currParsingElementsPath.rfind("."+name)]
		
	def char_data(self, data):
		self.currParsingValue += data

	def parseThisFile(self, xmlFilePath, delegate):
		p = xml.parsers.expat.ParserCreate()
		p.StartElementHandler = self.start_element
		p.EndElementHandler = self.end_element
		p.CharacterDataHandler = self.char_data
		self.delegate = delegate

		try:
			p.ParseFile(open(xmlFilePath, "rb"))
		except ExpatError as err:
			print("Error:", errors.messages[err.code])

class InfoXmlParserDelegate:
	def __init__(self):
		self.appInfo = AppInfo()
		
	def foundValueForElementsPath(self, valueText, elementNamesPath):	
		strippedValue = valueText.strip()
		if elementNamesPath == ".application_info.app_name_long":
			print("App name found:", strippedValue)
			self.appInfo.setApplicationName(strippedValue)
		elif elementNamesPath == ".application_info.icons.svg":
			print("SVG icon path found:", strippedValue)
			self.appInfo.setSvgIconPath(strippedValue)
		elif elementNamesPath == ".application_info.icons.large":
			print("Large icon path found:", strippedValue)
			self.appInfo.setLargeIconPath(strippedValue)
		elif elementNamesPath == ".application_info.icons.medium":
			print("Medium icon path found:", strippedValue)
			self.appInfo.setMediumIconPath(strippedValue)
		elif elementNamesPath == ".application_info.icons.small":
			print("Small icon path found:", strippedValue)
			self.appInfo.setSmallIconPath(strippedValue)
		elif elementNamesPath == ".application_info.executable":
			print("Executable path found:", strippedValue)
			self.appInfo.setExecutablePath(strippedValue)
		elif elementNamesPath == ".application_info.working_dir":
			print("Executable working directory found:", strippedValue)
			self.appInfo.setExecutableWorkingDirectory(strippedValue)
		elif elementNamesPath == ".application_info.licences.txt" or elementNamesPath == ".application_info.licences.html":
			print("Licence found:", strippedValue)
			self.appInfo.setLicencePath(strippedValue)
		elif elementNamesPath == ".application_info.app_id":
			if strippedValue is not None and strippedValue != "":
				print("AppId found:", strippedValue)
				self.appInfo.setApplicationId(strippedValue)
			
	def getAppInfo(self):
		return self.appInfo
	
class AppInfoToSQLiteWriter:
	def writeToSQLite(self, database, appInfo):
		#
		# create and store the parsed info to info-database
		conn = sqlite3.connect(database)
		c = conn.cursor()

		try:
			# create appInfo database
			c.execute('''CREATE TABLE staticAppInfo (appId TEXT UNIQUE, appName TEXT, executablePath TEXT, executableWorkingDirectory TEXT, svgIconPath TEXT, largeIconPath TEXT, mediumIconPath TEXT, smallIconPath TEXT, licenceFilePath TEXT)''')
			
			# add values
			c.execute("""INSERT INTO staticAppInfo values (?,?,?,?,?,?,?,?,?)""", (appInfo.getApplicationId(), appInfo.getApplicationName(), appInfo.getExecutablePath(), appInfo.getExecutableWorkingDirectory(), appInfo.getSvgIconPath(), appInfo.getLargeIconPath(), appInfo.getMediumIconPath(), appInfo.getSmallIconPath(), appInfo.getLicencePath()))
			
			# test print of values
			c.execute('''SELECT * FROM staticAppInfo''')
			print("Test staticAppInfos: ", c.fetchall())
			
		except sqlite3.Error as e:
			print(" [!!!] An error occurred:", e.args[0])
			
		# save
		conn.commit()

		# close
		c.close()

		
def printUsage():
	print('Required params: shorts: "{0}" AND/OR long: {1}'.format(requiredShortParams, requiredLongParamList))
	print('Optional params: shorts: "{0}" AND/OR long: {1}'.format(optionalShortParams, optionalLongParamList))
	
def main(argv):
	sourceAppInfoXmlFile = None
	targetAppInfoSQLiteFile = uv_defaultTargetAppInfoDatabase
	
	try:
		shortParams = requiredShortParams + optionalShortParams
		longParamList = requiredLongParamList + optionalLongParamList
		opts, args = getopt.getopt(argv, shortParams, longParamList) 
	except getopt.GetoptError:           
		printUsage()                          
		return 1
	for opt, arg in opts:
		if opt in ("-h", "--help"):
			printUsage()                     
			return 0                                   
		else:
			if opt == "--sourceXmlFile":
				sourceAppInfoXmlFile = arg
			if opt == "--targetSQLiteFile":
				targetAppInfoSQLiteFile = arg
				
	if sourceAppInfoXmlFile is not None:
		# test the target info-database file. If exists, don't do anything, only warn about.
		if os.path.isfile(targetAppInfoSQLiteFile):
			print("The target database file is already exists. Won't modify it.")
			return 2
		# parse
		infoXmlParserDelegate = InfoXmlParserDelegate()
		parser = InfoXmlParser()
		parser.parseThisFile(sourceAppInfoXmlFile, infoXmlParserDelegate)
		appInfo = infoXmlParserDelegate.getAppInfo()
		# save to database
		writer = AppInfoToSQLiteWriter()
		writer.writeToSQLite(targetAppInfoSQLiteFile, appInfo)
		# finished
		print("--finished--")
	else:
		printUsage()

if __name__ == '__main__':
	sys.exit(main(sys.argv[1:]))