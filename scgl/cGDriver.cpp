#include "cGDriver.h"
#include <GL/glew.h>

FILE* gLogFile = nullptr;
cIGZGBufferRegionExtension::~cIGZGBufferRegionExtension() { }
cIGZGDriverVertexBufferExtension::~cIGZGDriverVertexBufferExtension() { }

/*static_assert(sizeof(sGDMode) == 56U);
static_assert(offsetof(sGDMode, fullscreen) == 0x20);
static_assert(offsetof(sGDMode, is3DAccelerated) == 0x22);
static_assert(offsetof(sGDMode, _unknownFuncPtr) == 0x34);*/

namespace nSCGL
{
	// This one is shared with the Textures compilation unit and can't be static.
	GLenum typeMap[16] = {
		GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT,
		GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_5_5_1, GL_RGBA,
		GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_RGBA, GL_RGBA
	};

	static GLenum drawModeMap[8] = { GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_QUADS, GL_QUAD_STRIP };
	static GLenum glFuncMap[8] = { GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS };
	static GLenum capabilityMap[8] = { GL_ALPHA_TEST, GL_DEPTH_TEST, GL_STENCIL_TEST, GL_CULL_FACE, GL_BLEND, GL_TEXTURE_2D, GL_FOG, 0 };
	static GLenum fogParamTypeMap[] = { GL_FOG_MODE, GL_FOG_COLOR, GL_FOG_DENSITY, GL_FOG_START, GL_FOG_END, GL_FOG_COORD_SRC };

	static GLenum glBlendMap[11] = {
		GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR,
		GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA_SATURATE
	};

	cGDriver::cGDriver() :
		window(nullptr),
		glfwExtensionCount(),
		glfwExtensions(),
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
		colorMultiplierR(0.0f),
		colorMultiplierG(0.0f),
		colorMultiplierB(0.0f),
		colorMultiplierA(0.0f),
		normalArrayEnabled(false),
		colorArrayEnabled(false),
		ambientMaterialEnabled(false),
		diffuseMaterialEnabled(false),
		activeTextureStage(0),
		maxTextureUnits(0),
		deviceContext(nullptr),
		bufferRegionFlags(0)
	{
	}

	cGDriver::~cGDriver() {
	}

	void cGDriver::DrawArrays(GLenum gdMode, GLint first, GLsizei count) {
		SIZE_CHECK(gdMode, drawModeMap);

		GLenum mode = drawModeMap[gdMode];

		NOTIMPL();
		SetTextureState();
		glDrawArrays(mode, first, count);
	}

	void cGDriver::DrawElements(GLenum gdMode, GLsizei count, GLenum gdType, void const* indices) {
		SIZE_CHECK(gdMode, drawModeMap);
		SIZE_CHECK(gdType, typeMap);

		GLenum mode = drawModeMap[gdMode];
		GLenum type = typeMap[gdType];

		NOTIMPL();
		SetTextureState();
		glDrawElements(mode, count, type, indices);
	}

	void cGDriver::InterleavedArrays(GLenum format, GLsizei stride, void const* pointer) {
		if (stride == 0) {
			stride = VertexFormatStride(format);
		}

		int normalLength = VertexFormatNumElements(format, 3);
		int colorLength = VertexFormatNumElements(format, 5);

		float const* fPointer = reinterpret_cast<float const*>(pointer);
		glVertexPointer(3, GL_FLOAT, stride, pointer);

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

			int normalOffset = VertexFormatElementOffset(format, 3, 0);
			glNormalPointer(GL_FLOAT, stride, reinterpret_cast<uint8_t const*>(pointer) + normalOffset);
		}

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

