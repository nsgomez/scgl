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
#include <stdint.h>

struct sGDMode
{
	uint32_t index;                    // 0
	uint32_t width;                    // 4
	uint32_t height;                   // 8
	uint32_t depth;                    // c // dwRGBBitCount in DDSURFACEDESC2 parlance
	uint32_t alphaColorMask;           // 10
	uint32_t redColorMask;             // 14
	uint32_t greenColorMask;           // 18
	uint32_t blueColorMask;            // 1c
	bool isFullscreen;                 // 20
	bool supportsStencilBuffer;        // 21
	uint8_t __unknown2;                // 22
	uint8_t __unknown3;                // 23
	uint32_t textureStageCount;        // 24
	bool supportsMultitexture;         // 28 // GL_ARB_multitexture
	bool supportsTextureEnvCombine;    // 29 // GL_EXT_texture_env_combine
	bool supportsNvTextureEnvCombine4; // 2a // GL_NV_texture_env_combine4
	bool supportsFogCoord;             // 2b // GL_EXT_fog_coord
	bool supportsDxtTextures;          // 2c // GL_EXT_texture_compression_s3tc
	uint8_t __unknown5[3];             // 2d, 2e, 2f
	bool isInitialized;                // 30
	                                   // 34 (end of struct)
};
