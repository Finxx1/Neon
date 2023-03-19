//
//  Neon.m
//  Neon
//
//  Created by Finxx1 on 3/11/23.
//

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

- (void)keyDown:(NSEvent *)event {
	___NeonKeys[event.keyCode] = 1;
	___NeonKeysDown[event.keyCode] = 1;
}

- (void)keyUp:(NSEvent *)event {
	___NeonKeys[event.keyCode] = 0;
	___NeonKeysUp[event.keyCode] = 1;
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
        }
    }
}
@end

/// Adds an image to the internal list
void NeonAddImage(char* filename) {
    ___imagecount++;
    ___images = realloc(___images, ___imagecount * sizeof(CGImageRef));
    
	NSData *imageData = [[NSData alloc] initWithContentsOfFile:[NSString stringWithCString:filename encoding:NSUTF8StringEncoding]];
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

/// Adds an audio file to the internal list
void NeonAddAudio(char* filename) {
	if (___sounds == nil) ___sounds = [[NSMutableArray<NSSound*> alloc] init];
	
	[___sounds addObject:[[NSSound alloc] initWithContentsOfFile:[NSString stringWithCString:filename encoding:NSUTF8StringEncoding] byReference:NO]];
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
	[NSApp.mainWindow setTitle:[NSString stringWithCString:name encoding:NSUTF8StringEncoding]];
}

/// Change the window resolution during execution
void NeonChangeResolution(int width, int height) {
	[NSApp.mainWindow setFrame:NSMakeRect(NSApp.mainWindow.frame.origin.x, NSApp.mainWindow.frame.origin.y, width, height) display:NO]; // Don't redraw since program might not be finished updating
}

/// Creates a Cocoa window with a view and takes a draw function.
void NeonRun(char* name, int x, int y, int width, int height, NeonDraw* (*drawfunc)(void), void(*updatefunc)(double), unsigned short fps) {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    NSWindow* win = [[NSWindow alloc] initWithContentRect:NSMakeRect(x, y, width, height) styleMask:(NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskMiniaturizable) backing:NSBackingStoreBuffered defer:NO];
    [win setTitle:[NSString stringWithCString:name encoding:NSUTF8StringEncoding]];
    
    ___drawFunc = drawfunc;
    ___updateFunc = updatefunc;
    
    NSView* view = [[NeonQuartzView alloc] initWithFrame:NSMakeRect(0, 0, (float)width, (float)height)];
    
    [win setContentView:view];
    
    [win makeKeyAndOrderFront:nil];
    
    [NSApp activateIgnoringOtherApps:YES];
    
    //free(_draw);
    _draw = ___drawFunc();
    
	[NSTimer scheduledTimerWithTimeInterval:1.0/(double)(fps) repeats:YES block:^(NSTimer* timer){___updateFunc(1); _draw = ___drawFunc(); memset(___NeonKeysDown, 0, 255 * sizeof(_Bool)); memset(___NeonKeysUp, 0, 255 * sizeof(_Bool)); win.contentView.needsDisplay = YES;}];
    
    [NSApp run];
}
