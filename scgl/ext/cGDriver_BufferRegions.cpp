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

	// Note: officially, the parameter order is (region, srcX0, srcY0, width, height, dstX0, dstY0)
	// But SimGL seems to expect a DirectX coordinate system where the origin is at the top left at the window,
	// but OpenGL puts the origin at the *bottom* left, so we need to invert the Y axis.
	bool cGDriver::ReadBufferRegion(uint32_t region, GLint srcX0, GLint dstY0, GLsizei width, GLsizei height, int32_t dstX0, int32_t srcY0) {
		uint32_t bufferRegionIndex = region - 1;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferHandles[bufferRegionIndex]);

		GLint srcX1 = srcX0 + width;
		GLint srcY1 = srcY0 + height;

		GLint dstX1 = dstX0 + width;
		GLint dstY1 = dstY0 + height;

		glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, framebufferMasks[bufferRegionIndex], GL_NEAREST);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		return true;
	}

	bool cGDriver::DrawBufferRegion(uint32_t region, GLint srcX0, GLint dstY0, GLsizei width, GLsizei height, GLint dstX0, GLint srcY0) {
		uint32_t bufferRegionIndex = region - 1;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferHandles[bufferRegionIndex]);

		GLint srcX1 = srcX0 + width;
		GLint srcY1 = srcY0 + height;

		GLint dstX1 = dstX0 + width;
		GLint dstY1 = dstY0 + height;

		glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, framebufferMasks[bufferRegionIndex], GL_NEAREST);
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