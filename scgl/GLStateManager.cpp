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

static GLenum drawModeMap[8] = { GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_QUADS, GL_QUAD_STRIP };
static GLenum glFuncMap[8] = { GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS };
static GLenum matrixModeMap[] = { GL_MODELVIEW, GL_PROJECTION };
static GLenum capabilityMap[8] = { GL_ALPHA_TEST, GL_DEPTH_TEST, GL_STENCIL_TEST, GL_CULL_FACE, GL_BLEND, GL_TEXTURE_2D, GL_FOG, 0 };

GLStateManager::GLStateManager() :
	normalArrayEnabled(false),
	colorArrayEnabled(false),
	normalOffset(0),
	colorOffset(0),
	interleavedFormat(-1),
	interleavedStride(0),
	interleavedPointer(nullptr),
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
	ambientLightEnabled(true),
	diffuseLightEnabled(true),
	ambientLightParams{ 0.2f, 0.2f, 0.2f, 1.0f },
	diffuseLightParams{ 0.0f, 0.0f, 0.0f, 1.0f },
	activeMatrixMode(0),  // GL_MODELVIEW
	glSavedMatrixMode(0), // GL_MODELVIEW
	isIdentityMatrix{ true, true, true },
	enabledCapabilities{ false, false, false, false, false, false, false, false },
	texEnvMode(1),        // GL_MODULATE
	texEnvColor{ 0.0f, 0.0f, 0.0f, 0.0f },
	textureParameters{ GL_LINEAR, GL_NEAREST_MIPMAP_LINEAR, GL_REPEAT, GL_REPEAT },
	textureCoordSource(0),
	textureUnits(),
	activeTextureUnit(0),
	textureStageData()
{
}

void GLStateManager::ApplyTextureStages() {
	for (uint32_t i = 0; i < MAX_TEXTURE_UNITS; i++) {
		TextureStageData& texStage = textureStageData[i];
		if (!texStage.toBeEnabled) {
			if (texStage.currentlyEnabled) {
				glClientActiveTexture(GL_TEXTURE0 + i);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);

				glActiveTexture(GL_TEXTURE0 + i);
				glDisable(GL_TEXTURE_2D);

				texStage.currentlyEnabled = false;
				texStage.textureHandle = nullptr;
			}
		}
		else {
			glClientActiveTexture(GL_TEXTURE0 + i);

			if (!texStage.currentlyEnabled) {
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glActiveTexture(GL_TEXTURE0 + i);
				glEnable(GL_TEXTURE_2D);

				texStage.currentlyEnabled = true;
			}

			int texCoordOffset = RZVertexFormatElementOffset(interleavedFormat, kGDElementType_TexCoord, texStage.coordSrc);
			void const* textureHandle = reinterpret_cast<uint8_t const*>(interleavedPointer) + texCoordOffset;

			if (texStage.textureHandle != textureHandle) {
				glTexCoordPointer(2, GL_FLOAT, interleavedStride, textureHandle);
				texStage.textureHandle = textureHandle;
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureParameters[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureParameters[1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureParameters[2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureParameters[3]);
		}
	}
}

void GLStateManager::DrawArrays(GLenum gdMode, GLint first, GLsizei count) {
	SIZE_CHECK(gdMode, drawModeMap);

	GLenum mode = drawModeMap[gdMode];

	ApplyTextureStages();
	glDrawArrays(mode, first, count);

	//glActiveTexture(GL_TEXTURE0 + activeTextureUnit);
	//glClientActiveTexture(GL_TEXTURE0 + activeTextureUnit);
}

void GLStateManager::DrawElements(GLenum gdMode, GLsizei count, GLenum gdType, void const* indices) {
	SIZE_CHECK(gdMode, drawModeMap);
	SIZE_CHECK(gdType, typeMap);

	GLenum mode = drawModeMap[gdMode];
	GLenum type = typeMap[gdType];

	ApplyTextureStages();
	glDrawElements(mode, count, type, indices);

	//glActiveTexture(GL_TEXTURE0 + activeTextureUnit);
	//glClientActiveTexture(GL_TEXTURE0 + activeTextureUnit);
}

void GLStateManager::InterleavedArrays(GLenum format, GLsizei stride, void const* pointer) {
	if (format != interleavedFormat) {
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
	}

	interleavedPointer = pointer;
	interleavedFormat = format;
	interleavedStride = stride;
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
	//if (ambientLightParams[0] != r || ambientLightParams[1] != g || ambientLightParams[2] != b) {
		ambientLightParams[0] = r;
		ambientLightParams[1] = g;
		ambientLightParams[2] = b;

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLightParams);
	//}
}

void GLStateManager::AlphaMultiplier(float a) {
	//if (diffuseLightParams[3] != a) {
		diffuseLightParams[3] = a;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseLightParams);
	//}
}

