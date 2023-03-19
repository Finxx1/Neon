//
//  main.c
//  Example
//
//  Created by Finxx1 on 3/18/23.
//

#include "Neon.h"

NeonDraw* draw(void) {
	NeonDraw* r = NeonCreateDraw();
	
	return r;
}

void update(double DeltaTime) {
	
}

int main(int argc, const char * argv[]) {
	NeonRun("Example Neon program", 0, 0, 1280, 720, draw, update, 60);
    return 0;
}
