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
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "cGDriver.h"
#include "GLSupport.h"
#include "VertexFormatUtils.h"

FILE* gLogFile = nullptr;
cIGZGBufferRegionExtension::~cIGZGBufferRegionExtension() { }
cIGZGDriverVertexBufferExtension::~cIGZGDriverVertexBufferExtension() { }

/*static_assert(sizeof(sGDMode) == 56U);
static_assert(offsetof(sGDMode, fullscreen) == 0x20);
static_assert(offsetof(sGDMode, is3DAccelerated) == 0x22);
static_assert(offsetof(sGDMode, _unknownFuncPtr) == 0x34);*/

namespace nSCGL
{
	static GLenum fogParamTypeMap[] = { GL_FOG_MODE, GL_FOG_COLOR, GL_FOG_DENSITY, GL_FOG_START, GL_FOG_END, GL_FOG_COORD_SRC };

	cGDriver::cGDriver() :
		lastError(DriverError::OK),
#ifndef NDEBUG
		dbgLastError(GL_NO_ERROR),
#endif
		currentVideoMode(-1),
		driverInfo("Maxis 3D GDriver\nOpenGL\n3.0\n"),
		videoModeCount(0),
		refCount(0),
		windowWidth(0),
		windowHeight(0),
		viewportX(0),
		viewportY(0),
		viewportWidth(0),
		viewportHeight(0),
		bufferRegionFlags(0),
		framebufferHandles(),
		renderbufferHandles(),
		framebufferMasks(),
		supportedExtensions(),
		windowHandle(nullptr),
		deviceContext(nullptr),
		glContext(nullptr)
	{
	}

	cGDriver::~cGDriver() {
	}

	void cGDriver::DrawArrays(GLenum gdMode, GLint first, GLsizei count) {
		state.DrawArrays(gdMode, first, count);
	}

	void cGDriver::DrawElements(GLenum gdMode, GLsizei count, GLenum gdType, void const* indices) {
		state.DrawElements(gdMode, count, gdType, indices);
	}

	void cGDriver::InterleavedArrays(GLenum format, GLsizei stride, void const* pointer) {
		if (stride == 0) {
			stride = VertexFormatStride(format);
		}

		state.InterleavedArrays(format, stride, pointer);
	}

	uint32_t cGDriver::MakeVertexFormat(uint32_t, intptr_t gdElementTypePtr) {
		NOTIMPL();
		return UINT_MAX;
	}

	uint32_t cGDriver::MakeVertexFormat(uint32_t gdVertexFormat) {
		return RZMakeVertexFormat(gdVertexFormat);
	}

	uint32_t cGDriver::VertexFormatStride(uint32_t gdVertexFormat) {
		return RZVertexFormatStride(gdVertexFormat);
	}

	uint32_t cGDriver::VertexFormatElementOffset(uint32_t gdVertexFormat, uint32_t gdElementType, uint32_t count) {
		return RZVertexFormatElementOffset(gdVertexFormat, gdElementType, count);
	}

	uint32_t cGDriver::VertexFormatNumElements(uint32_t gdVertexFormat, uint32_t gdElementType) {
		return RZVertexFormatNumElements(gdVertexFormat, gdElementType);
	}

	void cGDriver::Clear(GLbitfield mask) {
		GLbitfield glMask = 0;
		glMask  = (mask & 0x1000) >> 4; // GL_DEPTH_BUFFER_BIT   (0x100)
		glMask |= (mask & 0x2000) >> 3; // GL_STENCIL_BUFFER_BIT (0x400)
		glMask |= (mask & 0x4000);      // GL_COLOR_BUFFER_BIT   (0x4000)
		glClear(glMask);
	}

