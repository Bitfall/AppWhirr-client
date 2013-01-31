#include "mac_applescriptexecutor.h"

#include <Carbon/Carbon.h>
#include <QDebug>

//
// interface
@interface AppleScriptExecutor_p : NSObject {

}

- (BOOL)executeAppleScript:(NSString *)appleScriptCode;

@end

//
// implementation
@implementation AppleScriptExecutor_p

- (BOOL)executeAppleScript:(NSString *)appleScriptCode
{
    BOOL _ret = NO;

    NSDictionary *error = [NSDictionary new];
    NSString *script = appleScriptCode;
    NSAppleScript *appleScript = [[NSAppleScript new] initWithSource:script];
    NSAppleEventDescriptor *execResultDescriptor = [appleScript executeAndReturnError:&error];
    if (execResultDescriptor != nil) {
        NSLog(@"success: %@", execResultDescriptor);
        _ret = YES;
    } else {
        NSLog(@"failure of apple-scrip-execution: %@", appleScriptCode);
    }

    if( [error count] ) {
        NSLog(@"'error' is not empty: %@", error);
    }
    else {
        NSLog(@"'error' is empty - OK");
    }

    return _ret;
}

@end


//
// C++/ObjC interface impl.
void Mac_AppleScriptExecutor::executeAppleScript(QString appleScriptCode)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    NSString *appleScriptCodeAsNSString = [NSString stringWithUTF8String:appleScriptCode.toUtf8()];
    AppleScriptExecutor_p *p_asExecutor = [AppleScriptExecutor_p new];
    if( [p_asExecutor executeAppleScript:appleScriptCodeAsNSString] == YES ) {
        qDebug() << "Success";
    }
    else {
        qDebug() << "Failed";
    }
    [p_asExecutor release];

    [pool drain];
}
