#include "../cGDriver.h"
#include "../GLSupport.h"

#define MAX_VERTICES 8192

// SimCity 4 only uses one format for vertex buffers.
#define VERTEX_BUFFER_FORMAT 0xb

namespace nSCGL
{
	extern GLenum drawModeMap[8];

	uint32_t cGDriver::FindFreeVertexBuffer() {
		for (uint32_t i = 0; i < vertexBuffers.size(); i++) {
			if (!vertexBuffers[i].locked) {
				return i + 1;
			}
		}

		return 0;
	}

	uint32_t cGDriver::AllocateVertexBuffer() {
		GLuint bufferName;
		glGenBuffers(1, &bufferName);

		if (bufferName == 0) {
			return 0;
		}

		VertexBufferData vertexBuffer{};
		vertexBuffer.size = VertexFormatStride(VERTEX_BUFFER_FORMAT) * MAX_VERTICES;
		vertexBuffer.handle = new uint8_t[vertexBuffer.size];

		glBindBuffer(GL_ARRAY_BUFFER, bufferName);
		glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (bufferName > vertexBuffers.size()) {
			vertexBuffers.push_back(vertexBuffer);
		}
		else {
			vertexBuffers[bufferName - 1] = vertexBuffer;
		}

		return bufferName;
	}

	uint32_t cGDriver::GetVertexBufferName(uint32_t gdVertexFormat) {
		GLuint bufferName = FindFreeVertexBuffer();
		if (bufferName != 0) {
			return bufferName;
		}

		return AllocateVertexBuffer();
	}

	uint32_t cGDriver::VertexBufferType(uint32_t id) {
		return VERTEX_BUFFER_FORMAT;
	}

	uint32_t cGDriver::MaxVertices(uint32_t id) {
		return MAX_VERTICES;
	}

	void* cGDriver::GetVertices(uint32_t id, uint32_t count) {
		if (id == 0 || count > MAX_VERTICES) {
			return 0;
		}

		VertexBufferData& vertexBuffer = vertexBuffers[id - 1];
		vertexBuffer.count = count;
		vertexBuffer.locked = true;
		return vertexBuffer.handle;
	}

	void* cGDriver::ContinueVertices(uint32_t id, uint32_t count) {
		if (id == 0) {
			return 0;
		}

		VertexBufferData& vertexBuffer = vertexBuffers[id - 1];
		if (vertexBuffer.count + count >= MAX_VERTICES) {
			return nullptr;
		}

		uint32_t oldCount = vertexBuffer.count;
		vertexBuffer.count += count;
		return vertexBuffer.handle + oldCount;
	}

	void cGDriver::ReleaseVertices(uint32_t id) {
		vertexBuffers[id - 1].locked = false;
	}

	void cGDriver::DrawPrims(uint32_t id, uint32_t gdPrimType) {
		ApplyTextureStages();
		MakeVertexBufferActive(id);
		glDrawArrays(drawModeMap[gdPrimType], 0, vertexBuffers[id - 1].count);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void cGDriver::DrawPrimsIndexed(uint32_t id, uint32_t gdPrimType, uint32_t count, uint16_t* indices) {
		ApplyTextureStages();
		MakeVertexBufferActive(id);
		glDrawElements(drawModeMap[gdPrimType], count, GL_UNSIGNED_SHORT, indices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void cGDriver::MakeVertexBufferActive(uint32_t id) {
		VertexBufferData const& vertexBuffer = vertexBuffers[id - 1];

		int stride = VertexFormatStride(VERTEX_BUFFER_FORMAT);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		GLsizei size = stride * vertexBuffer.count;
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertexBuffer.handle);
		//glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size, vertexBuffer.handle, GL_STATIC_DRAW);
		glVertexPointer(3, GL_FLOAT, stride, 0);

		int normalLength = VertexFormatNumElements(VERTEX_BUFFER_FORMAT, 3);
		if (normalLength == 0) {
			if (normalArrayEnabled) {
				glDisableClientState(GL_NORMAL_ARRAY);
				normalArrayEnabled = false;
			}
		}
		else {
			if (!normalArrayEnabled) {
				glEnableClientState(GL_NORMAL_ARRAY);
				normalArrayEnabled = true;
			}

			int normalOffset = VertexFormatElementOffset(VERTEX_BUFFER_FORMAT, 3, 0);
			glNormalPointer(GL_FLOAT, stride, reinterpret_cast<GLvoid const*>(normalOffset));
		}

		int colorLength = VertexFormatNumElements(VERTEX_BUFFER_FORMAT, 5);
		if (colorLength == 0) {
			if (colorArrayEnabled) {
				glDisableClientState(GL_COLOR_ARRAY);
				colorArrayEnabled = false;
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		else {
			if (!colorArrayEnabled) {
				glEnableClientState(GL_COLOR_ARRAY);
				colorArrayEnabled = true;
			}

			int colorOffset = VertexFormatElementOffset(VERTEX_BUFFER_FORMAT, 5, 0);
			glColorPointer(GL_BGRA, GL_UNSIGNED_BYTE, stride, reinterpret_cast<GLvoid const*>(colorOffset));
		}

		int texCoordLength = VertexFormatNumElements(VERTEX_BUFFER_FORMAT, 7);
		if (texCoordLength != 0) {
			int texCoordOffset = VertexFormatElementOffset(VERTEX_BUFFER_FORMAT, 7, 0);
			glTexCoordPointer(2, GL_FLOAT, stride, reinterpret_cast<GLvoid const*>(texCoordOffset));
		}
	}

	void cGDriver::Reset(void) {
		NOTIMPL();
		/*for (uint32_t id = 1; id <= vertexBuffers.size(); id++) {
			VertexBufferData const& vertexBuffer = vertexBuffers[id - 1];
			ReleaseVertices(id);
			glDeleteBuffers(1, &id);
		}

		vertexBuffers.clear();
		vertexBuffers.reserve(48);*/
	}
}