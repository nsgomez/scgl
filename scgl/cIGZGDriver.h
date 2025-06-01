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

#pragma once
#include <cIGZString.h>
#include <cIGZUnknown.h>

static const uint32_t GZIID_cIGZGDriver = 0xa4554849;

enum eGDCapability
{
	kGDCapability_AlphaTest,
	kGDCapability_DepthTest,
	kGDCapability_StencilTest,
	kGDCapability_CullFace,
	kGDCapability_Blend,
	kGDCapability_Texture2D,
	kGDCapability_Fog,
	kGDCapability_Unused0,

	kGDNumCapabilities,
};

enum class eGDTextureStageCombineScaleParamType
{
	RGB,
	Alpha,

	ENUM_COUNT,
};

enum class eGDTextureStageCombineScaleParam
{
	X1,
	X2,
	X4,

	ENUM_COUNT,
};

enum class eGDTextureStageCombineOperandType
{
	Operand0RGB,
	Operand1RGB,
	Operand2RGB,
	Operand3RGB,
	Operand0Alpha,
	Operand1Alpha,
	Operand2Alpha,
	Operand3Alpha,

	ENUM_COUNT,
};

enum class eGDBlend
{
	Zero,
	One,
	SrcColor,
	OneMinusSrcColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha,
	DstColor,
	OneMinusDstColor,
	SrcAlphaSaturate,

	ENUM_COUNT,
};

enum class eGDTextureStageCombineSourceParamType
{
	Src0RGB,
	Src1RGB,
	Src2RGB,
	Src3RGB,
	Src0Alpha,
	Src1Alpha,
	Src2Alpha,
	Src3Alpha,

	ENUM_COUNT,
};

enum class eGDTextureStageCombineSourceParam
{
	Texture,
	Previous,
	Constant,
	PrimaryColor,

	ENUM_COUNT,
};

enum class eGDTextureStageCombineParamType
{
	RGB,
	Alpha,

	ENUM_COUNT,
};

enum class eGDTextureStageCombineModeParam
{
	Replace,
	Modulate,
	Add,
	AddSigned,
	Interpolate,
	Dot3RGB,

	ENUM_COUNT,
};

enum eGDTextureEnvParamType
{
	kGDTextureEnvParamType_Mode,
	kGDTextureEnvParamType_Color,

	kGDNumTextureEnvParamTypes,
};

enum eGDTextureEnvParam
{
	kGDTextureEnvParam_Replace,
	kGDTextureEnvParam_Modulate,
	kGDTextureEnvParam_Decal,
	kGDTextureEnvParam_Blend,
	kGDTextureEnvParam_Combine,
	kGDTextureEnvParam_Combine4,

	kGDNumTextureEnvParams,
};

class cGDCombiner;
struct sGDMode;

class cIGZGDriver : public cIGZUnknown
{
public:
	virtual void DrawArrays(uint32_t gdPrimType, int32_t first, int32_t count) = 0;
	virtual void DrawElements(uint32_t gdPrimType, int32_t count, uint32_t gdType, void const* indices) = 0;
	virtual void InterleavedArrays(uint32_t gdVertexFormat, int32_t, void const*) = 0;

	virtual uint32_t MakeVertexFormat(uint32_t, intptr_t gdElementTypePtr) = 0;
	virtual uint32_t MakeVertexFormat(uint32_t gdVertexFormat) = 0;
	virtual uint32_t VertexFormatStride(uint32_t gdVertexFormat) = 0;
	virtual uint32_t VertexFormatElementOffset(uint32_t gdVertexFormat, uint32_t gdElementType, uint32_t) = 0;
	virtual uint32_t VertexFormatNumElements(uint32_t gdVertexFormat, uint32_t gdElementType) = 0;

	virtual void Clear(uint32_t) = 0;
	virtual void ClearColor(float, float, float, float) = 0;
	virtual void ClearDepth(double) = 0;
	virtual void ClearStencil(int32_t) = 0;

	virtual void ColorMask(bool) = 0;
	virtual void DepthFunc(uint32_t gdTestFunc) = 0;
	virtual void DepthMask(bool) = 0;

	virtual void StencilFunc(uint32_t gdTestFunc, int32_t, uint32_t) = 0;
	virtual void StencilMask(uint32_t) = 0;
	virtual void StencilOp(uint32_t gdStencilOp, uint32_t gdStencilOp2, uint32_t gdStencilOp3) = 0;

	virtual void BlendFunc(uint32_t gdBlendFunc, uint32_t gdBlend) = 0;
	virtual void AlphaFunc(uint32_t gdTestFunc, float) = 0;
	virtual void ShadeModel(uint32_t gdShade) = 0;

