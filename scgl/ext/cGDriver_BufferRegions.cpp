#include "../cGDriver.h"
#include <GL/glew.h>

namespace nSCGL
{
	bool cGDriver::BufferRegionEnabled(void) {
		return true;
	}

	uint32_t cGDriver::NewBufferRegion(int32_t gdBufferRegionType) {
		static GLenum regionTypeMap[] = { GL_KTX_BACK_REGION, GL_KTX_Z_REGION, GL_KTX_STENCIL_REGION };

#ifndef NDEBUG
		if (gdBufferRegionType >= sizeof(regionTypeMap) / sizeof(regionTypeMap[0])) {
			UNEXPECTED();
			return 0;
		}
#endif

		GLuint result = glNewBufferRegion(regionTypeMap[gdBufferRegionType]);
		return result;
	}

	bool cGDriver::DeleteBufferRegion(int32_t region) {
		glDeleteBufferRegion(region);
		bufferRegions.erase(region);
		return true;
	}

	bool cGDriver::ReadBufferRegion(uint32_t region, GLint x, GLint y, GLsizei width, GLsizei height, int32_t unused0, int32_t unused1) {
		glReadBufferRegion(region, x, y, width, height);
		return true;
	}

	bool cGDriver::DrawBufferRegion(uint32_t region, GLint x, GLint y, GLsizei width, GLsizei height, GLint xDest, GLint yDest) {
		glDrawBufferRegion(region, x, y, width, height, xDest, yDest);
		return true;
	}

	bool cGDriver::IsBufferRegion(uint32_t region) {
		return bufferRegions.contains(region);
	}

	bool cGDriver::CanDoPartialRegionWrites(void) {
		return true;
	}

	bool cGDriver::CanDoOffsetReads(void) {
		return true;
	}

	bool cGDriver::DeleteAllBufferRegions(void) {
		for (uint32_t region : bufferRegions) {
			glDeleteBufferRegion(region);
		}

		bufferRegions.clear();
		return true;
	}
}