#pragma once
#include <stdint.h>

struct sGDMode
{
	uint32_t index;  // 0
	uint32_t width;  // 4
	uint32_t height; // 8
	uint32_t depth;  // c // dwRGBBitCount in DDSURFACEDESC2 parlance
	uint32_t __unknown1[4];            // 10, 14, 18, 1c
	bool isFullscreen;                 // 20
	bool __unknown2;                   // 21
	bool supportsHardwareAcceleration; // 22
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

/*
	uint32_t index;  // 0
	uint32_t width;  // 4
	uint32_t height; // 8
	uint32_t depth;  // c // dwRGBBitCount in DDSURFACEDESC2 parlance
	uint32_t __unknown1[4]; // 10, 14, 18, 1c
	alignas(2) bool fullscreen; // 20
	alignas(2) bool is3DAccelerated; // 22
	uint32_t __unknown3[3]; // 24, 28, 2c, 30
	//void* _unknownFuncPtr; // 34
*/

/*
#pragma once
#include <stdint.h>

struct sGDMode
{
	uint32_t videoModeId;
	uint32_t height;
	uint32_t width;
	uint32_t depth;
	uint8_t __unk_pad_1[20];
	uint8_t __unknown1;
	uint8_t __unk_pad_2;
	uint8_t __unknown2;
	uint8_t __unk_pad_3[11];
	uint32_t __unknown3;
};
*/