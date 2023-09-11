#pragma once
#include <stdint.h>
#include <stdio.h>
#include "cIGZGDriver.h"
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
#define NOTIMPL() { if (gLogFile == nullptr) gLogFile = fopen("cGDriver.notimpl.log", "w"); fprintf(gLogFile, "%s\n", __FUNCSIG__); fflush(gLogFile); }
#define UNEXPECTED NOTIMPL
#define SIZE_CHECK(param, map) if (param >= sizeof(map) / sizeof(map[0])) { UNEXPECTED(); return; }
#define SIZE_CHECK_RETVAL(param, map, ret) if (param >= sizeof(map) / sizeof(map[0])) { UNEXPECTED(); return ret; }
#endif

struct TextureStageData // TODO: DEPRECATED
{
	uint32_t coordSrc;
	void const* textureHandle;
	bool toBeEnabled;
	bool currentlyEnabled;
};

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
	void TexStageCombine(eGDTextureStageCombineParamType gdParamType, eGDTextureStageCombineModeParam gdParam);
	void TexStageCombine(eGDTextureStageCombineSourceParamType gdParamType, eGDTextureStageCombineSourceParam gdParam);
	void TexStageCombine(eGDTextureStageCombineOperandType gdParamType, eGDBlend gdBlend);
	void TexStageCombine(eGDTextureStageCombineScaleParamType gdPname, eGDTextureStageCombineScaleParam gdParam);
	void SetCombiner(cGDCombiner const& combiner, uint32_t texUnit);

public:
	void SetTexture(GLuint textureId, GLenum texUnit);
	intptr_t GetTexture(GLenum texUnit);
	void CreateTexture(uint32_t texFormat, uint32_t width, uint32_t height, uint32_t levels, uint32_t texhints);
	void LoadTextureLevel(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, uint32_t gdTexFormat, uint32_t gdType, uint32_t rowLength, void const* pixels);

private:
	void ApplyTextureStages();
	
public:
	uint32_t interleavedFormat;
	uint32_t interleavedStride;
	void const* interleavedPointer;

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

	uint8_t activeMatrixMode;
	uint8_t glSavedMatrixMode;
	bool isIdentityMatrix[3];

	bool enabledCapabilities[8];

	uint8_t texEnvMode;
	GLfloat texEnvColor[4];

	GLint textureParameters[4];

	uint32_t textureCoordSource;

private:
	GLTextureUnit textureUnits[2];
	uint8_t activeTextureUnit;

private:
	// TODO: temporarily reusing what we already have
	TextureStageData textureStageData[MAX_TEXTURE_UNITS]; // 0x2c?
};