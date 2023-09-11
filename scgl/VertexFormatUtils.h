#pragma once
#include <cstdint>

enum eGDVertexFormat
{
	kGDVertexFormat_V3F_C4UB = 1,
	kGDVertexFormat_V3F_T2F = 2,
	kGDVertexFormat_V3F_2T2F = 3,
	kGDVertexFormat_V3F_C4UB_T2F = 10,
	kGDVertexFormat_V3F_C4UB_2T2F = 11,
	kGDVertexFormat_V3F = 32,
	kGDVertexFormat_V3F_N3F = 33,
	kGDVertexFormat_V3F_N3F_C4UB = 34,
	kGDVertexFormat_V3F_N3F_T2F = 35,
	kGDVertexFormat_V3F_N3F_2T2F = 36,
	kGDVertexFormat_V3F_N3F_C4UB_T2F = 37,
	kGDVertexFormat_V3F_N3F_C4UB_2T2F = 38,
};

enum eGDElementType
{
	kGDElementType_Vertices,
	kGDElementType_Normal = 3,
	kGDElementType_Color = 5,
	kGDElementType_TexCoord = 7,
};

uint32_t RZMakeVertexFormat(uint32_t gdVertexFormat);
uint32_t RZVertexFormatStride(uint32_t gdVertexFormat);
uint32_t RZVertexFormatElementOffset(uint32_t gdVertexFormat, uint32_t gdElementType, uint32_t count);
uint32_t RZVertexFormatNumElements(uint32_t gdVertexFormat, uint32_t gdElementType);