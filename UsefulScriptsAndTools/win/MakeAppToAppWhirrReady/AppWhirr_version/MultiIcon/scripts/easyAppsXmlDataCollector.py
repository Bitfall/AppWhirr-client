# coding: utf-8

#
# version: 0.8
# created by: Viktor Benei
# date: 2011.05
#

#
# collects the data needed for the easy apps app.xml
# then writes it to the specified path/file
#
#	usage:
#		python easyAppsXmlDataCollector.py [app_name] [svg_image_url] [large_image_url] [medium_image_url] [small_image_url] [app_exe_path] [output_info_xml_path]
#		
#		! the order cannot be changed, but you can leave from right to left as many as you want
#		! if you want to skip a parameter then pass a __default param instead of anything useful
#

import os
import sys
import xml.dom.minidom


# -- customize these - User Variables

# 
uvRootDirectory = "."
#
uvDefaultOutputFilePath = "app.xml"
#
uvIsOutputPretty = True
#
uvAllowSearchHtmlDocsAndLicences = True


# --


# global vars
gOutputFile = None
gOutputXml = None

gAppName = "Test name"
gIconUrls = { 'svg': '', 'large': '', 'medium': '', 'small': '' }


# functions

# -----
def write_to_output_file (writeThis, printAsWell = False) :
	global gOutputFile

	gOutputFile.write("{0}\n".format(writeThis))
	if printAsWell:
		print(writeThis)
		

#
# XML utility functions
		
# -----
def initialize_xml () :
	global gOutputXml
	
	xml_string = '<?xml version=\"1.0\" encoding=\"utf-8\"?><application_info></application_info>'
	gOutputXml = xml.dom.minidom.parseString(xml_string)
	
# -----
def write_xml_to_file () :
	global gOutputXml
	global uvIsOutputPretty
	
	xml_string = ""
	if (uvIsOutputPretty) :
		xml_string = gOutputXml.toprettyxml()
	else :
		xml_string = gOutputXml.toxml()
	print(xml_string)
	write_to_output_file(xml_string)

# -----
def create_and_add_this_element_node_to_node (element_node_name, parent_node) :
	global gOutputXml
	new_executable_child_node = gOutputXml.createElement( element_node_name )
	parent_node.appendChild( new_executable_child_node )
	return new_executable_child_node
	
# -----
def create_and_add_this_element_node_to_root (element_node_name) :
	global gOutputXml
	root_node = gOutputXml.documentElement
	new_executable_child_node = create_and_add_this_element_node_to_node( element_node_name, root_node )
	return new_executable_child_node

# -----
def create_and_add_text_node_with_this_data_to_node (data, parent_node) :
	global gOutputXml
	new_text_node = gOutputXml.createTextNode( data )
	parent_node.appendChild( new_text_node )
	return new_text_node
	
# -----	
def add_executable_to_xml (executable_path) :	
	new_node = create_and_add_this_element_node_to_root( "executable" )
	create_and_add_text_node_with_this_data_to_node( executable_path, new_node )
	
# -----		
def add_working_dir_by_file_to_xml (f) :
	new_node = create_and_add_this_element_node_to_root( "working_dir" )
	create_and_add_text_node_with_this_data_to_node( os.path.dirname(f), new_node )
	
# -----	
def add_app_id_to_xml () :
	new_node = create_and_add_this_element_node_to_root( "app_id" )
	create_and_add_text_node_with_this_data_to_node( "-1", new_node )

# -----	
def add_app_name_short_to_xml () :
	new_node = create_and_add_this_element_node_to_root( "app_name_short" )
	create_and_add_text_node_with_this_data_to_node( gAppName, new_node )

# -----	
def add_app_name_long_to_xml () :
	new_node = create_and_add_this_element_node_to_root( "app_name_long" )
	create_and_add_text_node_with_this_data_to_node( gAppName, new_node )

# -----	
def add_default_icon_node_to_xml () :
	new_node = create_and_add_this_element_node_to_root( "icons" )
	
	svg_node = create_and_add_this_element_node_to_node( "svg", new_node )
	create_and_add_text_node_with_this_data_to_node( gIconUrls['svg'], svg_node )
	
	large_node = create_and_add_this_element_node_to_node( "large", new_node )
	create_and_add_text_node_with_this_data_to_node( gIconUrls['large'], large_node )
	
	medium_node = create_and_add_this_element_node_to_node( "medium", new_node )
	create_and_add_text_node_with_this_data_to_node( gIconUrls['medium'], medium_node )
	
	small_node = create_and_add_this_element_node_to_node( "small", new_node )
	create_and_add_text_node_with_this_data_to_node( gIconUrls['small'], small_node )

# -----	
def create_and_add_sync_node_to_xml () :
	return create_and_add_this_element_node_to_root( "sync" )

# -----	
def create_and_add_docs_node_to_xml () :
	return create_and_add_this_element_node_to_root( "docs" )

# -----	
def create_and_add_licences_node_to_xml () :
	return create_and_add_this_element_node_to_root( "licences" )


