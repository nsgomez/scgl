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