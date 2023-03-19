//
//  Neon.h
//  Neon
//
//  Created by Finxx1 on 3/11/23.
//

#ifndef Neon_h
#define Neon_h

#include <stdlib.h>
#define NeonCreateDraw() calloc(1, sizeof(NeonDraw))

#define NeonColor(r, g, b, a) (unsigned char []){r, g, b, a}

// Draw instruction IDs
// 1 - Rectangle
// 2 - Image

typedef struct {
    unsigned int ID;
    
	union {
		struct {
			int x, y, width, height;
			unsigned char color[4];
			_Bool filled;
		} RectParameters;
		
		struct {
			int x, y, width, height;
			unsigned long id;
		} ImageParameters;
	};
} NeonDrawInstruction;

// Rendering
/// Used to render.
typedef struct {
    unsigned long InstructionCount;
	NeonDrawInstruction* Instructions;
} NeonDraw;

/// Adds a rectangle to a draw.
void NeonDrawRect(NeonDraw* draw, int x, int y, int width, int height, unsigned char color[4]);

/// Adds an image to the internal list
void NeonAddImage(char* filename);

/// Adds an image to the internal list
void NeonAddImageRaw(unsigned char* bytes, unsigned long len);

/// Adds an image to a draw
void NeonDrawImage(NeonDraw* draw, int x, int y, int width, int height, unsigned long ID);
// /Rendering

// Audio
/// Adds an audio file to the internal list
void NeonAddAudio(char* filename);

/// Adds audio data to the internal list
void NeonAddAudioRaw(unsigned char* bytes, unsigned long len);

/// Plays the specified audio ID
void NeonPlayAudio(unsigned long ID, _Bool repeats);

/// Stops the specified audio ID
void NeonStopAudio(unsigned long ID);
// /Audio

// Input
/// Returns an array of all keys currently being pressed. The returned pointer is managed by the engine and will be alive for as long as the program is running.
_Bool* NeonKeys(void);

/// Returns an array of all keys that were just pressed. The returned pointer is managed by the engine and will be alive for as long as the program is running.
_Bool* NeonKeysDown(void);

/// Returns an array of all keys that were just released. The returned pointer is managed by the engine and will be alive for as long as the program is running.
_Bool* NeonKeysUp(void);

/// Returns the current mouse X position. The returned pointer is managed by the engine and will be alive for as long as the program is running.
short* NeonMouseX(void);

/// Returns the current mouse Y position. The returned pointer is managed by the engine and will be alive for as long as the program is running.
short* NeonMouseY(void);
// /Input

// OS
/// Creates a window. WARNING: This function will never return! It should be called last!
void NeonRun(char* name, int x, int y, int width, int height, NeonDraw* (drawFunc)(void), void (updateFunc)(double), unsigned short fps);

/// Change the window title during execution
void NeonChangeName(char* name);

/// Change the window resolution during execution
void NeonChangeResolution(int width, int height);
// /OS
#endif /* Neon_h */
