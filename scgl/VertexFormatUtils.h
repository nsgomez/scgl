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