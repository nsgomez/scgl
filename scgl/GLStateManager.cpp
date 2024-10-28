#include <cassert>
#include "GLStateManager.h"
#include "VertexFormatUtils.h"

// These are shared with the Textures compilation unit and can't be static.
GLenum typeMap[16] = {
	GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT,
	GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_5_5_1, GL_RGBA,
	GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_RGBA, GL_RGBA
};

GLenum glBlendMap[11] = {
	GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA_SATURATE
};

GLenum matrixModeMap[2] = { GL_MODELVIEW, GL_PROJECTION };

static GLenum drawModeMap[8] = { GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_QUADS, GL_QUAD_STRIP };
static GLenum glFuncMap[8] = { GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS };
static GLenum capabilityMap[8] = { GL_ALPHA_TEST, GL_DEPTH_TEST, GL_STENCIL_TEST, GL_CULL_FACE, GL_BLEND, GL_TEXTURE_2D, GL_FOG, 0 };

GLShareableState::GLShareableState() :
	interleavedFormat(-1),
	interleavedStride(0),
	interleavedPointer(nullptr),
	activeMatrixMode(0), // GL_MODELVIEW
	glActiveTextureUnit(0)
{
}

GLStateManager::GLStateManager() :
	normalArrayEnabled(false),
	colorArrayEnabled(false),
	normalOffset(0),
	colorOffset(0),
	shareable(),
	colorMaskFlag(true),
	depthFunc(1),         // GL_LESS
	depthMask(true),
	stencilFunc(7),       // GL_ALWAYS
	stencilFuncRef(0),
	stencilFuncMask(-1),
	stencilMask(-1),
	stencilFailFunc(0),   // GL_KEEP
	stencilZFailFunc(0),  // GL_KEEP
	stencilZPassFunc(0),  // GL_KEEP
	blendSrcFactor(1),    // GL_ONE
	blendDstFactor(0),    // GL_ZERO
	alphaFunc(7),         // GL_ALWAYS
	alphaRef(0.0f),
	shadeModel(1),        // GL_SMOOTH,
	ambientLightEnabled(false),
	diffuseLightEnabled(false),
	ambientLightParams{ 0.2f, 0.2f, 0.2f, 1.0f },
	diffuseLightParams{ 0.0f, 0.0f, 0.0f, 1.0f },
	isIdentityMatrix{ true, true, true },
	enabledCapabilities{ false, false, false, false, false, false, false, false },
	texEnvMode(1),        // GL_MODULATE
	texEnvColor{ 0.0f, 0.0f, 0.0f, 0.0f },
	textureParameters{ GL_LINEAR, GL_NEAREST_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT },
	textureCoordSource(0),
	activeTextureUnit(0),
	areTextureUnitsDirty(false),
	textureUnits{
		GLTextureUnit(0, &shareable),
		GLTextureUnit(1, &shareable)
	}
{
}

void GLStateManager::ApplyTextureStages() {
	for (uint32_t i = 0; i < MAX_TEXTURE_UNITS; i++) {
		textureUnits[i].ApplyStateChanges();

		if (textureUnits[i].IsEnabled() && textureUnits[i].needsTextureParamRefresh) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureParameters[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureParameters[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureParameters[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureParameters[3]);

			textureUnits[i].needsTextureParamRefresh = false;
		}
	}
}

void GLStateManager::DrawArrays(GLenum gdMode, GLint first, GLsizei count) {
	SIZE_CHECK(gdMode, drawModeMap);

	GLenum mode = drawModeMap[gdMode];

	ApplyTextureStages();
	glDrawArrays(mode, first, count);
}

void GLStateManager::DrawElements(GLenum gdMode, GLsizei count, GLenum gdType, void const* indices) {
	SIZE_CHECK(gdMode, drawModeMap);
	SIZE_CHECK(gdType, typeMap);

	GLenum mode = drawModeMap[gdMode];
	GLenum type = typeMap[gdType];

	ApplyTextureStages();
	glDrawElements(mode, count, type, indices);
}

void GLStateManager::InterleavedArrays(GLenum format, GLsizei stride, void const* pointer) {
	if (format != shareable.interleavedFormat) {
		int normalLength = RZVertexFormatNumElements(format, kGDElementType_Normal);
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

			normalOffset = RZVertexFormatElementOffset(format, kGDElementType_Normal, 0);
		}

		int colorLength = RZVertexFormatNumElements(format, kGDElementType_Color);
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

			colorOffset = RZVertexFormatElementOffset(format, kGDElementType_Color, 0);
		}
	}

	glVertexPointer(3, GL_FLOAT, stride, pointer);

	if (normalArrayEnabled) {
		glNormalPointer(GL_FLOAT, stride, reinterpret_cast<uint8_t const*>(pointer) + normalOffset);
	}

	if (colorArrayEnabled) {
		// GPU must implement GL_ARB_vertex_array_bgra or GL_EXT_vertex_array_bgra for this to work.
		// These extensions did not exist when SimCity 4 was released, so their workaround was to
		// use the CPU to swap the order of color components. That's slow - let's never do that.
		glColorPointer(GL_BGRA, GL_UNSIGNED_BYTE, stride, reinterpret_cast<uint8_t const*>(pointer) + colorOffset);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseLightParams);
	}

	shareable.interleavedPointer = pointer;
	shareable.interleavedFormat = format;
	shareable.interleavedStride = stride;
}

