#pragma once
#include <cIGZString.h>
#include <cIGZUnknown.h>

static const uint32_t GZIID_cIGZGBufferRegionExtension = 0x669565fe;

/**
 * A graphics extension exposing functionality from GL_KTX_buffer_region.
 * 
 * Buffer regions enable the game to read data from an OpenGL framebuffer (i.e. the front,
 * back, stencil, and z-buffers), copy them to an offscreen area, and redraw them at a
 * later time.
 * 
 * SimCity 4 uses this to move the camera without redrawing everything on the screen
 * on every frame.
 */
class cIGZGBufferRegionExtension : public cIGZUnknown
{
public:
	virtual bool BufferRegionEnabled(void) = 0;
	virtual uint32_t NewBufferRegion(int32_t gdBufferRegionType) = 0;
	virtual bool DeleteBufferRegion(int32_t bufferRegion) = 0;
	virtual bool ReadBufferRegion(uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) = 0;
	virtual bool DrawBufferRegion(uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) = 0;
	virtual bool IsBufferRegion(uint32_t bufferRegion) = 0;
	virtual bool CanDoPartialRegionWrites(void) = 0;
	virtual bool CanDoOffsetReads(void) = 0;

	virtual ~cIGZGBufferRegionExtension(void) = 0;

	virtual bool Init(void) = 0;
	virtual bool Shutdown(void) = 0;
	virtual bool FinalRelease(void) = 0;
	virtual bool DeleteAllBufferRegions(void) = 0;
};