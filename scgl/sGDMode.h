#pragma once
#include <stdint.h>

struct sGDMode
{
	uint32_t index;  // 0
	uint32_t width;  // 4
	uint32_t height; // 8
	uint32_t depth;  // c // dwRGBBitCount in DDSURFACEDESC2 parlance
	uint32_t alphaColorMask;           // 10
	uint32_t redColorMask;             // 14
	uint32_t greenColorMask;           // 18
	uint32_t blueColorMask;            // 1c
	bool isFullscreen;                 // 20
	bool supportsStencilBuffer;        // 21
	bool __unknown2;                   // 22
	bool __unknown3;                   // 23
	bool supportsTextureCompression;   // 24 // GL_ARB_texture_compression
	bool __unknown4[3];                // 25, 26, 27
	bool supportsMultitexture;         // 28 // GL_ARB_multitexture
	bool supportsTextureEnvCombine;    // 29 // GL_EXT_texture_env_combine
	bool supportsNvTextureEnvCombine4; // 2a // GL_NV_texture_env_combine4
	bool supportsFogCoord;             // 2b // GL_EXT_fog_coord
	bool supportsDxtTextures;          // 2c // GL_EXT_texture_compression_s3tc
	bool __unknown5[3];                // 2d, 2e, 2f
	bool isInitialized;                // 30
	                                   // 34 (end of struct)
};