void GLStateManager::EnableVertexColors(bool ambient, bool diffuse) {
	//if (ambientLightEnabled != ambient || diffuseLightEnabled != diffuse) {
		ambientLightEnabled = ambient;
		diffuseLightEnabled = diffuse;

		uint8_t oldFlags = (ambientLightEnabled ? 1 : 0) | (diffuseLightEnabled ? 2 : 0);
		uint8_t newFlags = (ambient ? 1 : 0) | (diffuse ? 2 : 0);

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

		glEnable(GL_COLOR_MATERIAL);
	//}
}

void GLStateManager::MatrixMode(GLenum mode) {
	SIZE_CHECK(mode, matrixModeMap);
	activeMatrixMode = mode;
}

void GLStateManager::LoadMatrix(GLfloat const* m) {
	//if (glSavedMatrixMode != activeMatrixMode) {
		glMatrixMode(matrixModeMap[activeMatrixMode]);
		glSavedMatrixMode = activeMatrixMode;
	//}

	glLoadMatrixf(m);
	isIdentityMatrix[activeMatrixMode] = false;
}

void GLStateManager::LoadIdentity(void) {
	//if (!isIdentityMatrix[activeMatrixMode]) {
		//if (glSavedMatrixMode != activeMatrixMode) {
			glMatrixMode(matrixModeMap[activeMatrixMode]);
			glSavedMatrixMode = activeMatrixMode;
		//}

		glLoadIdentity();
		isIdentityMatrix[activeMatrixMode] = true;
	//}
}

void GLStateManager::Enable(GLenum gdCap) {
	if (gdCap == kGDCapability_Texture2D) {
		textureStageData[activeTextureUnit].toBeEnabled = true;
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
		textureStageData[activeTextureUnit].toBeEnabled = false;
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
		return textureStageData[activeTextureUnit].currentlyEnabled;
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
}

void GLStateManager::TexStage(GLenum texUnit) {
	activeTextureUnit = texUnit;
	glClientActiveTexture(GL_TEXTURE0 + texUnit);
	glActiveTexture(GL_TEXTURE0 + texUnit);
}

void GLStateManager::TexStageCoord(uint32_t gdTexCoordSource) {
	static float sCoord[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	static float tCoord[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	static float rCoord[] = { 0.0f, 0.0f, 1.0f, 0.0f };

	textureStageData[activeTextureUnit].coordSrc = gdTexCoordSource;

	//if (textureCoordSource != gdTexCoordSource) {
		if ((gdTexCoordSource & 0xfffffff8) == 0x10) { // mimics D3DTSS_TCI_CAMERASPACEPOSITION
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glTexGenfv(GL_S, GL_EYE_PLANE, sCoord);
			glTexGenfv(GL_T, GL_EYE_PLANE, tCoord);
			glTexGenfv(GL_R, GL_EYE_PLANE, rCoord);

			glPopMatrix();
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);
			glMatrixMode(matrixModeMap[activeMatrixMode]);
		}
		// There are technically TexGen modes for 0x20 (D3DTSS_TCI_CAMERASPACENORMAL) and
		// 0x30 (D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR), but SimCity 4 doesn't seem to
		// use them, so they're left unimplemented.
		else {
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
		}

		textureCoordSource = gdTexCoordSource;
	//}
}

void GLStateManager::TexStageMatrix(GLfloat const* matrix, uint32_t unknown0, uint32_t unknown1, uint32_t gdTexMatFlags) {
	if (matrix == nullptr) {
		//if (!isIdentityMatrix[GLStatefulMatrix_Texture]) {
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(matrixModeMap[activeMatrixMode]);

			isIdentityMatrix[GLStatefulMatrix_Texture] = true;
		//}

		return;
	}

	glMatrixMode(GL_TEXTURE);
	if ((gdTexMatFlags & 3) == 1 && unknown0 == 4 && unknown1 == 2) {
		GLfloat replacementMatrix[16];
		GLfloat* replacementPtr = replacementMatrix;
		for (int i = 0; i < 16; i++) {
			replacementPtr[i] = matrix[i];
		}

		replacementMatrix[2] = 0.0f;
		replacementMatrix[6] = 0.0f;
		replacementMatrix[10] = 1.0f;
		replacementMatrix[14] = 0.0f;

		replacementMatrix[3] = 0.0f;
		replacementMatrix[7] = 0.0f;
		replacementMatrix[11] = 0.0f;
		replacementMatrix[15] = 1.0f;

		glLoadMatrixf(replacementMatrix);
	}
	else if ((gdTexMatFlags & 1) == 0 || (unknown0 > 3 && (unknown1 > 3 || (gdTexMatFlags & 2) == 0))) {
		glLoadMatrixf(matrix);
	}
	else {
		NOTIMPL();
	}

	isIdentityMatrix[GLStatefulMatrix_Texture] = false;
	glMatrixMode(matrixModeMap[activeMatrixMode]);
}

void GLStateManager::SetTexture(GLuint textureId, GLenum texUnit) {
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glActiveTexture(GL_TEXTURE0 + activeTextureUnit);
}

intptr_t GLStateManager::GetTexture(GLenum texUnit) {
	glActiveTexture(GL_TEXTURE0 + texUnit);

	int activeTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &activeTexture);

	glActiveTexture(GL_TEXTURE0 + activeTextureUnit);
	return activeTexture;
}