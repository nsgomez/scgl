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

#include "../cGDriver.h"

namespace nSCGL
{
	char const* cGDriver::GetVertexBufferName(uint32_t gdVertexFormat) {
		NOTIMPL();
		return nullptr;
	}

	uint32_t cGDriver::VertexBufferType(uint32_t) {
		NOTIMPL();
		return 0;
	}

	uint32_t cGDriver::MaxVertices(uint32_t) {
		NOTIMPL();
		return 0;
	}

	uint32_t cGDriver::GetVertices(int32_t, bool) {
		NOTIMPL();
		return 0;
	}

	uint32_t cGDriver::ContinueVertices(uint32_t, uint32_t) {
		NOTIMPL();
		return 0;
	}

	void cGDriver::ReleaseVertices(uint32_t) {
		NOTIMPL();
	}

	void cGDriver::DrawPrims(uint32_t, uint32_t gdPrimType, void*, uint32_t) {
		NOTIMPL();
	}

	void cGDriver::DrawPrimsIndexed(uint32_t, uint32_t gdPrimType, uint32_t, uint16_t*, void*, uint32_t) {
		NOTIMPL();
	}

	void cGDriver::Reset(void) {
		NOTIMPL();
	}
}