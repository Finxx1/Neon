//
//  Neon.m
//  Neon
//
//  Created by Finxx1 on 3/11/23.
//

// Here be dragons.

#import <Cocoa/Cocoa.h> // Quartz

#include "Neon.h"

NeonDraw* (*___drawFunc)(void);
NeonDraw* _draw;

void (*___updateFunc)(double);

CGImageRef* ___images = NULL;
size_t ___imagecount = 0;

NSMutableArray<NSSound*>* ___sounds = nil;

_Bool ___NeonKeys[255];
_Bool ___NeonKeysDown[255];
_Bool ___NeonKeysUp[255];

short ___NeonMouseX = 0;
short ___NeonMouseY = 0;

_Bool* NeonKeys(void) { return ___NeonKeys; }
_Bool* NeonKeysDown(void) { return ___NeonKeysDown; }
_Bool* NeonKeysUp(void) { return ___NeonKeysUp; }

short* NeonMouseX(void) { return &___NeonMouseX; }
short* NeonMouseY(void) { return &___NeonMouseY; }

NSString* NSStringFromCString(char* string) {
	return [NSString stringWithCString:string encoding:NSUTF8StringEncoding];
}

@interface NeonQuartzView : NSView {
	NSTrackingArea* trackingArea; // Mouse updates
}

- (void)drawRect:(NSRect)dirtyRect;
@end

@implementation NeonQuartzView
- (BOOL)canBecomeKeyView {
	return YES;
}

- (BOOL)acceptsFirstResponder {
	return YES;
}

- (void) viewWillMoveToWindow:(NSWindow *)newWindow {
	[super viewWillMoveToWindow:newWindow];
	
	NSTrackingArea* trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds] options: (NSTrackingMouseMoved | NSTrackingActiveAlways) owner:self userInfo:nil];
	[self addTrackingArea:trackingArea];
}

// just so macos doesnt think we are pressing buttons at an invalid moment and play a dumb sound
- (void)keyDown:(NSEvent *)event {}
- (void)keyUp:(NSEvent *)event {}

- (void)flagsChanged:(NSEvent *)event {
	if ((event.modifierFlags & 1)) {
		if (!___NeonKeys[0x39]) ___NeonKeysDown[0x39] = true;
		___NeonKeys[0x39] = true;
	} else {
		if (___NeonKeys[0x39]) ___NeonKeysUp[0x39] = true;
		___NeonKeys[0x39] = false;
	}
	if ((event.modifierFlags & 2)) {
		if (!___NeonKeys[0x38]) ___NeonKeysDown[0x38] = true;
		___NeonKeys[0x38] = true;
	} else {
		if (___NeonKeys[0x38]) ___NeonKeysUp[0x38] = true;
		___NeonKeys[0x38] = false;
	}
	if ((event.modifierFlags & 4)) {
		if (!___NeonKeys[0x3B]) ___NeonKeysDown[0x3B] = true;
		___NeonKeys[0x3B] = true;
	} else {
		if (___NeonKeys[0x3B]) ___NeonKeysUp[0x3B] = true;
		___NeonKeys[0x3B] = false;
	}
	if ((event.modifierFlags & 8)) {
		if (!___NeonKeys[0x3A]) ___NeonKeysDown[0x3A] = true;
		___NeonKeys[0x3A] = true;
	} else {
		if (___NeonKeys[0x3A]) ___NeonKeysUp[0x3A] = true;
		___NeonKeys[0x3A] = false;
	}
	if ((event.modifierFlags & 16)) {
		if (!___NeonKeys[0x37]) ___NeonKeysDown[0x37] = true;
		___NeonKeys[0x37] = true;
	} else {
		if (___NeonKeys[0x37]) ___NeonKeysUp[0x37] = true;
		___NeonKeys[0x37] = false;
	}
}

- (void)mouseMoved:(NSEvent *)event {
	___NeonMouseX = [self.window mouseLocationOutsideOfEventStream].x;
	___NeonMouseY = self.frame.size.height - [self.window mouseLocationOutsideOfEventStream].y;
}