#
# Other functions

# -----		
def getFileLists (directory, fileExtList, isIncludeSubdirs) :                                        
	"get list of file info objects for files of particular extensions" 
	
	
	pathes = [os.path.normpath(f)
		for f in os.listdir(directory)] 
		
	fileList = []
	for path in pathes:
		fullPath = os.path.join(directory, path)
		if os.path.isfile(fullPath):
			if os.path.splitext(fullPath)[1] in fileExtList:
				fileList.append(fullPath)
		elif isIncludeSubdirs:
			fileList.extend(getFileLists(fullPath, fileExtList, isIncludeSubdirs))
	
	return fileList
	
def isProbablyLicenceFile (f) :
	file_name = os.path.basename(os.path.splitext(f)[0]).lower()
	print(file_name)
	if ( file_name == "licence" or file_name == "licences" or file_name == "license" or file_name == "licenses" or file_name == "eula" ) :
		return True;
		
	return False;

		
# -- MAIN
print("-- start --")

gExePath = None
gOutputFilePath = None

# --
# scan the input arguments
arg_num = len(sys.argv);
if (arg_num >= 2) :
	if (sys.argv[1] != "__default") :
		#gAppName = unicode(sys.argv[1])
		gAppName = sys.argv[1]
		print("Appname found: ", gAppName)
if (arg_num >= 3) : 
	if (sys.argv[2] != "__default") :
		gIconUrls['svg'] = sys.argv[2]
		print("svg icon found: ", gIconUrls['svg'])
if (arg_num >= 4) : 
	if (sys.argv[3] != "__default") :
		gIconUrls['large'] = sys.argv[3]
		print("large icon found: ", gIconUrls['large'])
if (arg_num >= 5) : 
	if (sys.argv[4] != "__default") :
		gIconUrls['medium'] = sys.argv[4]
		print("medium icon found: ", gIconUrls['medium'])
if (arg_num >= 6) : 
	if (sys.argv[5] != "__default") :
		gIconUrls['small'] = sys.argv[5]
		print("small icon found: ", gIconUrls['small'])
if (arg_num >= 7) : 
	if (sys.argv[6] != "__default") :
		gExePath = sys.argv[6]
		print("exe path found: ", gExePath)
if (arg_num >= 8) : 
	if (sys.argv[7] != "__default") :
		gOutputFilePath = sys.argv[7]
		print("output file path set: ", gOutputFilePath)

		
# --
# let's do the magic

if gOutputFilePath is not None:
	gOutputFile = open(gOutputFilePath, "w", encoding='utf-8')
else:
	gOutputFile = open(uvDefaultOutputFilePath, "w", encoding='utf-8')

initialize_xml()
#write_to_output_file("hello world 23")

# default appId
add_app_id_to_xml()
# default app name short
add_app_name_short_to_xml()
# default app name long
add_app_name_long_to_xml()
# default icons
add_default_icon_node_to_xml()
# default sync
create_and_add_sync_node_to_xml()

# executable and working directory
exe_files = None
if gExePath is not None:
	exe_files = [gExePath]
else:
	exe_files = getFileLists(uvRootDirectory, [".exe"], True);

for f in exe_files:
		#print ("@{0}".format(f))
		add_executable_to_xml(f)
		add_working_dir_by_file_to_xml(f)

# add docs
docs_node = create_and_add_docs_node_to_xml()
licences_node = create_and_add_licences_node_to_xml()
doc_extensions = [".txt", ".pdf"]
if ( uvAllowSearchHtmlDocsAndLicences ) :
	doc_extensions.append(".html")
doc_files = getFileLists(uvRootDirectory, doc_extensions, True)
for f in doc_files:
		new_doc_node = None
		
		if ( os.path.splitext(f)[1] == ".pdf" ) :
			# check some filters determins wheter it's a licence file, or if it's not then it's a doc file (probably)
			if ( isProbablyLicenceFile(f) ) :
				new_doc_node = create_and_add_this_element_node_to_node( "pdf", licences_node )
			else :
				new_doc_node = create_and_add_this_element_node_to_node( "pdf", docs_node )
		elif ( os.path.splitext(f)[1] == ".txt" ) :
			if ( isProbablyLicenceFile(f) ) :
				new_doc_node = create_and_add_this_element_node_to_node( "txt", licences_node )
			else :
				new_doc_node = create_and_add_this_element_node_to_node( "txt", docs_node )				
		elif ( uvAllowSearchHtmlDocsAndLicences and os.path.splitext(f)[1] == ".html" ) :
			if ( isProbablyLicenceFile(f) ) :
				new_doc_node = create_and_add_this_element_node_to_node( "html", licences_node )
			else :
				new_doc_node = create_and_add_this_element_node_to_node( "html", docs_node )	
			
		if (new_doc_node is not None) :
			create_and_add_text_node_with_this_data_to_node( f, new_doc_node )
		
		
write_xml_to_file()
	
print("-- end --")

gOutputFile.close()