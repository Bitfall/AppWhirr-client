//
//  iconizerAppDelegate.m
//  iconizer
//
//  Created by Birmacher on 2/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "iconizerAppDelegate.h"

@implementation iconizerAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application 
	if ([[NSUserDefaults standardUserDefaults] objectForKey:@"pathToSave"]) {
		[pathTextField setStringValue:[[NSUserDefaults standardUserDefaults] objectForKey:@"pathToSave"]];
	}
}

#pragma mark -
#pragma mark button

- (IBAction)selectFile:(id)sender {
	int i; // Loop counter.
	
	// Create the File Open Dialog class.
	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
	
	// Enable the selection of files in the dialog.
	[openDlg setCanChooseFiles:YES];
	
	// Enable the selection of directories in the dialog.
	[openDlg setCanChooseDirectories:NO];
	
	// Display the dialog.  If the OK button was pressed,
	// process the files.
	if ( [openDlg runModalForDirectory:nil file:nil] == NSOKButton )
	{
		// Get an array containing the full filenames of all
		// files and directories selected.
		NSArray* files = [openDlg filenames];
		
		// Loop through all the files and process them.
		for( i = 0; i < [files count]; i++ )
		{
			NSString* fileName = [files objectAtIndex:i];
			[applicationTextField setStringValue:fileName];
		}
	}
}

- (IBAction)selectPathToSave:(id)sender {
	int i; // Loop counter.
	
	// Create the File Open Dialog class.
	NSOpenPanel* openDlg = [NSOpenPanel openPanel];
	
	// Enable the selection of files in the dialog.
	[openDlg setCanChooseFiles:NO];
	
	// Enable the selection of directories in the dialog.
	[openDlg setCanChooseDirectories:YES];
	
	// Display the dialog.  If the OK button was pressed,
	// process the files.
	if ( [openDlg runModalForDirectory:nil file:nil] == NSOKButton )
	{
		// Get an array containing the full filenames of all
		// files and directories selected.
		NSArray* files = [openDlg filenames];
		
		// Loop through all the files and process them.
		for( i = 0; i < [files count]; i++ )
		{
			NSString* dirName = [files objectAtIndex:i];
			[pathTextField setStringValue:dirName];
			
			[[NSUserDefaults standardUserDefaults] setObject:dirName forKey:@"pathToSave"];
		}
	}
}

- (IBAction)startButton:(id)sender {
	if ([NSThread mainThread] == [NSThread currentThread]) {
		// Perform it in background
		[self performSelectorInBackground:@selector(startButton:)
							   withObject:sender];
		return;
	}
	
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	[progressIndicator startAnimation:self];
	
	workspace = [[NSWorkspace alloc] init];
	
	// Get largest icon representation
	[self getLargeImage];
	[self getMediumImage];
	[self getSmallImage];
	
	[iconImage setImage:largeImage];
	
	// Get info plist
	info = [[NSBundle bundleWithPath:[applicationTextField stringValue]] infoDictionary];
	
	// Set App Display Name
	[applicationName setStringValue:[info valueForKey:@"CFBundleName"]];
	
	// Generate output
	[self generateOutput];
	
	[workspace release];
	
	[progressIndicator stopAnimation:self];
	
	[pool release];
}

#pragma mark -
#pragma mark image

- (void)getSmallImage {
	int size=64;
	for (; size>8; size/=2) {
		if ([self getImageForSize:size container:&smallImage]) break;
	}
}

- (void)getMediumImage {
	int size=128;
	for (; size>8; size/=2) {
		if ([self getImageForSize:size container:&mediumImage]) break;
	}
}

- (void)getLargeImage {
	int size=256;
	for (; size>8; size/=2) {
		if ([self getImageForSize:size container:&largeImage]) break;
	}
}

- (BOOL)getImageForSize:(float)size container:(NSImage**)container {
	NSImage* images = [workspace iconForFile:[applicationTextField stringValue]];
	for (NSImageRep* image in [images representations]) {
		if ([image size].height == size &&
			[image size].width == size) {
			*container = [[NSImage alloc] initWithSize:[image size]];
			[*container addRepresentation:image];
			
			return TRUE;
		}
	}
	return FALSE;
}

- (void)savePNG:(NSImage*)image toFile:(NSString*)file {
	NSBitmapImageRep* bits = [[image representations] objectAtIndex:0];
	
	NSData* data;
	data = [bits representationUsingType:NSPNGFileType properties:nil];
	[data writeToFile:file atomically:NO];
}

#pragma mark -
#pragma mark output

