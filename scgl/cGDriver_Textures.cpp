#include "cGDriver.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef NDEBUG
#define DBGLOGERR()
#else
#define DBGLOGERR() dbgLastError = glGetError();
#endif

namespace nSCGL
{
	static GLenum texEnvParamMap[2] = { GL_TEXTURE_ENV_MODE, GL_TEXTURE_ENV_COLOR };
	static GLenum internalFormatMap[8] = {
		GL_RGB5, GL_RGB8, GL_RGBA4, GL_RGB5_A1,
		GL_RGBA8, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
		GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
	};

	static GLenum formatMap[11] = {
		GL_RGB, GL_RGBA, GL_BGR, GL_BGRA, GL_ALPHA, GL_LUMINANCE, GL_LUMINANCE_ALPHA,
		GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
		GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
	};

	extern GLenum typeMap[16];

	void cGDriver::GenTextures(GLsizei n, GLuint* textures) {
		glGenTextures(n, textures);
	}

	void cGDriver::DeleteTextures(GLsizei n, GLuint const* textures) {
		glDeleteTextures(n, textures);
	}

	bool cGDriver::IsTexture(GLuint texture) {
		bool result = glIsTexture(texture) != 0;
		return result;
	}

	void cGDriver::PrioritizeTextures(GLsizei n, GLuint const* textures, GLclampf const* priorities) {
		glPrioritizeTextures(n, textures, priorities);
	}

	bool cGDriver::AreTexturesResident(GLsizei n, GLuint const* textures, bool* residences) {
		bool result = glAreTexturesResident(n, textures, reinterpret_cast<GLboolean*>(residences)) != 0;
		return result;
	}

	void cGDriver::BindTexture(GLenum target, GLuint texture) {
#ifndef NDEBUG
		if (target > 0) {
			UNEXPECTED();
			return;
		}
#endif

		glBindTexture(GL_TEXTURE_2D, texture);
	}

