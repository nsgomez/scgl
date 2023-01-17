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