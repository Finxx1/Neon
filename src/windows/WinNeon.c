#include <asprintf.h>

#include <Neon.h>
#include <Windows.h>

#include <Windowsx.h>

#define GLEW_STATIC
#include <GL/glew.h>

#pragma comment(lib, "glew32s")

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

HWND hwnd;

GLenum err;

void Panic(wchar_t* message) {
    MessageBoxW(NULL, message, L"Neon ERROR", MB_OK);
    PostMessage(hwnd, WM_CLOSE, 0, 0);
}

typedef struct {
    double x, y;
} vec2;

wchar_t* UTF8toWchar(char* utf8) {
	wchar_t* r;
	size_t foo = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	r = (wchar_t*)malloc(foo * sizeof(wchar_t));

	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, r, foo);
	return r;
}

int w, h;
int fps;

vec2 CoordTranslate(int x, int y) {
    vec2 r;
    r.x = (double)x / (double)w * 2.0 - 1.0;
    r.y = (double)(h - y) / (double)h * 2.0 - 1.0;
    return r;
}

PIXELFORMATDESCRIPTOR pfd =
{
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
    PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
    32,                   // Colordepth of the framebuffer.
    0, 0, 0, 0, 0, 0,
    0,
    0,
    0,
    0, 0, 0, 0,
    24,                   // Number of bits for the depthbuffer
    8,                    // Number of bits for the stencilbuffer
    0,                    // Number of Aux buffers in the framebuffer.
    PFD_MAIN_PLANE,
    0,
    0, 0, 0
};

NeonDraw* _draw;
NeonDraw* (*_DrawFunc)(void);

void (*_updateFunc)(double);

_Bool ___keys[256];
_Bool ___keysdown[256];
_Bool ___keysup[256];

short mx, my;

_Bool* NeonKeys(void) {
    return ___keys;
}

_Bool* NeonKeysDown(void) {
    return ___keysdown;
}

_Bool* NeonKeysUp(void) {
    return ___keysup;
}

short* NeonMouseX(void) {
    return &mx;
}

short* NeonMouseY(void) {
    return &my;
}

double mxr, myr; // mouse x/y ratio

LRESULT dostuff(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    char foo[256];
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(hWnd, &p);
    mx = p.x * mxr;
    my = p.y * myr;
    if (!GetKeyboardState(&foo[0])) Panic(L"Failed to get keyboard state.");
    memset(___keys, 0, 256 * sizeof(_Bool));
    memset(___keysdown, 0, 256 * sizeof(_Bool));
    memset(___keysup, 0, 256 * sizeof(_Bool));
    for (int i = 0; i < 256; i++) {
        if (___keys[i] >= 0 && foo[i] < 0) {
            ___keysdown[i] = 1;
        }
        if (___keys[i] < 0 && foo[i] >= 0) {
            ___keysup[i] = 1;
        }
        if (foo[i] < 0) {
            ___keys[i] = 1;
        }
    }
    _updateFunc(1.0/(double)fps);
    InvalidateRect(hWnd, NULL, FALSE);
}

HGLRC glhandle;
HDC dc;

GLuint* textures = NULL;
size_t texturecount;

void NeonAddImage(char* filename) { // has to after context creation
    int imagewidth, imageheight, depth;
    stbi_uc* data = stbi_load(filename, &imagewidth, &imageheight, &depth, 4);

    if (!data) {
        Panic(L"Error loading texture.");
    }

    GLuint texid;

    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexImage2D(GL_TEXTURE_2D, 0, depth, imagewidth, imageheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);

    err = glGetError();
    if (err) {
        char* foo;
        asprintf(&foo, "OpenGL error: %d.", err);
        Panic(UTF8toWchar(foo));
    }

    texturecount++;
    textures = realloc(textures, texturecount * sizeof(texturecount));
    if (!textures) {
        Panic(L"Failed to allocate space for texture.");
    }
    textures[texturecount - 1] = texid;

    glBindTexture(GL_TEXTURE_2D, 0);
}

