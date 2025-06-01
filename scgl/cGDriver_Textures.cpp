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

#include "cGDriver.h"
#include "cGDCombiner.h"
#include "GLSupport.h"

#ifdef NDEBUG
#define DBGLOGERR()
#else
#define DBGLOGERR() dbgLastError = glGetError();
#endif

extern GLenum typeMap[16];
extern GLenum glBlendMap[11];

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

		state.BindTexture(texture);
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
		state.TexEnv(target, pname, gdParam);
	}

	void cGDriver::TexEnv(GLenum target, GLenum pname, GLfloat const* params) {
		state.TexEnv(target, pname, params);
	}

	void cGDriver::TexParameter(GLenum target, GLenum pname, GLint param) {
		state.TexParameter(target, pname, param);
	}

	void cGDriver::TexStage(GLenum texUnit) {
		if (texUnit < MAX_TEXTURE_UNITS) {
			state.TexStage(texUnit);
			return;
		}

		SetLastError(DriverError::INVALID_VALUE);
	}

	void cGDriver::TexStageCoord(uint32_t gdTexCoordSource) {
		state.TexStageCoord(gdTexCoordSource);
	}

	void cGDriver::TexStageMatrix(GLfloat const* matrix, uint32_t unknown0, uint32_t unknown1, uint32_t gdTexMatFlags) {
		state.TexStageMatrix(matrix, unknown0, unknown1, gdTexMatFlags);
	}

	void cGDriver::TexStageCombine(eGDTextureStageCombineParamType gdParamType, eGDTextureStageCombineModeParam gdParam) {
		static GLenum pnameMap[] = { GL_COMBINE_RGB, GL_COMBINE_ALPHA };
		static GLint paramMap[] = { GL_REPLACE, GL_MODULATE, GL_ADD, GL_ADD_SIGNED, GL_INTERPOLATE, GL_DOT3_RGB };

		SIZE_CHECK((int)gdParamType, pnameMap);
		SIZE_CHECK((int)gdParam, paramMap);

		glTexEnvi(GL_TEXTURE_ENV, pnameMap[(int)gdParamType], paramMap[(int)gdParam]);
	}

	void cGDriver::TexStageCombine(eGDTextureStageCombineSourceParamType gdParamType, eGDTextureStageCombineSourceParam gdParam) {
		static GLenum pnameMap[] = { GL_SRC0_RGB, GL_SRC1_RGB, GL_SRC2_RGB, GL_SOURCE3_RGB_NV, GL_SRC0_ALPHA, GL_SRC1_ALPHA, GL_SRC2_ALPHA, GL_SOURCE3_ALPHA_NV };
		static GLint paramMap[] = { GL_TEXTURE, GL_PREVIOUS, GL_CONSTANT, GL_PRIMARY_COLOR };

		SIZE_CHECK((int)gdParamType, pnameMap);
		SIZE_CHECK((int)gdParam, paramMap);

		glTexEnvi(GL_TEXTURE_ENV, pnameMap[(int)gdParamType], paramMap[(int)gdParam]);
	}

	void cGDriver::TexStageCombine(eGDTextureStageCombineOperandType gdParamType, eGDBlend gdBlend) {
		static GLenum pnameMap[] = { GL_OPERAND0_RGB, GL_OPERAND1_RGB, GL_OPERAND2_RGB, GL_OPERAND3_RGB_NV, GL_OPERAND0_ALPHA, GL_OPERAND1_ALPHA, GL_OPERAND2_ALPHA, GL_OPERAND3_ALPHA_NV };

		SIZE_CHECK((int)gdParamType, pnameMap);
		SIZE_CHECK((int)gdBlend, glBlendMap);

		glTexEnvi(GL_TEXTURE_ENV, pnameMap[(int)gdParamType], glBlendMap[(int)gdBlend]);
	}

	void cGDriver::TexStageCombine(eGDTextureStageCombineScaleParamType gdPname, eGDTextureStageCombineScaleParam gdParam) {
		static GLenum pnameMap[] = { GL_RGB_SCALE, GL_ALPHA_SCALE };
		static GLfloat paramMap[] = { 1.0f, 2.0f, 4.0f };

		SIZE_CHECK((int)gdPname, pnameMap);
		SIZE_CHECK((int)gdParam, paramMap);

		glTexEnvfv(GL_TEXTURE_ENV, pnameMap[(int)gdPname], &paramMap[(int)gdParam]);
	}

	void cGDriver::SetTexture(GLuint textureId, GLenum texUnit) {
		state.SetTexture(textureId, texUnit);
	}

	intptr_t cGDriver::GetTexture(GLenum texUnit) {
		return state.GetTexture(texUnit);
	}

	intptr_t cGDriver::CreateTexture(uint32_t texformat, uint32_t width, uint32_t height, uint32_t levels, uint32_t texhints) {
		GLuint textureId;
		glGenTextures(1, &textureId);

		glBindTexture(GL_TEXTURE_2D, textureId);
		state.SetTextureImmediately(textureId);

		int numLevels = 1;
		if (levels != 0) {
			numLevels = levels;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, numLevels - 1);

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

		glBindTexture(GL_TEXTURE_2D, texture);
		state.SetTextureImmediately(texture);

		GLint texParamWidth, texParamHeight, internalFormat;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &texParamWidth);

		if (glGetError() == GL_NO_ERROR && texParamWidth != 0) {
			glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &texParamHeight);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

			if (glFormat >= GL_COMPRESSED_RGB_S3TC_DXT1_EXT && glFormat <= GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
				GLsizei size = ((width + 3) >> 2) * ((height + 3) >> 2) * (8 + (glFormat >= GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ? 8 : 0));
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

	void cGDriver::SetCombiner(cGDCombiner const& combiner, uint32_t texUnit) {
		static eGDBlend colorOperandMap[] = {
			eGDBlend::SrcColor,
			eGDBlend::OneMinusSrcColor,
			eGDBlend::SrcAlpha,
			eGDBlend::OneMinusSrcAlpha,
		};

		static eGDBlend alphaOperandMap[] = {
			eGDBlend::SrcAlpha,
			eGDBlend::OneMinusSrcAlpha,
			eGDBlend::SrcAlpha,
			eGDBlend::OneMinusSrcAlpha,
		};

		TexStage(texUnit);
		TexEnv(0, kGDTextureEnvParamType_Mode, kGDTextureEnvParam_Combine);

		TexStageCombine(eGDTextureStageCombineParamType::RGB, (eGDTextureStageCombineModeParam)combiner.RGBCombineMode);
		TexStageCombine(eGDTextureStageCombineScaleParamType::RGB, (eGDTextureStageCombineScaleParam)combiner.RGBScale);
		TexStageCombine(eGDTextureStageCombineOperandType::Operand0RGB, (eGDBlend)colorOperandMap[combiner.RGBParams[0].OperandType]);
		TexStageCombine(eGDTextureStageCombineSourceParamType::Src0RGB, (eGDTextureStageCombineSourceParam)combiner.RGBParams[0].SourceType);
		TexStageCombine(eGDTextureStageCombineOperandType::Operand1RGB, (eGDBlend)colorOperandMap[combiner.RGBParams[1].OperandType]);
		TexStageCombine(eGDTextureStageCombineSourceParamType::Src1RGB, (eGDTextureStageCombineSourceParam)combiner.RGBParams[1].SourceType);
		TexStageCombine(eGDTextureStageCombineOperandType::Operand2RGB, (eGDBlend)colorOperandMap[combiner.RGBParams[2].OperandType]);
		TexStageCombine(eGDTextureStageCombineSourceParamType::Src2RGB, (eGDTextureStageCombineSourceParam)combiner.RGBParams[2].SourceType);

		TexStageCombine(eGDTextureStageCombineScaleParamType::Alpha, (eGDTextureStageCombineScaleParam)combiner.AlphaScale);
		TexStageCombine(eGDTextureStageCombineParamType::Alpha, (eGDTextureStageCombineModeParam)combiner.AlphaCombineMode);
		TexStageCombine(eGDTextureStageCombineOperandType::Operand0Alpha, (eGDBlend)alphaOperandMap[combiner.AlphaParams[0].OperandType]);
		TexStageCombine(eGDTextureStageCombineSourceParamType::Src0Alpha, (eGDTextureStageCombineSourceParam)combiner.AlphaParams[0].SourceType);
		TexStageCombine(eGDTextureStageCombineOperandType::Operand1Alpha, (eGDBlend)alphaOperandMap[combiner.AlphaParams[1].OperandType]);
		TexStageCombine(eGDTextureStageCombineSourceParamType::Src1Alpha, (eGDTextureStageCombineSourceParam)combiner.AlphaParams[1].SourceType);
		TexStageCombine(eGDTextureStageCombineOperandType::Operand2Alpha, (eGDBlend)alphaOperandMap[combiner.AlphaParams[2].OperandType]);
		TexStageCombine(eGDTextureStageCombineSourceParamType::Src2Alpha, (eGDTextureStageCombineSourceParam)combiner.AlphaParams[2].SourceType);
	}
}