- (void)drawRect:(NSRect)dirtyRect {
    
    CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
    
    for (unsigned long i = 0; i < _draw->InstructionCount; i++) {
        switch (_draw->Instructions[i].ID) {
            case 1: { // Rectangle
				CGContextSetRGBFillColor(context, (float)(_draw->Instructions[i].RectParameters.color[0]) / 255.0, (float)(_draw->Instructions[i].RectParameters.color[1]) / 255.0, (float)(_draw->Instructions[i].RectParameters.color[2]) / 255.0, (float)(_draw->Instructions[i].RectParameters.color[3]) / 255.0);
				CGContextSetRGBStrokeColor(context, (float)(_draw->Instructions[i].RectParameters.color[0]) / 255.0, (float)(_draw->Instructions[i].RectParameters.color[1]) / 255.0, (float)(_draw->Instructions[i].RectParameters.color[2]) / 255.0, (float)(_draw->Instructions[i].RectParameters.color[3]) / 255.0);
				CGContextFillRect(context, CGRectMake(_draw->Instructions[i].RectParameters.x, self.window.contentView.frame.size.height - _draw->Instructions[i].RectParameters.y - _draw->Instructions[i].RectParameters.height, _draw->Instructions[i].RectParameters.width, _draw->Instructions[i].RectParameters.height));
				
				break;
            }
                
            case 2: { // Image
				CGContextDrawImage(context, CGRectMake(_draw->Instructions[i].ImageParameters.x, self.window.contentView.frame.size.height - _draw->Instructions[i].ImageParameters.y - _draw->Instructions[i].ImageParameters.height, _draw->Instructions[i].ImageParameters.width, _draw->Instructions[i].ImageParameters.height), ___images[_draw->Instructions[i].ImageParameters.id]);
				
				break;
            }
				
			case 3: { // Polygon
				CGContextSetRGBFillColor(context, _draw->Instructions[i].PolygonParameters.color[0] / 255.0, _draw->Instructions[i].PolygonParameters.color[1] / 255.0, _draw->Instructions[i].PolygonParameters.color[2] / 255.0, _draw->Instructions[i].PolygonParameters.color[3] / 255.0);
				CGContextSetRGBStrokeColor(context, _draw->Instructions[i].PolygonParameters.color[0] / 255.0, _draw->Instructions[i].PolygonParameters.color[1] / 255.0, _draw->Instructions[i].PolygonParameters.color[2] / 255.0, _draw->Instructions[i].PolygonParameters.color[3] / 255.0);
				
				CGContextBeginPath(context);
				
				CGContextMoveToPoint(context, _draw->Instructions[i].PolygonParameters.x[0], self.window.contentView.frame.size.height - _draw->Instructions[i].PolygonParameters.y[0]);
				
				for (unsigned int j = 1; j < _draw->Instructions[i].PolygonParameters.vertices; j++) {
					CGContextAddLineToPoint(context, _draw->Instructions[i].PolygonParameters.x[j], self.window.contentView.frame.size.height - _draw->Instructions[i].PolygonParameters.y[j]);
				}
				
				CGContextClosePath(context);
				CGContextDrawPath(context, kCGPathFill);
			}
        }
    }
}
@end

/// Adds an image to the internal list
void NeonAddImage(char* filename) {
    ___imagecount++;
    ___images = realloc(___images, ___imagecount * sizeof(CGImageRef));
    
	NSData *imageData = [[NSData alloc] initWithContentsOfFile:NSStringFromCString(filename)];
    CGImageSourceRef source = CGImageSourceCreateWithData((__bridge CFDataRef)imageData, NULL);
    ___images[___imagecount - 1] = CGImageSourceCreateImageAtIndex(source, 0, NULL);
}

/// Adds an image to the internal list
void NeonAddImageRaw(unsigned char* bytes, size_t len) {
	___imagecount++;
	___images = realloc(___images, ___imagecount * sizeof(CGImageRef));
	
	NSData *imageData = [[NSData alloc] initWithBytes:bytes length:len];
	CGImageSourceRef source = CGImageSourceCreateWithData((__bridge CFDataRef)imageData, NULL);
	___images[___imagecount - 1] = CGImageSourceCreateImageAtIndex(source, 0, NULL);
}

_Bool ___soundsinitialized = NO;

/// Adds an audio file to the internal list
void NeonAddAudio(char* filename) {
	if (___soundsinitialized == NO) {
		___sounds = [[NSMutableArray<NSSound*> alloc] init];
		___soundsinitialized = YES;
	}
	
	NSSound* sound = [[NSSound alloc] initWithContentsOfFile:NSStringFromCString(filename) byReference:NO];
	
	[___sounds addObject:sound];
}

