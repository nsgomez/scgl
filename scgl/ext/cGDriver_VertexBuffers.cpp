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

//
// Implementation notes (issue #10 - "Implement GDriverVertexBufferExtension")
// -----------------------------------------------------------------------------
// cIGZGDriverVertexBufferExtension isn't documented anywhere in the SDK headers
// SCGL is built against, and the only reference implementation is the closed
// PrimitiveManager in SimCity 4's DirectX driver, which we don't have source
// for. The parameter names in the interface (see cIGZGDriverVertexBufferExtension.h)
// are all placeholders, so the exact calling contract below is *inferred* from
// the method names/shapes, not confirmed against the game. In particular:
//
//   - GetVertexBufferName / VertexBufferType / MaxVertices all take a
//     gdVertexFormat, so they're treated here as capability queries the game
//     can make ahead of time for a given vertex layout.
//   - GetVertices / ContinueVertices / ReleaseVertices don't take a vertex
//     format, so they're treated as a reservation protocol against whatever
//     vertex layout is "current" (the format last passed to InterleavedArrays),
//     returning an opaque, stable slot handle - *not* a raw mapped pointer.
//   - DrawPrims / DrawPrimsIndexed both take a raw CPU-side vertex pointer
//     (and, for the indexed variant, a raw index pointer), so actual upload to
//     the GPU buffer happens at draw time via glBufferSubDataARB, using the
//     stride/format recorded for that slot back when it was reserved.
//
// This keeps every "handle" as a small integer slot index into a fixed pool
// (mirroring the buffer-region pool a few files over) rather than trusting an
// externally-supplied integer as a pointer, which would be unsafe to guess at.
//
// This should be treated as a solid starting point, not a verified-correct
// implementation: it hasn't been (and can't be, in this environment) exercised
// against the real game. Please validate the calling contract - e.g. by
// tracing real DrawPrims/DrawPrimsIndexed calls, or by comparing against the
// original D3D PrimitiveManager once issue #8 lands - before relying on it.
//

#include "../cGDriver.h"
#include "../GLSupport.h"
#include "../VertexFormatUtils.h"

extern GLenum drawModeMap[8];

namespace nSCGL
{
	// A 16-bit index buffer (see DrawPrimsIndexed) can only address this many distinct vertices.
	constexpr uint32_t kMaxVerticesPerSlot = 65536;

	int cGDriver::FindFreeVertexBufferSlot(void) {
		for (size_t i = 0; i < MAX_VERTEX_BUFFER_SLOTS; i++) {
			if (!vertexBufferSlots[i].inUse) {
				return static_cast<int>(i);
			}
		}

		return -1;
	}

	char const* cGDriver::GetVertexBufferName(uint32_t gdVertexFormat) {
		switch (gdVertexFormat) {
		case kGDVertexFormat_V3F_C4UB:            return "V3F_C4UB Vertex Buffer";
		case kGDVertexFormat_V3F_T2F:              return "V3F_T2F Vertex Buffer";
		case kGDVertexFormat_V3F_2T2F:             return "V3F_2T2F Vertex Buffer";
		case kGDVertexFormat_V3F_C4UB_T2F:         return "V3F_C4UB_T2F Vertex Buffer";
		case kGDVertexFormat_V3F_C4UB_2T2F:        return "V3F_C4UB_2T2F Vertex Buffer";
		case kGDVertexFormat_V3F:                  return "V3F Vertex Buffer";
		case kGDVertexFormat_V3F_N3F:              return "V3F_N3F Vertex Buffer";
		case kGDVertexFormat_V3F_N3F_C4UB:         return "V3F_N3F_C4UB Vertex Buffer";
		case kGDVertexFormat_V3F_N3F_T2F:          return "V3F_N3F_T2F Vertex Buffer";
		case kGDVertexFormat_V3F_N3F_2T2F:         return "V3F_N3F_2T2F Vertex Buffer";
		case kGDVertexFormat_V3F_N3F_C4UB_T2F:     return "V3F_N3F_C4UB_T2F Vertex Buffer";
		case kGDVertexFormat_V3F_N3F_C4UB_2T2F:    return "V3F_N3F_C4UB_2T2F Vertex Buffer";
		default:                                   return "Unknown Vertex Buffer";
		}
	}

	uint32_t cGDriver::VertexBufferType(uint32_t gdVertexFormat) {
		// The "type" is just the packed vertex format; it's what we need later
		// to recompute stride and set up client array pointers for a slot.
		if (gdVertexFormat < 0x80000000) {
			gdVertexFormat = RZMakeVertexFormat(gdVertexFormat);
		}

		return gdVertexFormat;
	}

	uint32_t cGDriver::MaxVertices(uint32_t gdVertexFormat) {
		if (!supportedExtensions.vertexBufferObject) {
			return 0;
		}

		uint32_t stride = RZVertexFormatStride(gdVertexFormat);
		if (stride == 0) {
			return 0;
		}

		return kMaxVerticesPerSlot;
	}

