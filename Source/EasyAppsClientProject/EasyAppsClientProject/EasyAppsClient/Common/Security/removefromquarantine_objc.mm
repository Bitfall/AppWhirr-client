#include "removefromquarantine.h"

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

#import <dlfcn.h>
#import <errno.h>
#import <sys/xattr.h>

//
// http://opensource.apple.com/source/X11server/X11server-106.5/Sparkle/sparkle.git/SUPlainInstallerInternals.m?txt
//

int RemoveFromQuarantine::removeXAttr(const char* name, const char* fromFile, int options) {
    NSString* file = [NSString stringWithUTF8String:fromFile];

    typedef int (*removexattr_type)(const char*, const char*, int);
    // Reference removexattr directly, it's in the SDK.
    static removexattr_type removexattr_func = removexattr;

    // Make sure that the symbol is present.  This checks the deployment
    // target instead of the SDK so that it's able to catch dlsym failures
    // as well as the null symbol that would result from building with the
    // 10.4 SDK and a lower deployment target, and running on 10.3.
    if (!removexattr_func) {
        errno = ENOSYS;
        return -1;
    }

    const char* path = NULL;
    @try {
        path = [file fileSystemRepresentation];
    }
    @catch (id exception) {
        // -[NSString fileSystemRepresentation] throws an exception if it's
        // unable to convert the string to something suitable.  Map that to
        // EDOM, "argument out of domain", which sort of conveys that there
        // was a conversion failure.
        errno = EDOM;
        return -1;
    }

    return removexattr_func(path, name, options);
}

void RemoveFromQuarantine::releaseFromQuarantine(const char* file) {
    NSString* root = [NSString stringWithUTF8String:file];

    const char* quarantineAttribute = "com.apple.quarantine";
    const int removeXAttrOptions = XATTR_NOFOLLOW;

    removeXAttr(quarantineAttribute,
                (const char*)[root UTF8String],
                removeXAttrOptions);

    // Only recurse if it's actually a directory.  Don't recurse into a
    // root-level symbolic link.

    // Supporting only >= 10.6
//#if MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4
//    NSDictionary* rootAttributes = [[NSFileManager defaultManager] fileAttributesAtPath:root traverseLink:NO];
//#else
    NSDictionary* rootAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:root error:nil];
//#endif
    NSString* rootType = [rootAttributes objectForKey:NSFileType];

    if (rootType == NSFileTypeDirectory) {
        // The NSDirectoryEnumerator will avoid recursing into any contained
        // symbolic links, so no further type checks are needed.
        NSDirectoryEnumerator* directoryEnumerator = [[NSFileManager defaultManager] enumeratorAtPath:root];
        NSString* file = nil;
        while ((file = [directoryEnumerator nextObject])) {
            removeXAttr(quarantineAttribute,
                        (const char*)[[root stringByAppendingPathComponent:file] UTF8String],
                        removeXAttrOptions);
//            [self removeXAttr:quarantineAttribute
//                     fromFile:[root stringByAppendingPathComponent:file]
//                      options:removeXAttrOptions];
        }
    }
}