			int colorOffset = VertexFormatElementOffset(format, 5, 0);
			glColorPointer(sizeof(GLfloat), GL_FLOAT, stride, reinterpret_cast<uint8_t const*>(pointer) + colorOffset);
		}

		interleavedFormat = format;
		interleavedStride = stride;
		interleavedPointer = pointer;
	}

	void cGDriver::SetTextureState() {
		/*for (uint32_t i = 0; i < maxTextureUnits; i++) { // TODO: NOTIMPL(): 0x28 (repeated by size 0xc) - texture data?
			if (false) { // NOTIMPL()
				if (false) { // NOTIMPL()
					if (false) { // NOTIMPL() - *(this + 0xF4) != 0
						glClientActiveTexture(GL_TEXTURE0 + i);
					}

					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					// NOTIMPL()
				}
			}
			else {
				if (false) { // NOTIMPL() - *(this + 0xF4) != 0
					glClientActiveTexture(GL_TEXTURE0 + i);
				}

				if (false) { // NOTIMPL()
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				}
			}
		}*/
	}

	uint32_t cGDriver::MakeVertexFormat(uint32_t, intptr_t gdElementTypePtr) {
		NOTIMPL();
		return UINT_MAX;
	}

	uint32_t cGDriver::MakeVertexFormat(uint32_t gdVertexFormat) {
		// nRZSimGL::PackStandardVertexFormat(eGDVertexFormat) -> eGDVertexFormat
		static uint32_t formatToPackedFormatMap[] = {
			0x00000000, 0x80000101, 0x80004001, 0x80008001,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000, 0x00000000, 0x80004101, 0x80008101,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x80000001, 0x80000041, 0x80000141, 0x80004041,
			0x80008041, 0x80004141, 0x80008141,
		};

		static constexpr size_t formatMapLength = sizeof(formatToPackedFormatMap) / sizeof(uint32_t);
		if (gdVertexFormat >= formatMapLength) {
			return gdVertexFormat;
		}

		return formatToPackedFormatMap[gdVertexFormat];
	}

	uint32_t cGDriver::VertexFormatStride(uint32_t gdVertexFormat) {
		switch (gdVertexFormat) {
		case 0x01:
			return 0x10;
		case 0x02:
			return 0x14;
		case 0x03:
		case 0x22:
			return 0x1c;
		case 0x0a:
		case 0x21:
			return 0x18;
		case 0x0b:
		case 0x23:
			return 0x20;
		case 0x20:
			return 0xc;
		case 0x24:
			return 0x28;
		case 0x25:
			return 0x24;
		case 0x26:
			return 0x2c;
		}

		if (gdVertexFormat < 0x80000000) {
			gdVertexFormat = MakeVertexFormat(gdVertexFormat);
		}

		gdVertexFormat &= 0x7fffffff;
		uint32_t stride = ((gdVertexFormat >> 10) & 0xf)
			+ (((gdVertexFormat >> 14) & 0xf) * 2)
			+ ((gdVertexFormat >> 8) & 0x3)
			+ ((gdVertexFormat & 3) * 3);

		stride *= sizeof(GLfloat);
		if ((gdVertexFormat & 0xf) != 0) {
			uint32_t addlStride = ((gdVertexFormat >> 18) & 0xf)
				+ (((gdVertexFormat >> 6) & 1) * 3)
				+ (((gdVertexFormat >> 22) & 0xf) * 4)
				+ ((gdVertexFormat >> 2) & 7)
				+ ((gdVertexFormat >> 7) & 1)
				+ ((gdVertexFormat >> 5) & 1);

			addlStride *= 4;
			stride += addlStride;
		}

		return stride;
	}

	uint32_t cGDriver::VertexFormatElementOffset(uint32_t gdVertexFormat, uint32_t gdElementType, uint32_t count) {
		static uint32_t elementTypeOffsetMap[] = { 12, 4, 4, 12, 4, 4, 4, 8, 12, 16 };
		static uint32_t elementTypeVertexFormatMap[] = { 0xf, 0x0, 0x3, 0x1f, 0x3f, 0x7f, 0xff, 0x3ff, 0x3fff, 0x3ffff, 0x3fffff };

		uint32_t offset = elementTypeOffsetMap[gdElementType] * count;
		if (gdVertexFormat < 0x80000000) {
			gdVertexFormat = MakeVertexFormat(gdVertexFormat);
		}

		if (gdElementType > 0) {
			offset += VertexFormatStride((elementTypeVertexFormatMap[gdElementType] | 0x80000000) & gdVertexFormat);
		}

		return offset;
	}

	uint32_t cGDriver::VertexFormatNumElements(uint32_t gdVertexFormat, uint32_t gdElementType) {
		static uint32_t elementTypeShiftFactor[] = { 0, 2, 5, 6, 7, 8, 10, 14, 18, 22 };
		static uint32_t elementTypeMask[] = { 0x3, 0x7, 0x1, 0x1, 0x1, 0x3, 0xf, 0xf, 0xf };

		if (gdVertexFormat < 0x80000000) {
			gdVertexFormat = MakeVertexFormat(gdVertexFormat);
		}

		return (gdVertexFormat >> elementTypeShiftFactor[gdElementType]) & elementTypeMask[gdElementType];
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
		glColorMask(flag, flag, flag, flag);
	}

	void cGDriver::DepthFunc(GLenum gdFunc) {
		SIZE_CHECK(gdFunc, glFuncMap);
		glDepthFunc(glFuncMap[gdFunc]);
	}

	void cGDriver::DepthMask(bool flag) {
		glDepthMask(flag);
	}

	void cGDriver::StencilFunc(GLenum gdFunc, GLint ref, GLuint mask) {
		SIZE_CHECK(gdFunc, glFuncMap);

		if (videoModes[currentVideoMode].supportsStencilBuffer) {
			glStencilFunc(glFuncMap[gdFunc], ref, mask);
		}
	}

	void cGDriver::StencilMask(GLuint mask) {
		if (videoModes[currentVideoMode].supportsStencilBuffer) {
			glStencilMask(mask);
		}
	}

	void cGDriver::StencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
		static GLenum glStencilMap[] = { GL_KEEP, GL_REPLACE, GL_INCR, GL_DECR, GL_INVERT };
		SIZE_CHECK(fail, glStencilMap);
		SIZE_CHECK(zfail, glStencilMap);
		SIZE_CHECK(zpass, glStencilMap);

		if (videoModes[currentVideoMode].supportsStencilBuffer) {
			glStencilOp(glStencilMap[fail], glStencilMap[zfail], glStencilMap[zpass]);
		}
	}

	void cGDriver::BlendFunc(GLenum sfactor, GLenum dfactor) {
		SIZE_CHECK(sfactor, glBlendMap);
		SIZE_CHECK(dfactor, glBlendMap);

		glBlendFunc(glBlendMap[sfactor], glBlendMap[dfactor]);
	}

	void cGDriver::AlphaFunc(GLenum func, GLclampf ref) {
		SIZE_CHECK(func, glFuncMap);
		glAlphaFunc(glFuncMap[func], ref);
	}

	void cGDriver::ShadeModel(GLenum mode) {
		static GLenum shadeModelMap[2] = { GL_FLAT, GL_SMOOTH };
		SIZE_CHECK(mode, shadeModelMap);

		glShadeModel(shadeModelMap[mode]);
	}

	void cGDriver::Fog(uint32_t gdFogParamType, uint32_t gdFogParam) {
		static GLenum fogParamMap[] = { GL_EXP, GL_EXP2, GL_LINEAR, GL_FOG_COORD, GL_ZERO };
		glFogi(fogParamTypeMap[gdFogParamType], fogParamMap[gdFogParam]);
	}

	void cGDriver::Fog(uint32_t gdFogParamType, GLfloat const* params) {
		glFogfv(fogParamTypeMap[gdFogParamType], params);
	}

	void cGDriver::ColorMultiplier(float r, float g, float b) {
		colorMultiplierR = r;
		colorMultiplierG = g;
		colorMultiplierB = b;
		SetLightingParameters();
	}

	void cGDriver::AlphaMultiplier(float a) {
		colorMultiplierA = a;
		SetLightingParameters();
	}

	void cGDriver::SetLightingParameters() {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		EnableVertexColors(ambientMaterialEnabled, diffuseMaterialEnabled);

		GLfloat params[4] = { colorMultiplierR, colorMultiplierG, colorMultiplierB, colorMultiplierA };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, params);

		params[0] = 1.0f;
		params[1] = 0.0f;
		params[2] = 0.0f;
		params[3] = 1.0f;
		glMaterialfv(GL_FRONT, GL_AMBIENT, params);

		params[0] = 0.0f;
		params[1] = 0.0f;
		params[2] = 0.0f;
		params[3] = colorMultiplierA;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, params);
	}

	void cGDriver::EnableVertexColors(bool ambient, bool diffuse) {
		ambientMaterialEnabled = ambient;
		diffuseMaterialEnabled = diffuse;

		if (!ambient && !diffuse) {
			glDisable(GL_COLOR_MATERIAL);
			return;
		}

		glEnable(GL_COLOR_MATERIAL);
		if (ambient && diffuse) {
			glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		}
		else if (diffuse) {
			glColorMaterial(GL_FRONT, GL_DIFFUSE);
		}
		else {
			glColorMaterial(GL_FRONT, GL_AMBIENT);
		}
	}

	void cGDriver::MatrixMode(GLenum mode) {
		const GLenum modeMap[] = { GL_MODELVIEW, GL_PROJECTION };
		SIZE_CHECK(mode, modeMap);

		glMatrixMode(modeMap[mode]);
	}

	void cGDriver::LoadMatrix(GLfloat const* m) {
		glLoadMatrixf(m);
	}

	void cGDriver::LoadIdentity(void) {
		glLoadIdentity();
	}

	void cGDriver::Enable(GLenum gdCap) {
		SIZE_CHECK(gdCap, capabilityMap);

		GLenum glCap = capabilityMap[gdCap];
		if (glCap != 0) {
			glEnable(glCap);
		}

		if (gdCap == kGDCapability_Texture2D) {
			textureStageData[activeTextureStage].enabled = true;
		}
	}

	void cGDriver::Disable(GLenum gdCap) {
		SIZE_CHECK(gdCap, capabilityMap);

		GLenum glCap = capabilityMap[gdCap];
		if (glCap != 0) {
			glDisable(glCap);
		}

		if (gdCap == kGDCapability_Texture2D) {
			textureStageData[activeTextureStage].enabled = false;
		}
	}

	bool cGDriver::IsEnabled(GLenum gdCap) {
		SIZE_CHECK_RETVAL(gdCap, capabilityMap, false);

		GLenum glCap = capabilityMap[gdCap];
		if (glCap != 0) {
			bool result = glIsEnabled(glCap) != 0;
			return result;
		}

		return false;
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