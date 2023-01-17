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