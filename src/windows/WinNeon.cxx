#include "Neon.h"

#pragma comment(lib, "d2d1.lib")

#include <Windows.h>
#include <d2d1.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

LPWSTR UTF8ToWChar(char* name) {
	int titlebufsize = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
	wchar_t* titlebuf = (wchar_t*)malloc(sizeof(wchar_t) * titlebufsize);
	MultiByteToWideChar(CP_UTF8, 0, name, -1, titlebuf, titlebufsize);

	return titlebuf;
}

#include <vector>

#define WIN_STYLE (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~(WS_MAXIMIZEBOX | WS_THICKFRAME)

_Bool ___keys[256];
_Bool ___keysdown[256];
_Bool ___keysup[256];

short ___NeonMouseX = 0;
short ___NeonMouseY = 0;

_Bool* NeonKeys(void) { return ___keys; }
_Bool* NeonKeysDown(void) { return ___keysdown; }
_Bool* NeonKeysUp(void) { return ___keysup; }

short* NeonMouseX(void) { return &___NeonMouseX; }
short* NeonMouseY(void) { return &___NeonMouseY; }

static NeonDraw* (*___drawfunc)(void);
static void (*___updatefunc)(double);

static HRESULT result;

static ID2D1Factory* d2dfactory = NULL;
static ID2D1HwndRenderTarget* d2dtarget = NULL;

typedef struct {
	ID2D1Bitmap* d2dbitmap;
} image;

static std::vector<image> _images(1);

void NeonAddImage(char* filename) {
	int x, y, z;
	stbi_uc* dat = stbi_load(filename, &x, &y, &z, 4);

	D2D1_BITMAP_PROPERTIES props = {};
	props.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	props.dpiX = 96;
	props.dpiY = 96;

	image foo;
	result = d2dtarget->CreateBitmap(D2D1::SizeU(x, y), dat, x * 4, props, &foo.d2dbitmap);

	stbi_image_free(dat);

	_images.push_back(foo);
}

void NeonAddImageRaw(unsigned char* bytes, unsigned long len) {
	int x, y, z;
	stbi_uc* dat = stbi_load_from_memory(bytes, len, &x, &y, &z, 4);

	D2D1_BITMAP_PROPERTIES props = {};
	props.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	props.dpiX = 96;
	props.dpiY = 96;

	image foo;
	result = d2dtarget->CreateBitmap(D2D1::SizeU(x, y), dat, x * 4, props, &foo.d2dbitmap);

	stbi_image_free(dat);

	_images.push_back(foo);
}

