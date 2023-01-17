#pragma once
#include <cIGZUnknown.h>

static const uint32_t GZIID_cIGZGDriverVertexBufferExtension = 0x09cd86f9;

/**
 * A graphics extension exposing functionality from GL_ARB_vertex_buffer_object.
 * 
 * Allows the game to send "buffer objects" to the graphics card, where they
 * can be cached in graphics memory to draw geometry faster.
 */
class cIGZGDriverVertexBufferExtension : public cIGZUnknown
{
public:
	virtual char const* GetVertexBufferName(uint32_t gdVertexFormat) = 0;
	virtual uint32_t VertexBufferType(uint32_t) = 0;
	virtual uint32_t MaxVertices(uint32_t) = 0;
	virtual uint32_t GetVertices(int32_t, bool) = 0;
	virtual uint32_t ContinueVertices(uint32_t, uint32_t) = 0;
	virtual void ReleaseVertices(uint32_t) = 0;
	virtual void DrawPrims(uint32_t, uint32_t gdPrimType, void*, uint32_t) = 0;
	virtual void DrawPrimsIndexed(uint32_t, uint32_t gdPrimType, uint32_t, uint16_t*, void*, uint32_t) = 0;
	virtual void Reset(void) = 0;

	virtual ~cIGZGDriverVertexBufferExtension(void) = 0;
};