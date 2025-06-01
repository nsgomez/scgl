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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include <cassert>
#include "../cGDriver.h"
#include "../GLSupport.h"

namespace nSCGL
{
	int cGDriver::FindFreeBufferRegionIndex(void) {
		if (bufferRegionFlags == UINT8_MAX) {
			// All bits are set, no room left for new regions.
			return -1;
		}

		for (uint32_t i = 0; i < sizeof(uint32_t) * 8; i++) {
			if ((bufferRegionFlags & (1 << i)) == 0) {
				return i;
			}
		}

		// We shouldn't be here since we already checked if there was a free index.
		assert(false);
		return -1;
	}

	bool cGDriver::BufferRegionEnabled(void) {
		return supportedExtensions.bufferRegion && wglCreateBufferRegionARB != nullptr;
	}

	uint32_t cGDriver::NewBufferRegion(int32_t gdBufferRegionType) {
		static GLenum regionTypeMap[] = { WGL_BACK_COLOR_BUFFER_BIT_ARB, WGL_DEPTH_BUFFER_BIT_ARB };
		SIZE_CHECK_RETVAL(gdBufferRegionType, regionTypeMap, 0);

		uint32_t bufferRegionIndex = FindFreeBufferRegionIndex();
		if (bufferRegionIndex == -1) {
			return 0;
		}

		GLuint framebuffer;
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		GLuint renderbuffer;
		glGenRenderbuffers(1, &renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);

		GLuint renderbufferStorageType, attachmentType;
		GLuint framebufferMask;

		if (regionTypeMap[gdBufferRegionType] == WGL_BACK_COLOR_BUFFER_BIT_ARB) {
			renderbufferStorageType = (videoModes[currentVideoMode].depth == 32) ? GL_RGBA8 : GL_RGB5_A1;
			attachmentType = GL_COLOR_ATTACHMENT0;
			framebufferMask = GL_COLOR_BUFFER_BIT;
		}
		else {
			renderbufferStorageType = (videoModes[currentVideoMode].depth == 32) ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT16;
			attachmentType = GL_DEPTH_ATTACHMENT;
			framebufferMask = GL_DEPTH_BUFFER_BIT;
		}

		glRenderbufferStorage(GL_RENDERBUFFER, renderbufferStorageType, windowWidth, windowHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, renderbuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			return 0;
		}

		bufferRegionFlags |= 1 << bufferRegionIndex;
		framebufferHandles[bufferRegionIndex] = framebuffer;
		renderbufferHandles[bufferRegionIndex] = renderbuffer;
		framebufferMasks[bufferRegionIndex] = framebufferMask;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return bufferRegionIndex + 1;
	}

	bool cGDriver::DeleteBufferRegion(int32_t region) {
		uint32_t bufferRegionIndex = region - 1;
		if ((bufferRegionFlags & (1 << bufferRegionIndex)) == 0) {
			return true;
		}

		glDeleteRenderbuffers(1, &renderbufferHandles[bufferRegionIndex]);
		glDeleteFramebuffers(1, &framebufferHandles[bufferRegionIndex]);

		bufferRegionFlags &= ~(1 << bufferRegionIndex);
		return true;
	}

	bool cGDriver::ReadBufferRegion(uint32_t region, GLint dstX, GLint dstY, GLsizei width, GLsizei height, int32_t srcX, int32_t srcY) {
		uint32_t bufferRegionIndex = region - 1;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferHandles[bufferRegionIndex]);

		RECT glSrc, glDst;
		glSrc.left = srcX;
		glSrc.right = srcX + width;

		glDst.left = dstX;
		glDst.right = dstX + width;

		// SimGL expects a DirectX coordinate system where origin is at the top left, but OpenGL puts origin at *bottom* left.
		// The input coordinates need to be corrected by inverting the Y axis.
		glSrc.top = windowHeight - srcY;
		glDst.top = windowHeight - dstY;

		// Similarly, we need to *subtract* `height` from the y-coords to build the `src` and `dst` rectangles.
		glSrc.bottom = glSrc.top - height;
		glDst.bottom = glDst.top - height;

		glBlitFramebuffer(glSrc.left, glSrc.top, glSrc.right, glSrc.bottom, glDst.left, glDst.top, glDst.right, glDst.bottom, framebufferMasks[bufferRegionIndex], GL_NEAREST);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

#ifndef NDEBUG
		if (framebufferMasks[bufferRegionIndex] == GL_COLOR_BUFFER_BIT)
		{
			glFinish();

			memset(pixels, 127, width * height * 3);
			glReadPixels(srcX, glSrc.bottom, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

			InvalidateRect((HWND)secondaryWindow, nullptr, TRUE);

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint((HWND)secondaryWindow, &ps);

			BITMAPINFO bm;
			bm.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bm.bmiHeader.biWidth = width;
			bm.bmiHeader.biHeight = height;
			bm.bmiHeader.biPlanes = 1;
			bm.bmiHeader.biBitCount = 24;
			bm.bmiHeader.biCompression = BI_RGB;
			bm.bmiHeader.biSizeImage = 0;
			bm.bmiHeader.biXPelsPerMeter = 2835;
			bm.bmiHeader.biYPelsPerMeter = 2835;
			bm.bmiHeader.biClrUsed = 0;
			bm.bmiHeader.biClrImportant = 0;
			bm.bmiColors->rgbBlue = 255;
			bm.bmiColors->rgbGreen = 255;
			bm.bmiColors->rgbRed = 255;
			bm.bmiColors->rgbReserved = 0;

			::SetDIBitsToDevice(
				hdc,
				dstX,
				dstY,
				width,
				height,
				0,
				0,
				0,
				height,
				pixels,
				&bm,
				DIB_RGB_COLORS);

			RECT rect;
			rect.left = dstX;
			rect.right = rect.left + width;
			rect.top = dstY;
			rect.bottom = rect.top + height;

			FrameRect(hdc, &rect, (HBRUSH)lineBrush);
			EndPaint((HWND)secondaryWindow, &ps);
		}
#endif

		return true;
	}

	bool cGDriver::DrawBufferRegion(uint32_t region, GLint srcX, GLint srcY, GLsizei width, GLsizei height, GLint dstX, GLint dstY) {
		uint32_t bufferRegionIndex = region - 1;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferHandles[bufferRegionIndex]);

		RECT glSrc;
		glSrc.left = srcX;
		glSrc.right = srcX + width;
		glSrc.bottom = windowHeight - srcY;
		glSrc.top = glSrc.bottom - height;

		RECT glDst;
		glDst.left = dstX;
		glDst.right = dstX + width;
		glDst.bottom = windowHeight - dstY;
		glDst.top = glDst.bottom - height;

		glBlitFramebuffer(glSrc.left, glSrc.bottom, glSrc.right, glSrc.top, glDst.left, glDst.bottom, glDst.right, glDst.top, framebufferMasks[bufferRegionIndex], GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		return true;
	}

	bool cGDriver::IsBufferRegion(uint32_t region) {
		return (bufferRegionFlags & (1 << (region - 1))) != 0;
	}

	bool cGDriver::CanDoPartialRegionWrites(void) {
		return true;
	}

	bool cGDriver::CanDoOffsetReads(void) {
		return true;
	}

	bool cGDriver::DeleteAllBufferRegions(void) {
		for (size_t i = 0; i < MAX_BUFFER_REGIONS; i++) {
			if (IsBufferRegion(i)) {
				DeleteBufferRegion(i);
			}
		}

		bufferRegionFlags = 0;
		return true;
	}
}