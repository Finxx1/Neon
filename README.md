# Neon
A lightweight and cross-platform library with a similar purpose to SDL

### Example

```c
#include "Neon.h"

NeonDraw* draw(void) {
	NeonDraw* r = NeonCreateDraw();
	
	return r;
}

void update(double DeltaTime) {
	
}

int main(int argc, const char * argv[]) {
	// Creates a 1280x720 window at 60 FPS
	NeonRun("Example Neon program", 0, 0, 1280, 720, draw, update, 60);
	return 0;
}
```
### 3rd party libraries
Neon uses third party libraries in cases where not using them hinders development. As of now, it uses these:
### STB_IMAGE
STB_IMAGE is used for reading image data across several formats. STB_IMAGE is licensed under the CC0 public-domain license.