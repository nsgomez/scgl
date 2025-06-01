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

#include <cIGZCOM.h>
#include <cRZCOMDllDirector.h>
#include <cRZSysServPtr.h>
#include "../cGDriver.h"

extern cRZCOMSlimDllDirector* RZGetCOMDllDirector();

static const uint32_t GZCLSID_cGZBuffer = 0xC470D325;
static const uint32_t GZIID_cIGZGraphicSystem = 0x73283c;
static const uint32_t RZSRVID_GraphicSystem = 0xc416025c;

class cIGZGraphicSystem : public cIGZUnknown
{
public:
	virtual bool CreateBuffer(cIGZBuffer** ppvObj) = 0;
	// Don't need to declare the rest of the interface right now
};

namespace nSCGL
{
	static inline cIGZBuffer* CreateBufferFromGraphicsSystem()
	{
		cRZSysServPtr<cIGZGraphicSystem, GZIID_cIGZGraphicSystem, RZSRVID_GraphicSystem> pGraphicsSystem;
		if ((cIGZGraphicSystem*)pGraphicsSystem == nullptr) {
			return nullptr;
		}

		cIGZBuffer* newBuffer = nullptr;
		if (!pGraphicsSystem->CreateBuffer(&newBuffer)) {
			return nullptr;
		}

		return newBuffer;
	}

	cIGZBuffer* cGDriver::CopyColorBuffer(int32_t x, int32_t y, int32_t width, int32_t height, cIGZBuffer* buffer) {
		int32_t startX = x;
		int32_t startY = y;
		int32_t endX = x + width;
		int32_t endY = y + height;

		if (startX < 0) { x = 0; }
		if (startY < 0) { y = 0; }
		if (endX > viewportWidth) { endX = viewportWidth; }
		if (endY > viewportHeight) { endY = viewportHeight; }

		uint8_t* colorBytes = nullptr;
		x = startX;
		y = startY;
		width = endX - startX;
		height = endY - startY;

		if (buffer == nullptr || !buffer->IsReady()) {
			buffer = CreateBufferFromGraphicsSystem();
			if (buffer == nullptr) {
				return nullptr;
			}
		}
		else if (buffer->GetColorType() != cGZBufferColorType::A8R8G8B8) {
			return nullptr;
		}

		colorBytes = new uint8_t[3 * width * height];
		if (colorBytes == nullptr) {
			return buffer;
		}

		glReadBuffer(GL_BACK);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(x, viewportHeight - startY - height, width, height, GL_RGB, GL_UNSIGNED_BYTE, colorBytes);

		uint8_t const* colorBytesCursor = colorBytes;
		if ((buffer->IsReady() || buffer->Init(width, height, cGZBufferColorType::A8R8G8B8, 32)) && buffer->Lock(cIGZBuffer::eLockFlags::IsDirtyUpdate)) {
			while (--height >= 0) {
				for (int i = 0; i < width; i++) {
					uint32_t color = 0xFF000000 | (colorBytesCursor[0] << 16) | (colorBytesCursor[1] << 8) | colorBytesCursor[2];
					buffer->SetPixel(i, height, color);

					colorBytesCursor += 3;
				}
			}

			buffer->Unlock(cIGZBuffer::eLockFlags::IsDirtyUpdate);
		}

#ifndef NDEBUG
		glPixelStorei(GL_PACK_ALIGNMENT, 4);
#endif

		delete[] colorBytes;
		return buffer;
	}
}