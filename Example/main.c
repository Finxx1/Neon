//
//  main.c
//  Example
//
//  Created by Finxx1 on 3/18/23.
//

#include "Neon.h"

#include <stdio.h>
#include <stdbool.h>

short* mx, * my;

NeonDraw* draw(void) {
	NeonDraw* r = NeonCreateDraw();
	
	return r;
}

void undo(void) {
	printf("undone!\n");
}

bool start = true;

void update(double DeltaTime) {
	
}

int main(int argc, const char * argv[]) {
	mx = NeonMouseX();
	my = NeonMouseY();
	
	NeonRun("Example Neon program", 0, 0, 1280, 720, draw, update, 60);
    return 0;
}
