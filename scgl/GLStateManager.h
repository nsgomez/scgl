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
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <stdint.h>
#include <stdio.h>
#include "cIGZGDriver.h"
#include "GLShareableState.h"
#include "GLSupport.h"
#include "GLTextureUnit.h"

// We're still using the GL fixed function pipeline,
// so we shouldn't have a high number of texture units.
constexpr size_t MAX_TEXTURE_UNITS = 2;

extern FILE* gLogFile;

#ifdef NDEBUG
#define NOTIMPL()
#define SIZE_CHECK(...)
#define SIZE_CHECK_RETVAL(...)
#else
#define NOTIMPL() { if (gLogFile == nullptr) gLogFile = fopen("C:\\temp\\cGDriver.notimpl.log", "w"); fprintf(gLogFile, "%s\n", __FUNCSIG__); fflush(gLogFile); }
#define UNEXPECTED NOTIMPL
#define SIZE_CHECK(param, map) if (param >= sizeof(map) / sizeof(map[0])) { UNEXPECTED(); return; }
#define SIZE_CHECK_RETVAL(param, map, ret) if (param >= sizeof(map) / sizeof(map[0])) { UNEXPECTED(); return ret; }
#endif

enum GLStatefulMatrix
{
	GLStatefulMatrix_ModelView,
	GLStatefulMatrix_Projection,
	GLStatefulMatrix_Texture,
};

class GLStateManager
{
public:
	GLStateManager();

public:
	void DrawArrays(GLenum gdMode, GLint first, GLsizei count);
	void DrawElements(GLenum gdMode, GLsizei count, GLenum gdType, void const* indices);
	void InterleavedArrays(GLenum format, GLsizei stride, void const* pointer);

public:
	void ColorMask(bool flag);
	void DepthFunc(GLenum gdFunc);
	void DepthMask(bool flag);
	void StencilFunc(GLenum gdFunc, GLint ref, GLuint mask);
	void StencilMask(GLuint mask);
	void StencilOp(GLenum fail, GLenum zfail, GLenum zpass);
	void BlendFunc(GLenum sfactor, GLenum dfactor);
	void AlphaFunc(GLenum func, GLclampf ref);
	void ShadeModel(GLenum mode);
	//void Fog(uint32_t gdFogParamType, uint32_t gdFogParam);
	//void Fog(uint32_t gdFogParamType, GLfloat const* params);
	void ColorMultiplier(float r, float g, float b);
	void AlphaMultiplier(float a);
	void EnableVertexColors(bool ambient, bool diffuse);

public:
	void MatrixMode(GLenum mode);
	void LoadMatrix(GLfloat const* m);
	void LoadIdentity(void);

public:
	void Enable(GLenum gdCap);
	void Disable(GLenum gdCap);
	bool IsEnabled(GLenum gdCap);

public:
	void TexEnv(GLenum target, GLenum pname, GLint gdParam);
	void TexEnv(GLenum target, GLenum pname, GLfloat const* params);
	void TexParameter(GLenum target, GLenum pname, GLint param);
	void TexStage(GLenum texUnit);
	void TexStageCoord(uint32_t gdTexCoordSource);
	void TexStageMatrix(GLfloat const* matrix, uint32_t unknown0, uint32_t unknown1, uint32_t gdTexMatFlags);

public:
	void BindTexture(GLuint textureId);
	void SetTexture(GLuint textureId, GLenum texUnit);
	void SetTextureImmediately(GLuint textureId);
	intptr_t GetTexture(GLenum texUnit);

private:
	void ApplyTextureStages();
	
public:
	struct GLShareableState shareable;

private:
	bool normalArrayEnabled;
	bool colorArrayEnabled;
	uint8_t normalOffset;
	uint8_t colorOffset;

	bool colorMaskFlag;

	uint8_t depthFunc;
	bool depthMask;

	uint8_t stencilFunc;
	GLint stencilFuncRef;
	GLuint stencilFuncMask;

	GLuint stencilMask;
	uint8_t stencilFailFunc;
	uint8_t stencilZFailFunc;
	uint8_t stencilZPassFunc;

	uint8_t blendSrcFactor;
	uint8_t blendDstFactor;

	uint8_t alphaFunc;
	GLclampf alphaRef;

	uint8_t shadeModel;

	bool ambientLightEnabled;
	bool diffuseLightEnabled;
	float ambientLightParams[4];
	float diffuseLightParams[4];

	bool isIdentityMatrix[3];

	bool enabledCapabilities[8];

	uint8_t texEnvMode;
	GLfloat texEnvColor[4];

	GLint textureParameters[4];

	uint32_t textureCoordSource;

private:
	GLTextureUnit textureUnits[2];
	uint8_t activeTextureUnit;
	bool areTextureUnitsDirty;
};