	void cGDriver::ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
		//glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClearColor(red, green, blue, alpha);
	}

	void cGDriver::ClearDepth(GLclampd depth) {
		glClearDepth(depth);
	}

	void cGDriver::ClearStencil(GLint s) {
		glClearStencil(s);
	}

	void cGDriver::ColorMask(bool flag) {
		state.ColorMask(flag);
	}

	void cGDriver::DepthFunc(GLenum gdFunc) {
		state.DepthFunc(gdFunc);
	}

	void cGDriver::DepthMask(bool flag) {
		state.DepthMask(flag);
	}

	void cGDriver::StencilFunc(GLenum gdFunc, GLint ref, GLuint mask) {
		state.StencilFunc(gdFunc, ref, mask);
	}

	void cGDriver::StencilMask(GLuint mask) {
		state.StencilMask(mask);
	}

	void cGDriver::StencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
		state.StencilOp(fail, zfail, zpass);
	}

	void cGDriver::BlendFunc(GLenum sfactor, GLenum dfactor) {
		state.BlendFunc(sfactor, dfactor);
	}

	void cGDriver::AlphaFunc(GLenum func, GLclampf ref) {
		state.AlphaFunc(func, ref);
	}

	void cGDriver::ShadeModel(GLenum mode) {
		state.ShadeModel(mode);
	}

	void cGDriver::Fog(uint32_t gdFogParamType, uint32_t gdFogParam) {
		static GLenum fogParamMap[] = { GL_EXP, GL_EXP2, GL_LINEAR, GL_FOG_COORD, GL_ZERO };
		SIZE_CHECK(gdFogParamType, fogParamTypeMap);
		SIZE_CHECK(gdFogParam, fogParamMap);

		glFogi(fogParamTypeMap[gdFogParamType], fogParamMap[gdFogParam]);
	}

	void cGDriver::Fog(uint32_t gdFogParamType, GLfloat const* params) {
		SIZE_CHECK(gdFogParamType, fogParamTypeMap);
		glFogfv(fogParamTypeMap[gdFogParamType], params);
	}

	void cGDriver::ColorMultiplier(float r, float g, float b) {
		state.ColorMultiplier(r, g, b);
	}

	void cGDriver::AlphaMultiplier(float a) {
		state.AlphaMultiplier(a);
	}

	void cGDriver::EnableVertexColors(bool ambient, bool diffuse) {
		state.EnableVertexColors(ambient, diffuse);
	}

	void cGDriver::MatrixMode(GLenum mode) {
		state.MatrixMode(mode);
	}

	void cGDriver::LoadMatrix(GLfloat const* m) {
		state.LoadMatrix(m);
	}

	void cGDriver::LoadIdentity(void) {
		state.LoadIdentity();
	}

	void cGDriver::Enable(GLenum gdCap) {
		state.Enable(gdCap);
	}

	void cGDriver::Disable(GLenum gdCap) {
		state.Disable(gdCap);
	}

	bool cGDriver::IsEnabled(GLenum gdCap) {
		return state.IsEnabled(gdCap);
	}

	void cGDriver::GetBoolean(GLenum pname, bool* params) {
#ifndef NDEBUG
		if (pname != 0) {
			UNEXPECTED();
			return;
		}
#endif

		glGetBooleanv(GL_UNPACK_ROW_LENGTH, reinterpret_cast<GLboolean*>(params));
	}

	void cGDriver::GetInteger(GLenum pname, GLint* params) {
#ifndef NDEBUG
		if (pname != 0) {
			UNEXPECTED();
			return;
		}
#endif

		glGetIntegerv(GL_UNPACK_ROW_LENGTH, params);
	}

	void cGDriver::GetFloat(GLenum pname, GLfloat* params) {
#ifndef NDEBUG
		if (pname != 0) {
			UNEXPECTED();
			return;
		}
#endif

		glGetFloatv(GL_UNPACK_ROW_LENGTH, params);
	}

	void cGDriver::PolygonOffset(int32_t offset) {
		float fOffset = (float)offset;
		if (offset < 0) {
			fOffset += 4294967296.0f;
		}

		glPolygonOffset(0.0f, fOffset);
	}

	void cGDriver::BitBlt(
		int32_t destLeft,
		int32_t destTop,
		int32_t unknownWidth1,
		int32_t unknownHeight1,
		uint32_t gdTexFormat,
		uint32_t gdType,
		void const* unknownBuffer1,
		bool unknown5,
		void const* unknownBuffer2)
	{
		uint8_t const* unknownUintBuffer1 = reinterpret_cast<uint8_t const*>(unknownBuffer1);
		uint8_t const* unknownUintBuffer2 = reinterpret_cast<uint8_t const*>(unknownBuffer2);

		SetLastError(DriverError::NOT_SUPPORTED);
	}

	void cGDriver::StretchBlt(
		int32_t destLeft,
		int32_t destTop,
		int32_t unknownWidth1,
		int32_t unknownHeight1,
		int32_t unknownWidth2,
		int32_t unknownHeight2,
		uint32_t gdTexFormat,
		uint32_t gdType,
		void const* unknownBuffer1,
		bool unknownBool,
		void const* unknownBuffer2)
	{
		uint8_t const* unknownUintBuffer1 = reinterpret_cast<uint8_t const*>(unknownBuffer1);
		uint8_t const* unknownUintBuffer2 = reinterpret_cast<uint8_t const*>(unknownBuffer2);

		SetLastError(DriverError::NOT_SUPPORTED);
	}

	void cGDriver::BitBltAlpha(
		int32_t unknown0,
		int32_t unknown1,
		int32_t unknown2,
		int32_t unknown3,
		uint32_t gdTexFormat,
		uint32_t gdType,
		void const* unknownBuffer1,
		bool unknown5,
		void const* unknownBuffer2,
		uint32_t unknown7)
	{
		uint8_t const* unknownUintBuffer1 = reinterpret_cast<uint8_t const*>(unknownBuffer1);
		uint8_t const* unknownUintBuffer2 = reinterpret_cast<uint8_t const*>(unknownBuffer2);

		SetLastError(DriverError::NOT_SUPPORTED);
	}

	void cGDriver::StretchBltAlpha(
		int32_t destLeft,
		int32_t destTop,
		int32_t unknownWidth1,
		int32_t unknownHeight1,
		int32_t unknownWidth2,
		int32_t unknownHeight2,
		uint32_t gdTexFormat,
		uint32_t gdType,
		void const* unknownBuffer1,
		bool unknown7,
		void const* unknownBuffer2,
		uint32_t unknown9)
	{
		uint8_t const* unknownUintBuffer1 = reinterpret_cast<uint8_t const*>(unknownBuffer1);
		uint8_t const* unknownUintBuffer2 = reinterpret_cast<uint8_t const*>(unknownBuffer2);

		SetLastError(DriverError::NOT_SUPPORTED);
	}

	void cGDriver::BitBltAlphaModulate(
		int32_t unknown0,
		int32_t unknown1,
		int32_t unknown2,
		uint32_t gdTexFormat,
		uint32_t gdType,
		void const* unknownBuffer1,
		bool unknown4,
		void const* unknownBuffer2,
		uint32_t unknown6)
	{
		uint8_t const* unknownUintBuffer1 = reinterpret_cast<uint8_t const*>(unknownBuffer1);
		uint8_t const* unknownUintBuffer2 = reinterpret_cast<uint8_t const*>(unknownBuffer2);

		SetLastError(DriverError::NOT_SUPPORTED);
	}

	void cGDriver::StretchBltAlphaModulate(
		int32_t destLeft,
		int32_t destTop,
		int32_t unknownWidth1,
		int32_t unknownHeight1,
		int32_t unknownWidth2,
		int32_t unknownHeight2,
		uint32_t gdTexFormat,
		uint32_t gdType,
		void const* unknownBuffer1,
		bool unknown7,
		void const* unknownBuffer2,
		uint32_t unknown9)
	{
		uint8_t const* unknownUintBuffer1 = reinterpret_cast<uint8_t const*>(unknownBuffer1);
		uint8_t const* unknownUintBuffer2 = reinterpret_cast<uint8_t const*>(unknownBuffer2);

		SetLastError(DriverError::NOT_SUPPORTED);
	}

	bool cGDriver::Punt(uint32_t, void*) {
		return false;
	}
}