void docrap(HWND hwnd, UINT, UINT_PTR, DWORD) {
	___updatefunc(1);
	InvalidateRect(hwnd, NULL, FALSE);
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		case WM_CREATE: {
			RECT rc;
			GetClientRect(hwnd, &rc);

			result = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &d2dfactory);
			result = d2dfactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &d2dtarget);

			BYTE foo[256];
			GetKeyboardState(foo);
			_Bool foo2[256];
			for (int i = 0; i < 256; i++) {
				___keys[i] = (foo[i] & 0x80) != 0;
				___keysdown[i] = (foo[i] & 0x80) != 0;
			}
			___updatefunc(0);

			return 0;
		}

		case WM_PAINT: {
			NeonDraw* draw = ___drawfunc();

			d2dtarget->BeginDraw();

			for (int i = 0; i < draw->InstructionCount; i++) {
				NeonDrawInstruction foo = draw->Instructions[i];
				switch (foo.ID) {
					case 1: {
						D2D1::ColorF color(foo.RectParameters.color[0] / 255.0, foo.RectParameters.color[1] / 255.0, foo.RectParameters.color[2] / 255.0, foo.RectParameters.color[0] / 255.0);

						ID2D1SolidColorBrush* brush;
						result = d2dtarget->CreateSolidColorBrush(color, &brush);

						D2D1_RECT_F rect = D2D1::RectF(foo.RectParameters.x, foo.RectParameters.y, foo.RectParameters.width + foo.RectParameters.x, foo.RectParameters.height + foo.RectParameters.y);

						d2dtarget->FillRectangle(rect, brush);

						brush->Release();
						break;
					}
					case 2: {
						D2D1_RECT_F dst = D2D1::RectF(foo.ImageParameters.x, foo.ImageParameters.y, foo.ImageParameters.width + foo.ImageParameters.x, foo.ImageParameters.height + foo.ImageParameters.y);
						d2dtarget->DrawBitmap(_images[foo.ImageParameters.id].d2dbitmap, dst);
						break;
					}
					case 3: {
						ID2D1PathGeometry* path = NULL;
						result = d2dfactory->CreatePathGeometry(&path);

						ID2D1GeometrySink* sink = NULL;
						result = path->Open(&sink);

						sink->BeginFigure(D2D1::Point2F(foo.PolygonParameters.x[0], foo.PolygonParameters.y[0]), D2D1_FIGURE_BEGIN_FILLED);
						for (int i = 1; i < foo.PolygonParameters.vertices; i++) {
							sink->AddLine(D2D1::Point2F(foo.PolygonParameters.x[i], foo.PolygonParameters.y[i]));
						}
						sink->EndFigure(D2D1_FIGURE_END_CLOSED);

						result = sink->Close();

						D2D1::ColorF color(foo.PolygonParameters.color[0] / 255.0, foo.PolygonParameters.color[1] / 255.0, foo.PolygonParameters.color[2] / 255.0, foo.PolygonParameters.color[0] / 255.0);

						ID2D1SolidColorBrush* brush;
						result = d2dtarget->CreateSolidColorBrush(color, &brush);

						d2dtarget->FillGeometry(path, brush);

						brush->Release();
						path->Release();
					}
				}
			}

			result = d2dtarget->EndDraw();

			return 0;
		}

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP: {
			WORD vkCode = LOWORD(wparam);

			BOOL isKeyReleased = (HIWORD(lparam) & KF_UP) == KF_UP;


			if (isKeyReleased) {
				___keysup[vkCode] = 1;
				___keys[vkCode] = 0;
			} else {
				BOOL wasKeyDown = (HIWORD(lparam) & KF_REPEAT) == KF_REPEAT;
				if (wasKeyDown) {
					___keysdown[vkCode] = 0;
				}
				else {
					___keysdown[vkCode] = 1;
					___keys[vkCode] = 1;
				}
			}

			return 0;
		}

		case WM_SIZE: {

			return 0;
		}

		case WM_DESTROY: {
			d2dtarget->Release();
			d2dfactory->Release();

			PostQuitMessage(0);

			return 0;
		}
		default: {
			return DefWindowProcW(hwnd, msg, wparam, lparam);
		}
	}
}

void NeonRun(char* name, int x, int y, int width, int height, NeonDraw* (drawFunc)(void), void (updateFunc)(double), unsigned short fps) {
	LPWSTR titlebuf = UTF8ToWChar(name);

	WNDCLASSEXW wc;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandleW(NULL);
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszClassName = titlebuf;
	wc.lpszMenuName = NULL;
	ATOM wcatom = RegisterClassExW(&wc);

	RECT winrect;
	winrect.left = 0;
	winrect.top = 0;
	winrect.right = width;
	winrect.bottom = height;

	___drawfunc = drawFunc;
	___updatefunc = updateFunc;

	AdjustWindowRect(&winrect, WIN_STYLE, FALSE);
	HWND hwnd = CreateWindowExW(0, (LPCWSTR)wcatom, titlebuf, WIN_STYLE, x, y, winrect.right - winrect.left, winrect.bottom - winrect.top, NULL, NULL, wc.hInstance, 0);

	SetTimer(hwnd, 2763, 1000 / fps, docrap);

	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return;
}