	virtual void BindTexture(uint32_t gdTextureTarget, uint32_t) = 0;
	virtual void TexImage2D(uint32_t gdTextureTarget, int32_t, int32_t gdInternalTexFormat, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*) = 0;
	virtual void PixelStore(uint32_t gdParameter, int32_t) = 0;

	virtual void TexEnv(uint32_t gdTextureEnvTarget, uint32_t gdTextureEnvParamType, int32_t gdTextureEnvModeParam) = 0;
	virtual void TexEnv(uint32_t gdTextureEnvTarget, uint32_t gdTextureEnvParamType, float const*) = 0;
	virtual void TexParameter(uint32_t gdTextureTarget, uint32_t gdTextureParamType, int32_t gdTextureParam) = 0;

	virtual void Fog(uint32_t gdFogParamType, uint32_t gdFogParam) = 0;
	virtual void Fog(uint32_t gdFogParamType, float const*) = 0;

	virtual void ColorMultiplier(float r, float g, float b) = 0;
	virtual void AlphaMultiplier(float a) = 0;
	virtual void EnableVertexColors(bool, bool) = 0;

	virtual void GenTextures(int32_t, uint32_t*) = 0;
	virtual void DeleteTextures(int32_t, uint32_t const*) = 0;
	virtual bool IsTexture(uint32_t) = 0;
	virtual void PrioritizeTextures(int32_t, uint32_t const*, float const*) = 0;
	virtual bool AreTexturesResident(int32_t, uint32_t const*, bool*) = 0;

	virtual void MatrixMode(uint32_t gdMatrixTarget) = 0;
	virtual void LoadMatrix(float const*) = 0;
	virtual void LoadIdentity(void) = 0;

	virtual void Flush(void) = 0;
	virtual void Enable(uint32_t gdDriverState) = 0;
	virtual void Disable(uint32_t gdDriverState) = 0;
	virtual bool IsEnabled(uint32_t gdDriverState) = 0;

	virtual void GetBoolean(uint32_t gdParameter, bool*) = 0;
	virtual void GetInteger(uint32_t gdParameter, int32_t*) = 0;
	virtual void GetFloat(uint32_t gdParameter, float*) = 0;
	virtual uint32_t GetError(void) = 0;

	virtual void TexStage(uint32_t) = 0; // cc
	virtual void TexStageCoord(uint32_t gdTexCoordSource) = 0; // d0
	virtual void TexStageMatrix(float const*, uint32_t, uint32_t, uint32_t gdTexMatFlags) = 0; // d4
	virtual void TexStageCombine(eGDTextureStageCombineParamType gdParamType, eGDTextureStageCombineModeParam gdParam) = 0; // e4
	virtual void TexStageCombine(eGDTextureStageCombineSourceParamType gdParamType, eGDTextureStageCombineSourceParam gdParam) = 0; // dc
	virtual void TexStageCombine(eGDTextureStageCombineOperandType gdParamType, eGDBlend gdBlend) = 0; // e0
	virtual void TexStageCombine(eGDTextureStageCombineScaleParamType gdParamType, eGDTextureStageCombineScaleParam gdParam) = 0; // d8

	virtual void SetTexture(uint32_t, uint32_t) = 0;
	virtual intptr_t GetTexture(uint32_t) = 0;
	virtual intptr_t CreateTexture(uint32_t gdInternalTexFormat, uint32_t, uint32_t, uint32_t, uint32_t gdTexHintFlags) = 0;
	virtual void LoadTextureLevel(uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, uint32_t, void const*) = 0;
	virtual void SetCombiner(cGDCombiner const& combiner, uint32_t) = 0;

	virtual uint32_t CountVideoModes(void) const = 0; // 0xfc
	virtual void GetVideoModeInfo(uint32_t, sGDMode& gdMode) = 0;
	virtual void GetVideoModeInfo(sGDMode& gdMode) = 0;
	virtual void SetVideoMode(int32_t, void*, bool, bool) = 0;

	virtual void PolygonOffset(int32_t) = 0;

	virtual void BitBlt(int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*) = 0; // 110
	virtual void StretchBlt(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*) = 0; // 114
	virtual void BitBltAlpha(int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*, uint32_t) = 0;
	virtual void StretchBltAlpha(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*, uint32_t) = 0;
	virtual void BitBltAlphaModulate(int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*, uint32_t) = 0; // 120
	virtual void StretchBltAlphaModulate(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*, uint32_t) = 0;

	virtual void SetViewport(void) = 0;
	virtual void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height) = 0;
	virtual void GetViewport(int32_t dimensions[4]) = 0;

	virtual char const* GetDriverInfo(void) const = 0;
	virtual uint32_t GetGZCLSID(void) const = 0;

	virtual bool Init(void) = 0; // 0x13c
	virtual bool Shutdown(void) = 0;
	virtual bool IsDeviceReady(void) = 0;
	virtual bool Punt(uint32_t, void*) = 0;
};