	void cGDriver::TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, void const* pixels) {
#ifndef NDEBUG
		if (target > 0) {
			UNEXPECTED();
			return;
		}
#endif

		NOTIMPL();
		glTexImage2D(GL_TEXTURE_2D, level, internalformat, width, height, border, format, type, pixels);
	}

	void cGDriver::PixelStore(GLenum pname, GLint param) {
#ifndef NDEBUG
		if (pname > 0) {
			UNEXPECTED();
			return;
		}
#endif

		glPixelStorei(GL_UNPACK_ROW_LENGTH, param);
	}

	void cGDriver::TexEnv(GLenum target, GLenum pname, GLint gdParam) {
		GLint paramMap[] = { GL_REPLACE, GL_MODULATE, GL_DECAL, GL_BLEND, GL_COMBINE, GL_COMBINE4_NV };
#ifndef NDEBUG
		if (pname >= sizeof(texEnvParamMap) / sizeof(texEnvParamMap[0]) || gdParam >= sizeof(paramMap) / sizeof(paramMap[0])) {
			UNEXPECTED();
			return;
		}
#endif

		glTexEnvi(GL_TEXTURE_ENV, texEnvParamMap[pname], paramMap[gdParam]);
	}

	void cGDriver::TexEnv(GLenum target, GLenum pname, GLfloat const* params) {
#ifndef NDEBUG
		if (pname >= sizeof(texEnvParamMap) / sizeof(texEnvParamMap[0])) {
			UNEXPECTED();
			return;
		}
#endif

		glTexEnvfv(GL_TEXTURE_ENV, texEnvParamMap[pname], params);
	}

	void cGDriver::TexParameter(GLenum target, GLenum pname, GLint param) {
		static GLenum texParamNameMap[] = { GL_TEXTURE_MAG_FILTER, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T };
		static GLenum texParamMap[] = { GL_NEAREST, GL_LINEAR, GL_CLAMP, GL_REPEAT, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR };

#ifndef NDEBUG
		if (pname >= sizeof(texParamNameMap) / sizeof(texParamNameMap[0]) || param >= sizeof(texParamMap) / sizeof(texParamMap[0])) {
			UNEXPECTED();
			return;
		}
#endif

		glTexParameteri(GL_TEXTURE_2D, texParamNameMap[pname], texParamMap[param]);
	}

	void cGDriver::TexStage(GLenum texture) {
		if (videoModes[currentVideoMode].supportsMultitexture || texture != 0) {
			if (texture < maxTextureUnits) {
				activeTextureStage = texture;
				glClientActiveTexture(GL_TEXTURE0 + texture);
				return;
			}

			SetLastError(DriverError::INVALID_VALUE);
		}
	}

	void cGDriver::TexStageCoord(uint32_t gdTexCoordSource) {
		static float sCoord[] = { 1.0f, 0.0f, 0.0f, 0.0f };
		static float tCoord[] = { 0.0f, 1.0f, 0.0f, 0.0f };

		textureStageData[activeTextureStage].coordSrc = gdTexCoordSource;

		//NOTIMPL();
		if ((gdTexCoordSource & 0xfffffff8) == 0x10) {
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glTexGenfv(GL_S, GL_EYE_PLANE, sCoord);
			glTexGenfv(GL_T, GL_EYE_PLANE, tCoord);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		}
		else {
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
		}
	}

	void cGDriver::TexStageMatrix(GLfloat const* matrix, uint32_t unknown0, uint32_t unknown1, uint32_t gdTexMatFlags) {
		glMatrixMode(GL_TEXTURE);
		if (matrix == nullptr) {
			glLoadIdentity();
			return;
		}

		if ((gdTexMatFlags & 3) == 1 && unknown0 == 4 && unknown1 == 2) {
			GLfloat replacementMatrix[16];
			GLfloat* replacementPtr = replacementMatrix;
			for (int i = 0; i < 16; i++) {
				*replacementPtr = *(matrix++);
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
			return;
		}

		if ((gdTexMatFlags & 1) == 0 || (unknown0 > 3 && (unknown1 > 3 || (gdTexMatFlags & 2) == 0))) {
			glLoadMatrixf(matrix);
		}
	}

	void cGDriver::TexStageCombine(eGDTextureStageCombineParamType gdParamType, eGDTextureStageCombineModeParam gdParam) {
		static GLenum pnameMap[] = { GL_COMBINE_RGB, GL_COMBINE_ALPHA };
		static GLint paramMap[] = { GL_REPLACE, GL_MODULATE, GL_ADD, GL_ADD_SIGNED, GL_INTERPOLATE, GL_DOT3_RGB_EXT };

#ifndef NDEBUG
		if (gdParamType >= sizeof(pnameMap) / sizeof(pnameMap[0]) || gdParam >= sizeof(paramMap) / sizeof(paramMap[0])) {
			UNEXPECTED();
			return;
		}
#endif

		if (!videoModes[currentVideoMode].supportsTextureEnvCombine) {
			SetLastError(DriverError::INVALID_VALUE);
			return;
		}

		glTexEnvi(GL_TEXTURE_ENV, pnameMap[gdParamType], paramMap[gdParam]);
	}

	void cGDriver::TexStageCombine(eGDTextureStageCombineSourceParamType gdParamType, eGDTextureStageCombineSourceParam gdParam) {
		static GLenum pnameMap[] = { GL_OPERAND0_RGB, GL_OPERAND1_RGB, GL_OPERAND2_RGB, GL_OPERAND3_RGB_NV, GL_OPERAND0_ALPHA, GL_OPERAND1_ALPHA, GL_OPERAND2_ALPHA, GL_OPERAND3_ALPHA_NV };
		static GLint paramMap[] = {
			GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
			GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA_SATURATE
		};

#ifndef NDEBUG
		if (gdParamType >= sizeof(pnameMap) / sizeof(pnameMap[0]) || gdParam >= sizeof(paramMap) / sizeof(paramMap[0])) {
			UNEXPECTED();
			return;
		}
#endif

		if (!videoModes[currentVideoMode].supportsTextureEnvCombine) {
			SetLastError(DriverError::INVALID_VALUE);
			return;
		}

		glTexEnvi(GL_TEXTURE_ENV, pnameMap[gdParamType], paramMap[gdParam]);
	}

	void cGDriver::TexStageCombine(eGDTextureStageCombineOperandType gdParamType, eGDBlend gdBlend) {
		static GLenum pnameMap[] = { GL_SRC0_RGB, GL_SRC1_RGB, GL_SRC2_RGB, GL_SOURCE3_RGB_NV, GL_SRC0_ALPHA, GL_SRC1_ALPHA, GL_SRC2_ALPHA, GL_SOURCE3_ALPHA_NV };
		static GLint paramMap[] = { GL_TEXTURE, GL_PREVIOUS, GL_CONSTANT, GL_PRIMARY_COLOR };

#ifndef NDEBUG
		if (gdParamType >= sizeof(pnameMap) / sizeof(pnameMap[0]) || gdBlend >= sizeof(paramMap) / sizeof(paramMap[0])) {
			UNEXPECTED();
			return;
		}
#endif

		if (!videoModes[currentVideoMode].supportsTextureEnvCombine) {
			SetLastError(DriverError::INVALID_VALUE);
			return;
		}

		glTexEnvi(GL_TEXTURE_ENV, pnameMap[gdParamType], paramMap[gdBlend]);
	}

	void cGDriver::TexStageCombine(eGDTextureStageCombineScaleParamType gdPname, eGDTextureStageCombineScaleParam gdParam) {
		static GLenum pnameMap[] = { GL_RGB_SCALE, GL_ALPHA_SCALE };
		static GLfloat paramMap[] = { 1.0f, 2.0f, 4.0f };

#ifndef NDEBUG
		if (gdPname >= sizeof(pnameMap) / sizeof(pnameMap[0]) || gdParam >= sizeof(paramMap) / sizeof(paramMap[0])) {
			UNEXPECTED();
			return;
		}
#endif

		if (!videoModes[currentVideoMode].supportsTextureEnvCombine) {
			SetLastError(DriverError::INVALID_VALUE);
			return;
		}

		glTexEnvfv(GL_TEXTURE_ENV, pnameMap[gdPname], &paramMap[gdParam]);
	}

	void cGDriver::SetTexture(GLenum target, GLuint texture) {
		glActiveTexture(GL_TEXTURE0 + texture);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	intptr_t cGDriver::GetTexture(GLuint texture) {
		glActiveTexture(GL_TEXTURE0 + texture);

		int activeTexture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &activeTexture);
		return activeTexture;
	}

	intptr_t cGDriver::CreateTexture(uint32_t texformat, uint32_t width, uint32_t height, uint32_t levels, uint32_t texhints) {
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		int numLevels = 1;
		if (levels != 0) {
			numLevels = levels;
		}

		GLint internalFormat = internalFormatMap[texformat];
		for (int i = 0; i < numLevels; i++) {
			glTexImage2D(GL_TEXTURE_2D, i, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			width = (width < 2) ? 1 : (width >> 1);
			height = (height < 2) ? 1 : (height >> 1);
		}

		return textureId;
	}

	void cGDriver::LoadTextureLevel(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, uint32_t gdTexFormat, uint32_t gdType, uint32_t rowLength, void const* pixels) {
		GLenum glFormat = formatMap[gdTexFormat];
		GLenum glType = typeMap[gdType];

		glGetError();
		glBindTexture(GL_TEXTURE_2D, texture);
		glGetError();

		GLint texParamWidth, texParamHeight, internalFormat;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &texParamWidth);

		if (glGetError() == GL_NO_ERROR && texParamWidth != 0) {
			glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &texParamHeight);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

			if (glFormat >= GL_COMPRESSED_RGB_S3TC_DXT1_EXT && glFormat <= GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
				GLsizei size = ((width + 3) >> 2) * ((height + 3) >> 2) * (8 + (glFormat >= GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 0));
				if (xoffset == 0 && yoffset == 0 && width == texParamWidth && height == texParamHeight) {
					glCompressedTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, size, pixels);
				}
				else {
					glCompressedTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, glFormat, size, pixels);
				}

				return;
			}

			glPixelStorei(GL_UNPACK_ROW_LENGTH, rowLength);

			if (xoffset == 0 && yoffset == 0 && width == texParamWidth && height == texParamHeight) {
				glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, 0, glFormat, glType, pixels);
			}
			else {
				glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, glFormat, glType, pixels);
			}
		}
	}

	void cGDriver::SetCombiner(intptr_t gdCombiner, uint32_t) {
		NOTIMPL();
	}
}