/// Adds audio data to the internal list
void NeonAddAudioRaw(unsigned char* bytes, size_t len) {
	if (___sounds == nil) ___sounds = [[NSMutableArray<NSSound*> alloc] init];
	
	[___sounds addObject:[[NSSound alloc] initWithData:[[NSData alloc] initWithBytes:bytes length:len]]];
}

/// Plays the specified audio index and returns an ID that can be used to stop playback
void NeonPlayAudio(unsigned long ID, _Bool repeats) {
	[___sounds[ID] stop]; // For some reason, a sound can only be played once at a time, so make sure to end the previous time if it hasn't been done already.
	if (repeats) [___sounds[ID] setLoops:YES]; // _Bool ≠ BOOL
	[___sounds[ID] play];
}

/// Stops the specified audio ID
void NeonStopAudio(unsigned long ID) {
	[___sounds[ID] stop];
}

/// Change the window title during execution
void NeonChangeName(char* name) {
	[NSApp.mainWindow setTitle:NSStringFromCString(name)];
}

/// Change the window resolution during execution
void NeonChangeResolution(int width, int height) {
	[NSApp.mainWindow setFrame:NSMakeRect(NSApp.mainWindow.frame.origin.x, NSApp.mainWindow.frame.origin.y, width, height) display:NO]; // Don't redraw since program might not be finished updating
}

// dumb cocoa stuff
@interface NeonDelegate : NSObject <NSApplicationDelegate>

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication;
- (void)applicationDidFinishLaunching:(NSNotification *)notification;

@end

@implementation NeonDelegate

// sdl does this too, so why wouldnt i
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
	return YES;
}

// if i put this in neonrun the menubar refuses to work. why? no idea.
- (void)applicationDidFinishLaunching:(NSNotification *)notification {
	[NSApp activateIgnoringOtherApps:YES];
}

@end

/// Creates a Cocoa window with a view and takes a draw function.
void NeonRun(char* name, int x, int y, int width, int height, NeonDraw* (*drawfunc)(void), void(*updatefunc)(double), unsigned short fps) {
    [NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	NeonDelegate* delegate = [[NeonDelegate alloc] init];
	[NSApp setDelegate:delegate];
	
	// https://stackoverflow.com/a/68511530
	NSMenu *menubar = [NSMenu new];
	NSMenuItem *menuBarItem = [NSMenuItem new];
	[menubar addItem:menuBarItem];
	[NSApp setMainMenu:menubar];
	NSMenu *appMenu = [NSMenu new];
	NSMenuItem *quitMenuItem = [[NSMenuItem alloc] initWithTitle:@"Quit"
	action:@selector(terminate:) keyEquivalent:@""];
	[appMenu addItem:quitMenuItem];
	[menuBarItem setSubmenu:appMenu];
    
    NSWindow* win = [[NSWindow alloc] initWithContentRect:NSMakeRect(x, y, width, height) styleMask:(NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskMiniaturizable) backing:NSBackingStoreBuffered defer:NO];
    [win setTitle:NSStringFromCString(name)];
    
    ___drawFunc = drawfunc;
    ___updateFunc = updatefunc;
    
    NSView* view = [[NeonQuartzView alloc] initWithFrame:NSMakeRect(0, 0, (float)width, (float)height)];
    
    [win setContentView:view];
    
    [win makeKeyAndOrderFront:nil];
    
    //free(_draw);
    _draw = ___drawFunc();
    
	[NSTimer scheduledTimerWithTimeInterval:1.0/(double)(fps) repeats:YES block:^(NSTimer* timer){___updateFunc(1); _draw = ___drawFunc(); memset(___NeonKeysDown, 0, 255 * sizeof(_Bool)); memset(___NeonKeysUp, 0, 255 * sizeof(_Bool)); win.contentView.needsDisplay = YES;}];
	
	[NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown handler:^NSEvent * _Nullable(NSEvent * _Nonnull event) {
			___NeonKeys[event.keyCode] = 1;
			___NeonKeysDown[event.keyCode] = 1;
		return event;
	}];
	[NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyUp handler:^NSEvent * _Nullable(NSEvent * _Nonnull event) {
		___NeonKeys[event.keyCode] = 0;
		___NeonKeysUp[event.keyCode] = 1;
		return event;
	}];
    
    [NSApp run];
}