void NeonAddImageRaw(unsigned char* bytes, unsigned long len) { // has to after context creation
    int imagewidth, imageheight, depth;
    stbi_uc* data = stbi_loadf_from_memory(bytes, len, &imagewidth, &imageheight, &depth, 4);

    GLuint texid;

    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexImage2D(GL_TEXTURE_2D, 0, depth, imagewidth, imageheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);

    err = glGetError();
    if (err) {
        char* foo;
        asprintf(&foo, "OpenGL error: %d.", err);
        Panic(UTF8toWchar(foo));
    }

    texturecount++;
    textures = realloc(textures, texturecount * sizeof(texturecount));
    if (!textures) {
        Panic(L"Failed to allocate space for texture.");
    }
    textures[texturecount - 1] = texid;

    glBindTexture(GL_TEXTURE_2D, 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    switch (Msg) {
        case WM_DESTROY: {
            wglMakeCurrent(dc, NULL);
            wglDeleteContext(glhandle);

            PostQuitMessage(0);
            break;
        }

        case WM_CREATE: {
            // adjust client size
            RECT bar;
            GetWindowRect(hWnd, &bar);
            bar.right = bar.left + w;
            bar.bottom = bar.top + h;
            AdjustWindowRect(&bar, (WS_OVERLAPPEDWINDOW | WS_VISIBLE), FALSE);

            RECT aaaa;
            GetClientRect(hWnd, &aaaa);

            mxr = (double)w / (double)aaaa.right;
            myr = (double)h / (double)aaaa.bottom;

            SetTimer(hWnd, 2763, 1000 / fps, dostuff);

            dc = GetDC(hWnd);

            SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);

            glhandle = wglCreateContext(dc);
            wglMakeCurrent(dc, glhandle);

            glewInit();

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_TEXTURE_2D);

            mx = 0;
            my = 0;
            unsigned char foo[256];
            if (!GetKeyboardState(&foo[0])) Panic(L"Failed to get keyboard state.");
            memset(___keys, 0, 256 * sizeof(_Bool));
            memset(___keysdown, 0, 256 * sizeof(_Bool));
            memset(___keysup, 0, 256 * sizeof(_Bool));
            for (int i = 0; i < 256; i++) {
                if (foo[i] < 0) {
                    ___keys[i] = 1;
                    ___keysdown[i] = 1;
                }
            }

            _updateFunc(0);

            break;
        }
        
        case WM_MOUSEMOVE: {
            mx = GET_X_LPARAM(lParam);
            my = GET_Y_LPARAM(lParam);
            break;
        }
                       
        case WM_SIZE: {
            glViewport(0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

            break;
        }

        case WM_PAINT: {
            _draw = _DrawFunc();

            glClear(GL_COLOR_BUFFER_BIT);

            for (size_t i = 0; i < _draw->InstructionCount; i++) {
                switch (_draw->Instructions[i].ID) {
                    case 1: {
                        int x = _draw->Instructions[i].RectParameters.x;
                        int y = _draw->Instructions[i].RectParameters.y;
                        int _w = _draw->Instructions[i].RectParameters.width;
                        int _h = _draw->Instructions[i].RectParameters.height;

                        vec2 a = CoordTranslate(x, y);
                        vec2 b = CoordTranslate(x + _w, y + _h);

                        unsigned char* color = _draw->Instructions[i].RectParameters.color;

                        glBegin(GL_QUADS);
                        glColor4d((double)color[0] / 255.0, (double)color[1] / 255.0, (double)color[2] / 255.0, (double)color[3] / 255.0);
                        glVertex2d(a.x, a.y);
                        glVertex2d(a.x, b.y);
                        glVertex2d(b.x, b.y);
                        glVertex2d(b.x, a.y);
                        glEnd();

                        break;
                    }

                    case 2: {
                        int x = _draw->Instructions[i].ImageParameters.x;
                        int y = _draw->Instructions[i].ImageParameters.y;
                        int _w = _draw->Instructions[i].ImageParameters.width;
                        int _h = _draw->Instructions[i].ImageParameters.height;

                        glBindTexture(GL_TEXTURE_2D, textures[_draw->Instructions[i].ImageParameters.id]);

                        vec2 a = CoordTranslate(x, y);
                        vec2 b = CoordTranslate(x + _w, y + _h);

                        glBegin(GL_QUADS);
                        glColor4d(1.0, 1.0, 1.0, 1.0);

                        glTexCoord2d(0.0, 0.0);
                        glVertex2d(a.x, a.y);
                        glTexCoord2d(0.0, 1.0);
                        glVertex2d(a.x, b.y);
                        glTexCoord2d(1.0, 1.0);
                        glVertex2d(b.x, b.y);
                        glTexCoord2d(1.0, 0.0);
                        glVertex2d(b.x, a.y);
                        glEnd();

                        glBindTexture(GL_TEXTURE_2D, 0);

                        break;
                    }
                }
            }

            SwapBuffers(dc);

            break;
        }
    }

    return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

void NeonRun(char* name, int x, int y, int width, int height, NeonDraw* (drawFunc)(void), void (updateFunc)(double), unsigned short _fps) {
    WNDCLASSEXW wcx;
    wcx.cbSize = sizeof(wcx);
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcx.lpfnWndProc = WndProc;
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;
    wcx.hInstance = NULL;
    wcx.hIcon = NULL;
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = GetStockObject(WHITE_BRUSH);
    wcx.lpszMenuName = NULL;
    wcx.lpszClassName = L"neonforwindows";
    wcx.hIconSm = NULL;
    RegisterClassExW(&wcx);

    fps = _fps;

    w = width;
    h = height;

    _DrawFunc = drawFunc;
    _updateFunc = updateFunc;

    hwnd = CreateWindowW(L"neonforwindows", UTF8toWchar(name), (WS_OVERLAPPEDWINDOW | WS_VISIBLE), x, y, width, height, NULL, NULL, NULL, 0);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}