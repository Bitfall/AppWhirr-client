//
//  iconizerAppDelegate.h
//  iconizer
//
//  Created by Birmacher on 2/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface iconizerAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
	
	IBOutlet NSTextField*			applicationTextField;
	IBOutlet NSTextField*			pathTextField;
	IBOutlet NSTextField*			errorText;
	
	IBOutlet NSProgressIndicator*	progressIndicator;

	IBOutlet NSImageView*			iconImage;
	IBOutlet NSTextField*			applicationName;
	
	NSImage*						smallImage;
	NSImage*						mediumImage;
	NSImage*						largeImage;
	
	NSWorkspace*					workspace;
	NSDictionary*					info;
}

@property (assign) IBOutlet NSWindow *window;

- (void)getSmallImage;
- (void)getMediumImage;
- (void)getLargeImage;
- (BOOL)getImageForSize:(float)size container:(NSImage**)container;
- (void)savePNG:(NSImage*)image toFile:(NSString*)file;

- (void)generateOutput;
- (void)createDirectories;
- (NSString*)createLevel:(NSInteger)level dictionary:(NSDictionary*)dict;

- (IBAction)selectFile:(id)sender;
- (IBAction)selectPathToSave:(id)sender;
- (IBAction)startButton:(id)sender;

@end
