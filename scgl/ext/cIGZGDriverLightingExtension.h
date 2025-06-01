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

static const uint32_t GZIID_cIGZGDriverLightingExtension = 0x87e2b87d;

/**
 * A graphics extension for lighting support and multiple light sources.
 * 
 * Not implemented by the software renderer, but practically every hardware
 * accelerated API will support this through the fixed pipeline or shaders.
 */
class cIGZGDriverLightingExtension : public cIGZUnknown
{
public:
	virtual void EnableLighting(bool) = 0;
	virtual void EnableLight(uint32_t, bool) = 0;
	virtual void LightModelAmbient(float, float, float, float) = 0;
	virtual void LightColor(uint32_t, uint32_t, float const*) = 0;
	virtual void LightColor(uint32_t, float const*, float const*, float const*) = 0;
	virtual void LightPosition(uint32_t, float const*) = 0;
	virtual void LightDirection(uint32_t, float const*) = 0;
	virtual void MaterialColor(uint32_t, float const*) = 0;
	virtual void MaterialColor(float const*, float const*, float const*, float const*, float) = 0;
};