#pragma once
#include <cIGZUnknown.h>
#include <cRZRect.h>

static const uint32_t GZIID_cIGZBuffer = 0x20732180;

class cIGZBufferHardwareCache;

class cGZBufferColorType
{
public:
	enum eColorType
	{
		Null,
		AutoFromScreenFormat,
		Unknown_RGB8,
		AutoFromScreenFormat16,
		R5G5B5 = 4,
		R5G6B5 = 5,
		A1R5G5B5 = 6,
		A4R4G4B4 = 7,
		R8G8B8 = 8,
		A8R8G8B8 = 9,
	};
};

class cIGZBuffer : public cIGZUnknown
{
public:
	enum eLockFlags {
		IsDirtyUpdate = 0x8000,
	};

public:
	virtual bool Init(uint32_t width, uint32_t height, cGZBufferColorType::eColorType bufferColorType, uint32_t colorDepthInBits) = 0;
	virtual bool Shutdown(void) = 0;
	virtual bool Uninitialize(void) = 0;

	virtual bool Lock(uint32_t lockFlags) = 0;
	virtual bool Unlock(uint32_t lockFlags) = 0;
	virtual bool IsLocked() const = 0;

	virtual uint32_t Width() const = 0;
	virtual uint32_t Height() const = 0;

	virtual uint32_t GetBufferArea() const = 0;
	virtual uint32_t GetBufferArea(cRZRect& rect) const = 0;
	virtual uint32_t GetMemoryUsage() const = 0;
	virtual cGZBufferColorType::eColorType GetColorType() const = 0;
	virtual uint8_t GetColorType(cGZBufferColorType::eColorType& bufferColorType) const = 0;
	virtual uint32_t GetBitsPerPixel() const = 0;
	virtual uint32_t GetBytesPerPixel() const = 0;

	virtual bool Fill(uint32_t color, cRZRect const& targetArea, cRZRect*) = 0;
	virtual bool Fill(uint32_t color, int32_t x, int32_t y, int32_t width, int32_t height, cRZRect*) = 0;
	virtual bool Fill(uint32_t color, cRZRect*) = 0;

	virtual uint32_t GetPixel(uint32_t x, uint32_t y) const = 0;
	virtual void SetPixel(uint32_t x, uint32_t y, uint32_t color) = 0;

	virtual void SetTransparency(uint32_t color) = 0;
	virtual uint32_t GetTransparency() const = 0;
	virtual bool IsPixelTransparent(uint32_t x, uint32_t y) = 0;
	virtual bool GetTransparentColor(uint32_t& color) const = 0;
	virtual void RemoveTransparency() = 0;

	virtual bool DrawLine(int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t) = 0;
	virtual bool Blt(cIGZBuffer* srcBuffer, cRZRect const& srcRect, cRZRect const& targetRect, cRZRect const*) = 0;

	virtual uint32_t ConvertRGBValueToNative(uint8_t red, uint8_t green, uint8_t blue) = 0;
	virtual bool ConvertNativeValueToRGB(uint32_t color, uint8_t& red, uint8_t& green, uint8_t& blue) = 0;
	virtual uint32_t ConvertRGBAValueToNative(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) = 0;
	virtual bool ConvertNativeValueToRGBA(uint32_t color, uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha) = 0;

	virtual uint32_t GetColorSurfaceBits() = 0;
	virtual uint32_t GetColorSurfaceStride() = 0;
	virtual uint32_t GetChangeCounter() = 0;

	virtual cIGZBufferHardwareCache* GetHardwareCache() = 0;
	virtual bool SetHardwareCache(cIGZBufferHardwareCache* cache) = 0;

	virtual bool IsReady() = 0;
};