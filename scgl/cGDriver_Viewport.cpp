#include "cGDriver.h"
#include "GLSupport.h"

namespace nSCGL
{
#ifndef NDEBUG
	static void __stdcall LogGLMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) {
			// We know we're using the fixed function pipeline.
			// FUTURE: but what if we weren't?
			return;
		}

		static FILE* glMessageLog = nullptr;
		if (glMessageLog == nullptr) {
			glMessageLog = fopen("C:\\temp\\cGDriver.opengl.log", "w");
		}

		fprintf(glMessageLog, "[GLerr] [source: %x] [type: %x] [id: %x] [severity: %d] %s\n", source, type, id, severity, message);
		fflush(glMessageLog);
	}
#endif

	uint32_t cGDriver::CountVideoModes(void) const {
		return videoModeCount;
	}

	void cGDriver::GetVideoModeInfo(uint32_t dwIndex, sGDMode& gdMode) {
		if (dwIndex == -1 || dwIndex >= (uint32_t)videoModeCount) {
			SetLastError(DriverError::OUT_OF_RANGE);
			return;
		}

		sGDMode tmp = videoModes[dwIndex];
		gdMode = tmp;
	}

	void cGDriver::GetVideoModeInfo(sGDMode& gdMode) {
		return GetVideoModeInfo(currentVideoMode, gdMode);
	}

	void cGDriver::SetVideoMode(int32_t newModeIndex, void* hwndProc, bool showWindow, bool) {
		if (newModeIndex == -1) {
			ShowWindow(static_cast<HWND>(windowHandle), SW_HIDE);

			currentVideoMode = -1;
			windowWidth = 0;
			windowHeight = 0;

			SetLastError(DriverError::OK);
		}
		else if (newModeIndex < videoModeCount) {
			// Destroy the old window (if we have one) and create a new one, so
			// Windows doesn't get mad at us if we call SetPixelFormat twice.
			sGDMode const& newMode = videoModes[newModeIndex];
			bool fullscreen = newMode.isFullscreen;

			currentVideoMode = newModeIndex;
			windowWidth = newMode.width;
			windowHeight = newMode.height;

			if (hwndProc == nullptr) {
				hwndProc = DefWindowProcA;
			}

			DestroyOpenGLContext();

			// Try to transition to fullscreen. If we can't, fall back to windowed mode.
			if (fullscreen) {
				DEVMODE newScreenSettings{};
				newScreenSettings.dmSize = sizeof(DEVMODE);
				newScreenSettings.dmPelsWidth = windowWidth;
				newScreenSettings.dmPelsHeight = windowHeight;
				newScreenSettings.dmBitsPerPel = newMode.depth;
				newScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				if (ChangeDisplaySettings(&newScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
					MessageBoxA(NULL, "Failed to change display settings for fullscreen. Falling back to windowed mode.", "SCGL video mode error", MB_ICONWARNING);
					fullscreen = false;
				}
			}

			// Create the new window with desired width and height
			DWORD dwStyle, dwExtStyle;
			RECT wndRect{ 0, 0, windowWidth, windowHeight };

			if (fullscreen) {
				dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_MAXIMIZE;
				dwExtStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;
			}
			else {
				dwStyle = WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				dwExtStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

				AdjustWindowRectEx(&wndRect, dwStyle, FALSE, dwExtStyle);
				OffsetRect(&wndRect, 0, GetSystemMetrics(SM_CYCAPTION));
			}

			HWND hwnd = CreateWindowExA(
				dwExtStyle,
				"GDriverClass--OpenGL",
				"GDriverWindow--OpenGL",
				dwStyle,
				wndRect.left,
				wndRect.top,
				wndRect.right - wndRect.left,
				wndRect.bottom - wndRect.top,
				nullptr,
				nullptr,
				GetModuleHandle(nullptr),
				nullptr);

			if (hwnd == nullptr) {
				MessageBoxA(NULL, "Failed to create window.", "SCGL video mode error", MB_ICONWARNING);
				return;
			}

			HDC hdc = GetDC(hwnd);
			windowHandle = hwnd;
			deviceContext = hdc;

			// Try to set the new pixel format with the OpenGL extension, which lets us guarantee hardware
			// acceleration. If we don't have it, we're probably on ancient hardware but fall back anyway.
			PIXELFORMATDESCRIPTOR pfd{};
			int pixelFormat;

			if (supportedExtensions.pixelFormat) {
				int attribIList[] = {
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_ALPHA_BITS_ARB, newMode.depth == 32 ? 8 : 1,
					WGL_COLOR_BITS_ARB, newMode.depth == 32 ? 24 : 15,
					WGL_DEPTH_BITS_ARB, newMode.depth == 32 ? 24 : 16,
					WGL_STENCIL_BITS_ARB, 8,
					0, 0, // Reserved for WGL_SAMPLE_BUFFERS_ARB
					0, 0, // Reserved for WGL_SAMPLES_ARB
					0,
				};

				// TODO: should have some way of disabling MSAA
				if (supportedExtensions.multisample) {
					attribIList[18] = WGL_SAMPLE_BUFFERS_ARB;
					attribIList[19] = 1;
					attribIList[20] = WGL_SAMPLES_ARB;
					attribIList[21] = 4;
				}

				uint32_t numFormats;
				if (!wglChoosePixelFormatARB(hdc, attribIList, nullptr, 1, &pixelFormat, &numFormats) || numFormats == 0) {
					MessageBoxA(NULL, "No compatible video mode detected.", "SCGL video mode error", MB_ICONWARNING);
					DestroyOpenGLContext();
					return;
				}

				DescribePixelFormat(hdc, pixelFormat, sizeof(pfd), &pfd);
			}
			else {
				pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
				pfd.nVersion = 1;
				pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
				pfd.iPixelType = PFD_TYPE_RGBA;
				pfd.cAlphaBits = (newMode.depth == 32) ? 8 : 1;
				pfd.cColorBits = (newMode.depth == 32) ? 24 : 15;
				pfd.cDepthBits = (newMode.depth == 32) ? 24 : 16;
				pfd.cStencilBits = 8;
				pfd.iLayerType = PFD_MAIN_PLANE;

				pixelFormat = ChoosePixelFormat(hdc, &pfd);
			}

			if (!SetPixelFormat(hdc, pixelFormat, &pfd)) {
				MessageBoxA(NULL, "Failed to set video mode.", "SCGL video mode error", MB_ICONWARNING);
				DestroyOpenGLContext();
				return;
			}

			// Now try to create the OpenGL context itself
			if (supportedExtensions.createContext) {
				std::vector<int> contextAttribIList{
					WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
					WGL_CONTEXT_MINOR_VERSION_ARB, 0,
#ifndef NDEBUG
					WGL_CONTEXT_FLAGS_ARB, supportedExtensions.debugOutput ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
#endif
				};

#ifdef NDEBUG
				if (supportedExtensions.createContextNoError) {
					contextAttribIList.push_back(WGL_CONTEXT_OPENGL_NO_ERROR_ARB);
					contextAttribIList.push_back(GL_TRUE);
				}
#endif

				contextAttribIList.push_back(0);
				glContext = wglCreateContextAttribsARB(hdc, nullptr, contextAttribIList.data());
			}
			else {
				glContext = wglCreateContext(hdc);
			}

			if (glContext == nullptr) {
				MessageBoxA(NULL, "Failed to create an OpenGL context", "SCGL video mode error", MB_ICONWARNING);
				DestroyOpenGLContext();
				return;
			}

#ifndef NDEBUG
			{
				HWND secondaryHwnd = CreateWindowExA(
					dwExtStyle,
					"GDriverClass--OpenGLDebug",
					"GDriverWindow--OpenGLDebug",
					(dwStyle) & ~(WS_EX_APPWINDOW | WS_POPUP | WS_SYSMENU),
					wndRect.left,
					wndRect.top,
					wndRect.right - wndRect.left,
					wndRect.bottom - wndRect.top,
					nullptr,
					nullptr,
					GetModuleHandle(nullptr),
					nullptr);

				if (secondaryHwnd == nullptr) {
					DWORD error = GetLastError();
					MessageBoxA(NULL, "Failed to create window.", "SCGL video mode error", MB_ICONWARNING);
					return;
				}

				HDC secondaryDC = GetDC(secondaryHwnd);
				secondaryWindow = secondaryHwnd;
				secondaryDeviceContext = secondaryDC;

				pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
				pfd.nVersion = 1;
				pfd.dwFlags = PFD_DRAW_TO_WINDOW;
				pfd.iPixelType = PFD_TYPE_RGBA;
				pfd.cAlphaBits = (newMode.depth == 32) ? 8 : 1;
				pfd.cColorBits = (newMode.depth == 32) ? 24 : 15;
				pfd.cDepthBits = (newMode.depth == 32) ? 24 : 16;
				pfd.cStencilBits = 8;
				pfd.iLayerType = PFD_MAIN_PLANE;

				pixelFormat = ChoosePixelFormat(secondaryDC, &pfd);

				if (!SetPixelFormat(secondaryDC, pixelFormat, &pfd)) {
					MessageBoxA(NULL, "Failed to set video mode on debug window.", "SCGL video mode error", MB_ICONWARNING);
				}

				ShowWindow(secondaryHwnd, SW_SHOWNORMAL);
				pixels = new char[3 * newMode.width * newMode.height];
				lineBrush = CreateSolidBrush(RGB(255, 0, 0));
			}
#endif

			wglMakeCurrent(hdc, static_cast<HGLRC>(glContext));

			if (supportedExtensions.swapControl) {
				wglSwapIntervalEXT(1);
			}

			// Window should now be ready to go, show it and wrap up initialization
			SetWindowLong(hwnd, GWL_WNDPROC, reinterpret_cast<LONG>(hwndProc));
			ShowWindow(hwnd, SW_SHOWNORMAL);

#ifndef NDEBUG
			if (supportedExtensions.debugOutput && glDebugMessageCallback != nullptr) {
				glDebugMessageCallback(LogGLMessage, nullptr);
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			}
#endif

			glEnableClientState(GL_VERTEX_ARRAY);

			// SimCity 4 doesn't use the GZDriverLightingExtension, but it does expect
			// there to be a light enabled and to 45deg above x/y.
			GLfloat lightPos[] = { 1.0, 1.0, 0.0, 0.0 };
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

			GLfloat ambientLightParams[] = { 1.0f, 0.0f, 0.0f, 1.0f };
			glMaterialfv(GL_FRONT, GL_AMBIENT, ambientLightParams);

			GLfloat diffuseLightParams[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseLightParams);

			SetViewport();
			SetLastError(DriverError::OK);
		}
		else {
			SetLastError(DriverError::OUT_OF_RANGE);
		}
	}

	bool cGDriver::IsDeviceReady(void) {
		return glContext != nullptr && supportedExtensions.bgraColor;
	}

	void cGDriver::Flush(void) {
		SwapBuffers(static_cast<HDC>(deviceContext));
	}

	void cGDriver::SetViewport(void) {
		glViewport(0, 0, windowWidth, windowHeight);
		glDisable(GL_SCISSOR_TEST);

		viewportX = viewportY = 0;
		viewportWidth = windowWidth;
		viewportHeight = windowHeight;
	}

	void cGDriver::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
		glViewport(x, y, width, height);
		glScissor(x, y, width, height);
		glEnable(GL_SCISSOR_TEST);

		viewportX = x;
		viewportY = y;
		viewportWidth = width;
		viewportHeight = height;
	}

	void cGDriver::GetViewport(int32_t dimensions[4]) {
		dimensions[0] = viewportX;
		dimensions[1] = viewportY;
		dimensions[2] = viewportX + viewportWidth;
		dimensions[3] = viewportY + viewportHeight;
	}
}