#include <string>
#include "cGDriver.h"
#include "GLSupport.h"

namespace nSCGL
{
	bool cGDriver::Init(void) {
		// Create an invisible default window to set up an initial OpenGL context
		WNDCLASS wc{};
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = DefWindowProcA;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = "GDriverClass--OpenGL";

		UnregisterClass(wc.lpszClassName, nullptr);

		if (!RegisterClass(&wc)) {
			MessageBoxA(NULL, "Failed to set up an OpenGL window class", "SCGL failed to start", MB_ICONERROR);
			return false;
		}

		windowHandle = CreateWindowA(wc.lpszClassName, "GDriverWindow--OpenGL--FalseContext", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, nullptr, nullptr, wc.hInstance, 0);
		if (windowHandle == nullptr) {
			MessageBoxA(NULL, "Failed to create an OpenGL window", "SCGL failed to start", MB_ICONERROR);
			return false;
		}

		deviceContext = GetDC(static_cast<HWND>(windowHandle));

		PIXELFORMATDESCRIPTOR defaultPfd{};
		defaultPfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		defaultPfd.nVersion = 1;
		defaultPfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		defaultPfd.iPixelType = PFD_TYPE_RGBA;
		defaultPfd.cColorBits = 16;
		defaultPfd.cDepthBits = 16;
		defaultPfd.cStencilBits = 8;
		defaultPfd.iLayerType = PFD_MAIN_PLANE;

		int pixelFormat = ChoosePixelFormat(static_cast<HDC>(deviceContext), &defaultPfd);
		if (!SetPixelFormat(static_cast<HDC>(deviceContext), pixelFormat, &defaultPfd)) {
			MessageBoxA(NULL, "Failed to set pixel format for window", "SCGL failed to start", MB_ICONERROR);
			return false;
		}

		glContext = wglCreateContext(static_cast<HDC>(deviceContext));
		if (glContext == nullptr || !wglMakeCurrent(static_cast<HDC>(deviceContext), static_cast<HGLRC>(glContext))) {
			MessageBoxA(NULL, "Failed to create an OpenGL context", "SCGL failed to start", MB_ICONERROR);
			return false;
		}

		// Load function pointers and check extensions now that we have a false context
		InitGLSupport();

		int glExtensionCount;
		glGetIntegerv(GL_NUM_EXTENSIONS, &glExtensionCount);

		for (int i = 0; i < glExtensionCount; i++) {
			const char* extensionName = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));

