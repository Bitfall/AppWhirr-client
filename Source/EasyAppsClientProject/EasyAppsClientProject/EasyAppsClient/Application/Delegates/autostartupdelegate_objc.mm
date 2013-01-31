#include "autostartupdelegate.h"

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

// Todo move to somewhere else;)
// ---
QString NSStringToQString(const NSString* string)
{
    return QString([string UTF8String]);
}

CFURLRef bundlePath() {
    return (CFURLRef)([NSURL fileURLWithPath:[[NSBundle mainBundle] bundlePath]]);
}
// ---

bool AutoStartupDelegate::willStartAtSystemStartup(QString& applicationPath) {
    bool willStartup = false;

    // Get mac specific path
    CFURLRef appURL = bundlePath();

    LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL,
                                                            kLSSharedFileListSessionLoginItems,
                                                            NULL);

    if (loginItems) {
        UInt32 seed = 0U;

        NSArray* loginItemsArray = (NSArray*)LSSharedFileListCopySnapshot(loginItems, &seed);
        for (id item in loginItemsArray) {
            LSSharedFileListItemRef itemRef = (LSSharedFileListItemRef)item;

            UInt32 resolutionFlags = kLSSharedFileListNoUserInteraction | kLSSharedFileListDoNotMountVolumes;
            CFURLRef URL = NULL;
            OSStatus err = LSSharedFileListItemResolve(itemRef, resolutionFlags, &URL, NULL);

            if (err == noErr) {
                if (CFEqual(URL, appURL)) {
                    CFRelease(URL);

                    willStartup = true;

                    break;
                }
            }
        }
        CFRelease(loginItems);
    }

    CFRelease(appURL);

    return willStartup;
}

bool AutoStartupDelegate::makeItAutoStartAtSystemStartup() {
    bool isSucceeded = true;

    // Shared Files List is recommended after 10.5
    // All other methods are deprecated
    LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL,
                                                            kLSSharedFileListSessionLoginItems,
                                                            NULL);
    if (loginItems) {
        //Insert an item to the list.
        LSSharedFileListItemRef item = LSSharedFileListInsertItemURL(loginItems,
                                                                     kLSSharedFileListItemBeforeFirst,
                                                                     NULL,
                                                                     NULL,
                                                                     bundlePath(),
                                                                     NULL,
                                                                     NULL);
        if (item){
            CFRelease(item);
        } else {
            isSucceeded = false;
        }
    }

    CFRelease(loginItems);

    return isSucceeded;
}

bool AutoStartupDelegate::removeItFromAutoStartList() {
    // Get shared file list
    LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL,
                                                            kLSSharedFileListSessionLoginItems,
                                                            NULL);

    if (loginItems) {
        UInt32 seed = 0U;

        NSArray* loginItemsArray = (NSArray*)LSSharedFileListCopySnapshot(loginItems, &seed);

        CFURLRef appURL = bundlePath();
        for (id item in loginItemsArray) {
            LSSharedFileListItemRef itemRef = (LSSharedFileListItemRef)item;

            UInt32 resolutionFlags = kLSSharedFileListNoUserInteraction | kLSSharedFileListDoNotMountVolumes;
            CFURLRef URL = NULL;
            OSStatus err = LSSharedFileListItemResolve(itemRef, resolutionFlags, &URL, NULL);

            if (err == noErr) {
                if (CFEqual(URL, appURL)) {
                    LSSharedFileListItemRemove(loginItems,itemRef);

                    CFRelease(URL);
                    break;
                }
            }
        }

        CFRelease(loginItems);
    }



    return true;
}
