//
//  Neon.c
//  Neon
//
//  Created by Finxx1 on 3/11/23.
//

#include "Neon.h"
#include <stdlib.h>

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

void NeonDrawImage(NeonDraw* draw, int x, int y, int width, int height, size_t ID) {
    draw->InstructionCount++;
    draw->Instructions = realloc(draw->Instructions, sizeof(NeonDrawInstruction) * draw->InstructionCount);
    
    draw->Instructions[draw->InstructionCount - 1].ID = 2;
    
    draw->Instructions[draw->InstructionCount - 1].ImageParameters.x = x;
    draw->Instructions[draw->InstructionCount - 1].ImageParameters.y = y;
    draw->Instructions[draw->InstructionCount - 1].ImageParameters.width = width;
    draw->Instructions[draw->InstructionCount - 1].ImageParameters.height = height;
    draw->Instructions[draw->InstructionCount - 1].ImageParameters.id = ID;
}