			if (strcmp(extensionName, "GL_ARB_multitexture") == 0) supportedExtensions.multitexture = true;
			if (strcmp(extensionName, "GL_ARB_texture_env_combine") == 0) supportedExtensions.textureEnvCombine = true;
			if (strcmp(extensionName, "GL_EXT_fog_coord") == 0) supportedExtensions.fogCoord = true;
			if (strcmp(extensionName, "GL_EXT_texture_compression_s3tc") == 0) supportedExtensions.textureCompression = true;
			if (strcmp(extensionName, "GL_NV_texture_env_combine4") == 0) supportedExtensions.nvTextureEnvCombine4 = true;
			if (strcmp(extensionName, "GL_KHR_debug") == 0) supportedExtensions.debugOutput = true;
			if (strcmp(extensionName, "GL_KHR_no_error") == 0) supportedExtensions.noError = true;
		}

		if (wglGetExtensionsStringARB != nullptr) {
			char* wglExtensions = _strdup(wglGetExtensionsStringARB(static_cast<HDC>(deviceContext)));
			char* token = strtok(wglExtensions, " ");

			while (token != nullptr) {
				if (strcmp(token, "WGL_ARB_buffer_region") == 0) supportedExtensions.bufferRegion = true;
				if (strcmp(token, "WGL_ARB_create_context") == 0) supportedExtensions.createContext = true;
				if (strcmp(token, "WGL_ARB_create_context_no_error") == 0) supportedExtensions.createContextNoError = true;
				if (strcmp(token, "WGL_ARB_create_context_profile") == 0) supportedExtensions.createContextProfile = true;
				if (strcmp(token, "WGL_ARB_multisample") == 0) supportedExtensions.multisample = true;
				if (strcmp(token, "WGL_ARB_pixel_format") == 0) supportedExtensions.pixelFormat = true;
				if (strcmp(token, "WGL_EXT_swap_control") == 0) supportedExtensions.swapControl = true;

				token = strtok(nullptr, " ");
			}
		}

		// Get device info while the false context is still up
		glGetIntegerv(GL_MAX_TEXTURE_UNITS, reinterpret_cast<GLint*>(&maxTextureUnits));

		char const* vendor = reinterpret_cast<char const*>(glGetString(GL_VENDOR));
		char const* renderer = reinterpret_cast<char const*>(glGetString(GL_RENDERER));
		char const* version = reinterpret_cast<char const*>(glGetString(GL_VERSION));

		static const char unknownDriverName[] = "UnknownDriverName";
		static const char unknownCardVersion[] = "UnknownCardVersion";

		driverInfo.append(unknownDriverName, sizeof(unknownDriverName) - 1);
		driverInfo.append("\n", 1);
		driverInfo.append(version, strlen(version));
		driverInfo.append("\n", 1);
		driverInfo.append(renderer, strlen(renderer));
		driverInfo.append("\n", 1);
		driverInfo.append(unknownCardVersion, sizeof(unknownCardVersion) - 1);
		driverInfo.append("\n", 1);
		driverInfo.append(renderer, strlen(renderer));
		driverInfo.append("\n", 1);

		InitializeVideoModeVector();

		// Tear down the false context. We'll create a new, better one in SetVideoMode.
		DestroyOpenGLContext();
		return true;
	}

	bool cGDriver::Shutdown(void) {
		DestroyOpenGLContext();
		UnregisterClass("GDriverClass--OpenGL", nullptr);
		return true;
	}

	void cGDriver::DestroyOpenGLContext(void) {
		if (glContext != nullptr) {
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(static_cast<HGLRC>(glContext));

			glContext = nullptr;
		}

		if (windowHandle != nullptr) {
			DestroyWindow(static_cast<HWND>(windowHandle));

			windowHandle = nullptr;
			deviceContext = nullptr;
		}
	}

	int32_t cGDriver::InitializeVideoModeVector(void) {
		if (videoModeCount != 0) {
			videoModes.clear();
			videoModeCount = 0;
		}

		DEVMODE displayMode{};
		DWORD i = 0;

		while (EnumDisplaySettingsA(nullptr, i++, &displayMode)) {
			int depth = displayMode.dmBitsPerPel;
			if (depth < 15) {
				continue;
			}

			bool isDuplicate = false;
			for (sGDMode const& it : videoModes) {
				if (it.width == displayMode.dmPelsWidth && it.height == displayMode.dmPelsHeight && it.depth == depth) {
					isDuplicate = true;
					break;
				}
			}

			if (isDuplicate) {
				continue;
			}

			sGDMode tempMode{};
			tempMode.textureStageCount = maxTextureUnits;

			// If not set, SC4 throws the "Could not initialize the hardware driver" error and switches to software mode.
			tempMode.isInitialized = true;

			// These capabilities are always present in OpenGL 3.0
			tempMode.supportsStencilBuffer = true;
			tempMode.supportsMultitexture = supportedExtensions.multitexture;
			tempMode.supportsTextureEnvCombine = supportedExtensions.textureEnvCombine;
			tempMode.supportsFogCoord = supportedExtensions.fogCoord;
			tempMode.supportsDxtTextures = supportedExtensions.textureCompression;
			tempMode.supportsNvTextureEnvCombine4 = supportedExtensions.nvTextureEnvCombine4;

			// TODO: what are these flags for and why does the game's OpenGL driver set them?
			tempMode.__unknown2 = true;
			tempMode.__unknown5[0] = false;
			tempMode.__unknown5[1] = false;
			tempMode.__unknown5[2] = false;

			if (depth > 16) {
				tempMode.alphaColorMask = 0xff000000;
				tempMode.redColorMask = 0x00ff0000;
				tempMode.greenColorMask = 0x0000ff00;
				tempMode.blueColorMask = 0x000000ff;
			}
			else {
				tempMode.alphaColorMask = 0x1;
				tempMode.redColorMask = 0xf800;
				tempMode.greenColorMask = 0x7c0;
				tempMode.blueColorMask = 0x3e;
			}

			tempMode.index = videoModeCount++;
			tempMode.width = displayMode.dmPelsWidth;
			tempMode.height = displayMode.dmPelsHeight;
			tempMode.depth = depth;

			tempMode.isFullscreen = true;

			videoModes.push_back(tempMode);

			tempMode.index = videoModeCount++;
			tempMode.isFullscreen = false;

			videoModes.push_back(tempMode);
		}

		return videoModeCount;
	}
}