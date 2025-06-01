/*
 *  SCGL - a free OpenGL driver for SimCity 4's SimGL interface
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include "GLSupport.h"

PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D = nullptr;
PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture = nullptr;
PFNGLGETSTRINGIPROC glGetStringi = nullptr;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = nullptr;

PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = nullptr;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = nullptr;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = nullptr;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = nullptr;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = nullptr;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = nullptr;

PFNWGLCREATEBUFFERREGIONARBPROC wglCreateBufferRegionARB = nullptr;
PFNWGLDELETEBUFFERREGIONARBPROC wglDeleteBufferRegionARB = nullptr;
PFNWGLSAVEBUFFERREGIONARBPROC wglSaveBufferRegionARB = nullptr;
PFNWGLRESTOREBUFFERREGIONARBPROC wglRestoreBufferRegionARB = nullptr;

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;

PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = nullptr;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;

PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = nullptr;

#define TRY_LOAD(type, name) name = reinterpret_cast<type>(wglGetProcAddress(#name))

void InitGLSupport(void) {
	TRY_LOAD(PFNGLCOMPRESSEDTEXIMAGE2DPROC, glCompressedTexImage2D);
	TRY_LOAD(PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC, glCompressedTexSubImage2D);
	TRY_LOAD(PFNGLACTIVETEXTUREPROC, glActiveTexture);
	TRY_LOAD(PFNGLCLIENTACTIVETEXTUREPROC, glClientActiveTexture);
	TRY_LOAD(PFNGLGETSTRINGIPROC, glGetStringi);
	TRY_LOAD(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback);

	TRY_LOAD(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer);
	TRY_LOAD(PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers);
	TRY_LOAD(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
	TRY_LOAD(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage);
	TRY_LOAD(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
	TRY_LOAD(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
	TRY_LOAD(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
	TRY_LOAD(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
	TRY_LOAD(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer);
	TRY_LOAD(PFNGLBLITFRAMEBUFFERPROC, glBlitFramebuffer);

	TRY_LOAD(PFNWGLCREATEBUFFERREGIONARBPROC, wglCreateBufferRegionARB);
	TRY_LOAD(PFNWGLDELETEBUFFERREGIONARBPROC, wglDeleteBufferRegionARB);
	TRY_LOAD(PFNWGLSAVEBUFFERREGIONARBPROC, wglSaveBufferRegionARB);
	TRY_LOAD(PFNWGLRESTOREBUFFERREGIONARBPROC, wglRestoreBufferRegionARB);

	TRY_LOAD(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
	TRY_LOAD(PFNWGLGETEXTENSIONSSTRINGARBPROC, wglGetExtensionsStringARB);

	TRY_LOAD(PFNWGLGETPIXELFORMATATTRIBIVARBPROC, wglGetPixelFormatAttribivARB);
	TRY_LOAD(PFNWGLGETPIXELFORMATATTRIBFVARBPROC, wglGetPixelFormatAttribfvARB);
	TRY_LOAD(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB);

	TRY_LOAD(PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT);
	TRY_LOAD(PFNWGLGETSWAPINTERVALEXTPROC, wglGetSwapIntervalEXT);
}