void GLStateManager::ColorMask(bool flag) {
	if (colorMaskFlag != flag) {
		glColorMask(flag, flag, flag, flag);
		colorMaskFlag = flag;
	}
}

void GLStateManager::DepthFunc(GLenum gdFunc) {
	if (depthFunc != gdFunc) {
		SIZE_CHECK(gdFunc, glFuncMap);

		glDepthFunc(glFuncMap[gdFunc]);
		depthFunc = gdFunc;
	}
}

void GLStateManager::DepthMask(bool flag) {
	if (depthMask != flag) {
		glDepthMask(flag);
		depthMask = flag;
	}
}

void GLStateManager::StencilFunc(GLenum gdFunc, GLint ref, GLuint mask) {
	if (stencilFunc != gdFunc || stencilFuncRef != ref || stencilFuncMask != mask) {
		SIZE_CHECK(gdFunc, glFuncMap);
		glStencilFunc(glFuncMap[gdFunc], ref, mask);

		stencilFunc = gdFunc;
		stencilFuncRef = ref;
		stencilFuncMask = mask;
	}
}

void GLStateManager::StencilMask(GLuint mask) {
	if (stencilMask != mask) {
		glStencilMask(mask);
		stencilMask = mask;
	}
}

void GLStateManager::StencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
	if (stencilFailFunc != fail || stencilZFailFunc != zfail || stencilZPassFunc != zpass) {
		static GLenum glStencilMap[] = { GL_KEEP, GL_REPLACE, GL_INCR, GL_DECR, GL_INVERT };
		SIZE_CHECK(fail, glStencilMap);
		SIZE_CHECK(zfail, glStencilMap);
		SIZE_CHECK(zpass, glStencilMap);

		glStencilOp(glStencilMap[fail], glStencilMap[zfail], glStencilMap[zpass]);

		stencilFailFunc = fail;
		stencilZFailFunc = zfail;
		stencilZPassFunc = zpass;
	}
}

void GLStateManager::BlendFunc(GLenum sfactor, GLenum dfactor) {
	if (blendSrcFactor != sfactor || blendDstFactor != dfactor) {
		SIZE_CHECK(sfactor, glBlendMap);
		SIZE_CHECK(dfactor, glBlendMap);

		glBlendFunc(glBlendMap[sfactor], glBlendMap[dfactor]);

		blendSrcFactor = sfactor;
		blendDstFactor = dfactor;
	}
}

void GLStateManager::AlphaFunc(GLenum func, GLclampf ref) {
	if (alphaFunc != func || alphaRef != ref) {
		SIZE_CHECK(func, glFuncMap);
		glAlphaFunc(glFuncMap[func], ref);

		alphaFunc = func;
		alphaRef = ref;
	}
}

void GLStateManager::ShadeModel(GLenum mode) {
	if (shadeModel != mode) {
		static GLenum shadeModelMap[2] = { GL_FLAT, GL_SMOOTH };
		SIZE_CHECK(mode, shadeModelMap);

		glShadeModel(shadeModelMap[mode]);

		shadeModel = mode;
	}
}

void GLStateManager::ColorMultiplier(float r, float g, float b) {
	if (ambientLightParams[0] != r || ambientLightParams[1] != g || ambientLightParams[2] != b) {
		ambientLightParams[0] = r;
		ambientLightParams[1] = g;
		ambientLightParams[2] = b;

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLightParams);
	}
}

void GLStateManager::AlphaMultiplier(float a) {
	if (diffuseLightParams[3] != a) {
		diffuseLightParams[3] = a;

		if (ambientLightEnabled || diffuseLightEnabled) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseLightParams);
		}
	}
}

void GLStateManager::EnableVertexColors(bool ambient, bool diffuse) {
	if (ambientLightEnabled != ambient || diffuseLightEnabled != diffuse) {
		uint8_t oldFlags = (ambientLightEnabled ? 1 : 0) | (diffuseLightEnabled ? 2 : 0);
		uint8_t newFlags = (ambient ? 1 : 0) | (diffuse ? 2 : 0);

		ambientLightEnabled = ambient;
		diffuseLightEnabled = diffuse;

		switch (newFlags) {
		case 0:
			glDisable(GL_COLOR_MATERIAL);
			return;

		case 1:
			glColorMaterial(GL_FRONT, GL_AMBIENT);
			break;

		case 2:
			glColorMaterial(GL_FRONT, GL_DIFFUSE);
			break;

		case 3:
			glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
			break;

		default:
			assert(false);
			break;
		}

		if (!oldFlags) {
			glEnable(GL_COLOR_MATERIAL);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseLightParams);
		}
	}
}

