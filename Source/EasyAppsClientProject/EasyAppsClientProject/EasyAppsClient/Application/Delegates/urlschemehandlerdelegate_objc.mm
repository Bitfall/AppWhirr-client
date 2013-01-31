#include "urlschemehandlerdelegate.h"

#include <QMessageBox>

// MAC - nezd at, igyekeztem jol atirni -> igazabol csak azt valtoztattam meg, hogy nem fixen MyApplication::instance -szal kommunikal hanem egy beadott delegate-et hasznal

@interface EventHandler : NSObject {
    BOOL            isRegistered;
    UrlSchemeHandlerDelegate *_schemeHandlerDelegate;
}
@end

@implementation EventHandler

- (id)initWithDelegate:(UrlSchemeHandlerDelegate *)schemeHandlerDelegate {
    self = [super init];
    if (self) {
        isRegistered = NO;
        _schemeHandlerDelegate = schemeHandlerDelegate;

        NSNotificationCenter* defaultCenter = [NSNotificationCenter defaultCenter];
        [defaultCenter addObserver:self
                          selector:@selector(applicationDidFinishLaunching:)
                              name:NSApplicationDidBecomeActiveNotification
                            object:nil];
    }
    return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    if (!isRegistered) {
        isRegistered = YES;

        NSAppleEventManager *appleEventManager = [NSAppleEventManager sharedAppleEventManager];
        [appleEventManager setEventHandler:self andSelector:@selector(handleGetURLEvent:withReplyEvent:) forEventClass:kInternetEventClass andEventID:kAEGetURL];
    }
}

- (void)handleGetURLEvent:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent {
    NSString* url = [[event paramDescriptorForKeyword:keyDirectObject] stringValue];
    NSString* urlToOpen = url;

    _schemeHandlerDelegate->__schemeCalledWithURL([urlToOpen UTF8String]);
}

@end

// Cpp
class UrlSchemeHandlerDelegate::MacEventProcessor {
public:
    NSAutoreleasePool*      pool;
    EventHandler*           eventHandler;
};

void UrlSchemeHandlerDelegate::setupMacEvents() {
    macEventProcessor = new MacEventProcessor;
    macEventProcessor->pool = [[NSAutoreleasePool alloc] init];
    macEventProcessor->eventHandler = [[EventHandler alloc] initWithDelegate:this];
}

void UrlSchemeHandlerDelegate::removeMacEvents() {
    [macEventProcessor->eventHandler release];
    [macEventProcessor->pool drain];

    delete macEventProcessor;
}
