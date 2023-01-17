#pragma once
#include <cIGZUnknown.h>
#include "../cIGZBuffer.h"

static const uint32_t GZIID_cIGZGSnapshotExtension = 0xe69bfe2a;

/**
 * A graphics extension for taking screenshots.
 * 
 * Screenshots are implemented by copying the color buffer (or front buffer)
 * from graphics memory to game memory.
 * 
 * Even though this is an extension, it is MANDATORY for SimCity 4.
 * Failing to implement this extension will cause the game to crash on load.
 */
class cIGZGSnapshotExtension : public cIGZUnknown
{
public:
	virtual cIGZBuffer* CopyColorBuffer(int32_t, int32_t, int32_t, int32_t, cIGZBuffer*) = 0;
};