void GLStateManager::MatrixMode(GLenum mode) {
	SIZE_CHECK(mode, matrixModeMap);

	if (shareable.activeMatrixMode != mode) {
		glMatrixMode(matrixModeMap[mode]);
	}

	shareable.activeMatrixMode = mode;
}

void GLStateManager::LoadMatrix(GLfloat const* m) {
	glLoadMatrixf(m);
	isIdentityMatrix[shareable.activeMatrixMode] = false;
}

void GLStateManager::LoadIdentity(void) {
	if (!isIdentityMatrix[shareable.activeMatrixMode]) {
		glLoadIdentity();
		isIdentityMatrix[shareable.activeMatrixMode] = true;
	}
}

void GLStateManager::Enable(GLenum gdCap) {
	if (gdCap == kGDCapability_Texture2D) {
		areTextureUnitsDirty |= textureUnits[activeTextureUnit].Enable();
	}
	else if (!enabledCapabilities[gdCap] && gdCap != kGDCapability_Unused0) {
		SIZE_CHECK(gdCap, capabilityMap);

		GLenum glCap = capabilityMap[gdCap];
		glEnable(glCap);

		enabledCapabilities[gdCap] = true;
	}
}

void GLStateManager::Disable(GLenum gdCap) {
	if (gdCap == kGDCapability_Texture2D) {
		areTextureUnitsDirty |= textureUnits[activeTextureUnit].Disable();
	}
	else if (gdCap != kGDCapability_Unused0 && enabledCapabilities[gdCap]) {
		SIZE_CHECK(gdCap, capabilityMap);

		GLenum glCap = capabilityMap[gdCap];
		glDisable(glCap);

		enabledCapabilities[gdCap] = false;
	}
}

bool GLStateManager::IsEnabled(GLenum gdCap) {
	SIZE_CHECK_RETVAL(gdCap, capabilityMap, false);

	if (gdCap != kGDCapability_Texture2D) {
		return enabledCapabilities[gdCap];
	}
	else {
		return textureUnits[activeTextureUnit].IsEnabled();
	}
}

void GLStateManager::TexEnv(GLenum target, GLenum pname, GLint gdParam) {
	//if (texEnvMode != gdParam) {
		GLint paramMap[] = { GL_REPLACE, GL_MODULATE, GL_DECAL, GL_BLEND, GL_COMBINE, GL_COMBINE4_NV };

		assert(pname == 0);
		SIZE_CHECK(gdParam, paramMap);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, paramMap[gdParam]);
		texEnvMode = gdParam;
	//}
}

void GLStateManager::TexEnv(GLenum target, GLenum pname, GLfloat const* params) {
	assert(pname == 1);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, params);
}

void GLStateManager::TexParameter(GLenum target, GLenum pname, GLint param) {
	static GLenum texParamNameMap[] = { GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T };
	static GLenum texParamMap[] = { GL_NEAREST, GL_LINEAR, GL_CLAMP, GL_REPEAT, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR };

	SIZE_CHECK(pname, texParamNameMap);
	SIZE_CHECK(param, texParamMap);

	textureParameters[pname] = texParamMap[param];
	for (int i = 0; i < MAX_TEXTURE_UNITS; i++) {
		textureUnits[0].needsTextureParamRefresh = true;
	}
}

void GLStateManager::TexStage(GLenum texUnit) {
	activeTextureUnit = texUnit;

	glClientActiveTexture(GL_TEXTURE0 + texUnit);
	glActiveTexture(GL_TEXTURE0 + texUnit);

	shareable.glActiveTextureUnit = texUnit;
}

void GLStateManager::TexStageCoord(uint32_t gdTexCoordSource) {
	areTextureUnitsDirty |= textureUnits[activeTextureUnit].TexStageCoord(gdTexCoordSource);
}

void GLStateManager::TexStageMatrix(GLfloat const* matrix, uint32_t unknown0, uint32_t unknown1, uint32_t gdTexMatFlags) {
	areTextureUnitsDirty |= textureUnits[activeTextureUnit].TexStageMatrix(matrix, unknown0, unknown1, gdTexMatFlags);
}

void GLStateManager::BindTexture(GLuint textureId) {
	areTextureUnitsDirty |= textureUnits[activeTextureUnit].SetTexture(textureId);
}

void GLStateManager::SetTexture(GLuint textureId, GLenum texUnit) {
	areTextureUnitsDirty |= textureUnits[texUnit].SetTexture(textureId);
}

void GLStateManager::SetTextureImmediately(GLuint textureId) {
	textureUnits[activeTextureUnit].SetTextureImmediately(textureId);
}

intptr_t GLStateManager::GetTexture(GLenum texUnit) {
	return textureUnits[texUnit].GetTexture();
}