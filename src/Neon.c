//
//  Neon.c
//  Neon
//
//  Created by Finxx1 on 3/11/23.
//

#include "Neon.h"
#include <stdlib.h>

#include <stdarg.h>

void NeonDrawRect(NeonDraw* draw, int x, int y, int width, int height, unsigned char color[4]) {
    draw->InstructionCount++;
    draw->Instructions = realloc(draw->Instructions, sizeof(NeonDrawInstruction) * draw->InstructionCount);
    
    // This is dumb, but it works fast and makes it easy for the user.
    draw->Instructions[draw->InstructionCount - 1].ID = 1;
	
    draw->Instructions[draw->InstructionCount - 1].RectParameters.width = width;
    draw->Instructions[draw->InstructionCount - 1].RectParameters.height = height;
    draw->Instructions[draw->InstructionCount - 1].RectParameters.x = x;
    draw->Instructions[draw->InstructionCount - 1].RectParameters.y = y;
    draw->Instructions[draw->InstructionCount - 1].RectParameters.color[0] = color[0];
    draw->Instructions[draw->InstructionCount - 1].RectParameters.color[1] = color[1];
    draw->Instructions[draw->InstructionCount - 1].RectParameters.color[2] = color[2];
    draw->Instructions[draw->InstructionCount - 1].RectParameters.color[3] = color[3];
}

void NeonDrawImage(NeonDraw* draw, int x, int y, int width, int height, unsigned long ID) {
    draw->InstructionCount++;
    draw->Instructions = realloc(draw->Instructions, sizeof(NeonDrawInstruction) * draw->InstructionCount);
    
    draw->Instructions[draw->InstructionCount - 1].ID = 2;
    
    draw->Instructions[draw->InstructionCount - 1].ImageParameters.x = x;
    draw->Instructions[draw->InstructionCount - 1].ImageParameters.y = y;
    draw->Instructions[draw->InstructionCount - 1].ImageParameters.width = width;
    draw->Instructions[draw->InstructionCount - 1].ImageParameters.height = height;
    draw->Instructions[draw->InstructionCount - 1].ImageParameters.id = ID;
}

void NeonDrawPolygon(NeonDraw* draw, unsigned char color[4], unsigned int verticies, ...) {
	draw->InstructionCount++;
	draw->Instructions = realloc(draw->Instructions, sizeof(NeonDrawInstruction) * draw->InstructionCount);
	
	draw->Instructions[draw->InstructionCount - 1].ID = 3;
	
	draw->Instructions[draw->InstructionCount - 1].PolygonParameters.vertices = verticies;
	draw->Instructions[draw->InstructionCount - 1].PolygonParameters.x = calloc(verticies, sizeof(int));
	draw->Instructions[draw->InstructionCount - 1].PolygonParameters.y = calloc(verticies, sizeof(int));
	
	va_list args;
	va_start(args, verticies);
	for (unsigned int i = 0; i < verticies; i++) {
		draw->Instructions[draw->InstructionCount - 1].PolygonParameters.x[i] = va_arg(args, int);
		draw->Instructions[draw->InstructionCount - 1].PolygonParameters.y[i] = va_arg(args, int);
	}
	va_end(args);
	
	draw->Instructions[draw->InstructionCount - 1].PolygonParameters.color[0] = color[0];
	draw->Instructions[draw->InstructionCount - 1].PolygonParameters.color[1] = color[1];
	draw->Instructions[draw->InstructionCount - 1].PolygonParameters.color[2] = color[2];
	draw->Instructions[draw->InstructionCount - 1].PolygonParameters.color[3] = color[3];
}
