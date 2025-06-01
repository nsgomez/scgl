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

static const uint32_t GZIID_cIGZGBufferRegionExtension = 0x669565fe;

/**
 * A graphics extension exposing functionality from GL_KTX_buffer_region.
 * 
 * Buffer regions enable the game to read data from an OpenGL framebuffer (i.e. the front,
 * back, stencil, and z-buffers), copy them to an offscreen area, and redraw them at a
 * later time.
 * 
 * SimCity 4 uses this to avoid redrawing the entire scene on every frame.
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