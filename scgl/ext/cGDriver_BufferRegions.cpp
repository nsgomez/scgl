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
		return wglCreateBufferRegionARB != nullptr;
	}

	uint32_t cGDriver::NewBufferRegion(int32_t gdBufferRegionType) {
		static GLenum regionTypeMap[] = { WGL_BACK_COLOR_BUFFER_BIT_ARB, WGL_DEPTH_BUFFER_BIT_ARB, WGL_STENCIL_BUFFER_BIT_ARB };
		SIZE_CHECK_RETVAL(gdBufferRegionType, regionTypeMap, 0);

		if (wglCreateBufferRegionARB == nullptr) {
			return 0;
		}

		uint32_t bufferRegionIndex = FindFreeBufferRegionIndex();
		if (bufferRegionIndex == -1) {
			return 0;
		}

		HANDLE result = wglCreateBufferRegionARB(reinterpret_cast<HDC>(deviceContext), 0, regionTypeMap[gdBufferRegionType]);
		if (result == nullptr) {
			return 0;
		}

		bufferRegionFlags |= 1 << bufferRegionIndex;
		bufferRegionHandles[bufferRegionIndex] = result;

		return bufferRegionIndex + 1;
	}

	bool cGDriver::DeleteBufferRegion(int32_t region) {
		uint32_t bufferRegionIndex = region - 1;

		wglDeleteBufferRegionARB(bufferRegionHandles[bufferRegionIndex]);
		bufferRegionHandles[bufferRegionIndex] = nullptr;
		bufferRegionFlags &= ~(1 << bufferRegionIndex);

		return true;
	}

	bool cGDriver::ReadBufferRegion(uint32_t region, GLint x, GLint y, GLsizei width, GLsizei height, int32_t unused0, int32_t unused1) {
		return wglSaveBufferRegionARB(bufferRegionHandles[region - 1], x, y, width, height);
	}

	bool cGDriver::DrawBufferRegion(uint32_t region, GLint x, GLint y, GLsizei width, GLsizei height, GLint xDest, GLint yDest) {
		return wglRestoreBufferRegionARB(bufferRegionHandles[region - 1], xDest, y, width, height, x, yDest);
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