	uint32_t cGDriver::GetVertices(int32_t count, bool dynamic) {
		if (!supportedExtensions.vertexBufferObject || count <= 0 || static_cast<uint32_t>(count) > kMaxVerticesPerSlot) {
			return 0;
		}

		int slotIndex = FindFreeVertexBufferSlot();
		if (slotIndex < 0) {
			return 0;
		}

		GLVertexBufferSlot& slot = vertexBufferSlots[slotIndex];
		uint32_t vertexFormat = state.shareable.interleavedFormat;
		uint32_t stride = RZVertexFormatStride(vertexFormat);
		if (stride == 0) {
			return 0;
		}

		GLuint vbo;
		glGenBuffersARB(1, &vbo);
		glBindBufferARB(GL_ARRAY_BUFFER, vbo);
		glBufferDataARB(GL_ARRAY_BUFFER, static_cast<ptrdiff_t>(stride) * count, nullptr, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		glBindBufferARB(GL_ARRAY_BUFFER, 0);

		slot.inUse = true;
		slot.dynamic = dynamic;
		slot.vertexFormat = vertexFormat;
		slot.stride = stride;
		slot.reservedVertexCount = static_cast<uint32_t>(count);
		slot.glVertexBuffer = vbo;
		slot.glIndexBuffer = 0;

		return static_cast<uint32_t>(slotIndex) + 1;
	}

	uint32_t cGDriver::ContinueVertices(uint32_t handle, uint32_t additionalCount) {
		if (handle == 0 || handle > MAX_VERTEX_BUFFER_SLOTS) {
			return 0;
		}

		GLVertexBufferSlot& slot = vertexBufferSlots[handle - 1];
		if (!slot.inUse) {
			return 0;
		}

		uint32_t newCount = slot.reservedVertexCount + additionalCount;
		if (newCount > kMaxVerticesPerSlot) {
			return 0;
		}

		// Grow the backing store in place. Since nothing has been uploaded to this
		// slot's buffer yet (upload only happens at draw time), simply re-issuing
		// glBufferDataARB with a bigger size and no data is enough - there's no
		// existing GPU-side content to preserve.
		glBindBufferARB(GL_ARRAY_BUFFER, slot.glVertexBuffer);
		glBufferDataARB(GL_ARRAY_BUFFER, static_cast<ptrdiff_t>(slot.stride) * newCount, nullptr, slot.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		glBindBufferARB(GL_ARRAY_BUFFER, 0);

		slot.reservedVertexCount = newCount;
		return handle;
	}

	void cGDriver::ReleaseVertices(uint32_t handle) {
		// Nothing to unmap - actual data upload is deferred to DrawPrims/DrawPrimsIndexed,
		// so releasing is purely advisory bookkeeping for now.
		(void)handle;
	}

	bool cGDriver::BindVertexBufferSlot(GLVertexBufferSlot& slot, void const* vertexData, uint32_t vertexCount) {
		if (!slot.inUse || vertexData == nullptr) {
			return false;
		}

		uint32_t uploadCount = (vertexCount < slot.reservedVertexCount) ? vertexCount : slot.reservedVertexCount;
		glBindBufferARB(GL_ARRAY_BUFFER, slot.glVertexBuffer);
		glBufferSubDataARB(GL_ARRAY_BUFFER, 0, static_cast<ptrdiff_t>(slot.stride) * uploadCount, vertexData);
		return true;
	}

	void cGDriver::DrawPrims(uint32_t handle, uint32_t gdPrimType, void* vertexData, uint32_t count) {
		if (handle == 0 || handle > MAX_VERTEX_BUFFER_SLOTS) {
			return;
		}

		SIZE_CHECK(gdPrimType, drawModeMap);
		GLVertexBufferSlot& slot = vertexBufferSlots[handle - 1];

		if (!BindVertexBufferSlot(slot, vertexData, count)) {
			return;
		}

		// The buffer is now current for GL_ARRAY_BUFFER; offsets below are relative
		// to its start rather than to a client-side pointer.
		state.InterleavedArrays(slot.vertexFormat, slot.stride, nullptr);
		glDrawArrays(drawModeMap[gdPrimType], 0, static_cast<GLsizei>(count));

		glBindBufferARB(GL_ARRAY_BUFFER, 0);
	}

	void cGDriver::DrawPrimsIndexed(uint32_t handle, uint32_t gdPrimType, uint32_t indexCount, uint16_t* indices, void* vertexData, uint32_t vertexCount) {
		if (handle == 0 || handle > MAX_VERTEX_BUFFER_SLOTS || indices == nullptr) {
			return;
		}

		SIZE_CHECK(gdPrimType, drawModeMap);
		GLVertexBufferSlot& slot = vertexBufferSlots[handle - 1];

		if (!BindVertexBufferSlot(slot, vertexData, vertexCount)) {
			return;
		}

		if (slot.glIndexBuffer == 0) {
			glGenBuffersARB(1, &slot.glIndexBuffer);
		}

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, slot.glIndexBuffer);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, static_cast<ptrdiff_t>(sizeof(uint16_t)) * indexCount, indices, GL_DYNAMIC_DRAW);

		state.InterleavedArrays(slot.vertexFormat, slot.stride, nullptr);
		glDrawElements(drawModeMap[gdPrimType], static_cast<GLsizei>(indexCount), GL_UNSIGNED_SHORT, nullptr);

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBufferARB(GL_ARRAY_BUFFER, 0);
	}

	void cGDriver::Reset(void) {
		for (size_t i = 0; i < MAX_VERTEX_BUFFER_SLOTS; i++) {
			GLVertexBufferSlot& slot = vertexBufferSlots[i];
			if (!slot.inUse) {
				continue;
			}

			if (slot.glVertexBuffer != 0) {
				glDeleteBuffersARB(1, &slot.glVertexBuffer);
			}

			if (slot.glIndexBuffer != 0) {
				glDeleteBuffersARB(1, &slot.glIndexBuffer);
			}

			slot = GLVertexBufferSlot();
		}
	}
}