- (void)generateOutput {
	// Icons
	NSMutableDictionary* icons = [NSMutableDictionary dictionary];
	if (largeImage) {
		[icons setValue:@"./easyAppsSpecific/icons/large.png" forKey:@"large"];
	}
	if (mediumImage) {
		[icons setValue:@"./easyAppsSpecific/icons/medium.png" forKey:@"medium"];
	}
	if (smallImage) {
		[icons setValue:@"./easyAppsSpecific/icons/small.png" forKey:@"small"];
	}
	
	// Info
	NSDictionary* applicationInfo = [NSMutableDictionary dictionaryWithObjectsAndKeys:
									 [info valueForKey:@"CFBundleIdentifier"], @"app_id",
									 [info valueForKey:@"CFBundleName"], @"app_name_short",
									 [info valueForKey:@"CFBundleName"], @"app_name_long",
									 icons, @"icons",
									 @"", @"sync",
									 [NSString stringWithFormat:@"./%@/%@", [info valueForKey:@"CFBundleName"], [[applicationTextField stringValue] lastPathComponent]], @"executable",
									 @"", @"working_dir",
									 @"", @"docs",
									 @"", @"licences",
									 nil];
	

	// Create path
	[self createDirectories];

	// Generate output
	NSDictionary* output = [NSMutableDictionary dictionaryWithObjectsAndKeys:
							applicationInfo, @"application_info",
							nil];
	
	NSString* xml = @"<?xml version=\"1.0\" ?>\n";
	xml = [xml stringByAppendingString:[self createLevel:0 dictionary:output]];
	
	// Save
	[xml writeToFile:[NSString stringWithFormat:@"%@/%@/app.xml", [pathTextField stringValue], [info valueForKey:@"CFBundleName"]]
		  atomically:NO 
			encoding:NSUTF8StringEncoding
			   error:nil];
	
	if (largeImage) {
		[self savePNG:largeImage toFile:[NSString stringWithFormat:@"%@/%@/easyAppsSpecific/icons/large.png", [pathTextField stringValue], [info valueForKey:@"CFBundleName"]]];
	}
	if (mediumImage) {
		[self savePNG:mediumImage toFile:[NSString stringWithFormat:@"%@/%@/easyAppsSpecific/icons/medium.png", [pathTextField stringValue], [info valueForKey:@"CFBundleName"]]];
	}
	if (smallImage) {
		[self savePNG:smallImage toFile:[NSString stringWithFormat:@"%@/%@/easyAppsSpecific/icons/small.png", [pathTextField stringValue], [info valueForKey:@"CFBundleName"]]];
	}
}

- (void)createDirectories {
	NSFileManager* fileManager = [[NSFileManager alloc] init];
	
	// Create directories
	[fileManager createDirectoryAtPath:[NSString stringWithFormat:@"%@/%@/easyAppsSpecific/icons", [pathTextField stringValue], [info valueForKey:@"CFBundleName"]]
		   withIntermediateDirectories:YES
							attributes:nil
								 error:nil];
	[fileManager createDirectoryAtPath:[NSString stringWithFormat:@"%@/%@/%@", [pathTextField stringValue], [info valueForKey:@"CFBundleName"], [info valueForKey:@"CFBundleName"]]
		   withIntermediateDirectories:YES
							attributes:nil
								 error:nil];
	
	// Copy files
	NSError* error;
	[fileManager copyItemAtPath:[applicationTextField stringValue]
						 toPath:[NSString stringWithFormat:@"%@/%@/%@/%@", [pathTextField stringValue], [info valueForKey:@"CFBundleName"], [info valueForKey:@"CFBundleName"], [[applicationTextField stringValue] lastPathComponent]]
						  error:&error];
	
	[fileManager release];
}

- (NSString*)createLevel:(NSInteger)level dictionary:(NSDictionary*)dict {
	NSString* string = @"";
	NSString* tr = @"";
	for (int i=0; i<level; i++) tr = [tr stringByAppendingString:@"\t"];
	
	int count = [dict count];
	id values[count];
	id keys[count];
	
	[dict getObjects:values
			 andKeys:keys];
	
	for (int i=0; i<count; i++) {
		string = [string stringByAppendingFormat:@"%@<%@>", tr, keys[i]];
		if ([values[i] isKindOfClass:[NSDictionary class]]) {
			string = [string stringByAppendingString:@"\n"];
		}
		
		if ([values[i] isKindOfClass:[NSDictionary class]]) {
			string = [string stringByAppendingString:[self createLevel:(level+1) dictionary:(NSDictionary*)values[i]]];
		} else {
			string = [string stringByAppendingFormat:@"%@", values[i]];
		}
		
		if ([values[i] isKindOfClass:[NSDictionary class]]) {
			string = [string stringByAppendingString:tr];
		}
		string = [string stringByAppendingFormat:@"</%@>\n", keys[i]];
	}
	
	return string;
}

@end
