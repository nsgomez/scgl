#pragma once
#include "GLSupport.h"
#include <stdint.h>

struct GLShareableState
{
public:
	GLShareableState();

public:
	uint32_t interleavedFormat;
	uint32_t interleavedStride;
	void const* interleavedPointer;
	uint8_t activeMatrixMode;
	uint8_t glActiveTextureUnit;
};