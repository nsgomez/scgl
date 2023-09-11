#include "VertexFormatUtils.h"

uint32_t RZMakeVertexFormat(uint32_t gdVertexFormat) {
	// nRZSimGL::PackStandardVertexFormat(eGDVertexFormat) -> eGDVertexFormat
	static uint32_t formatToPackedFormatMap[] = {
		0x00000000, 0x80000101, 0x80004001, 0x80008001,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x80004101, 0x80008101,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x80000001, 0x80000041, 0x80000141, 0x80004041,
		0x80008041, 0x80004141, 0x80008141,
	};

	return formatToPackedFormatMap[gdVertexFormat];
}

uint32_t RZVertexFormatStride(uint32_t gdVertexFormat) {
	switch (gdVertexFormat) {
	case kGDVertexFormat_V3F_C4UB:
		return 16;
	case kGDVertexFormat_V3F_T2F:
		return 20;
	case kGDVertexFormat_V3F_2T2F:
	case kGDVertexFormat_V3F_N3F_C4UB:
		return 28;
	case kGDVertexFormat_V3F_C4UB_T2F:
	case kGDVertexFormat_V3F_N3F:
		return 24;
	case kGDVertexFormat_V3F_C4UB_2T2F:
	case kGDVertexFormat_V3F_N3F_T2F:
		return 32;
	case kGDVertexFormat_V3F:
		return 12;
	case kGDVertexFormat_V3F_N3F_2T2F:
		return 40;
	case kGDVertexFormat_V3F_N3F_C4UB_T2F:
		return 36;
	case kGDVertexFormat_V3F_N3F_C4UB_2T2F:
		return 44;
	}

	if (gdVertexFormat < 0x80000000) {
		gdVertexFormat = RZMakeVertexFormat(gdVertexFormat);
	}

	gdVertexFormat &= 0x7fffffff;
	uint32_t stride = ((gdVertexFormat >> 10) & 0xf)
		+ (((gdVertexFormat >> 14) & 0xf) * 2)
		+ ((gdVertexFormat >> 8) & 0x3)
		+ ((gdVertexFormat & 3) * 3);

	stride *= 4;
	if ((gdVertexFormat & 0xf) != 0) {
		uint32_t addlStride = ((gdVertexFormat >> 18) & 0xf)
			+ (((gdVertexFormat >> 6) & 1) * 3)
			+ (((gdVertexFormat >> 22) & 0xf) * 4)
			+ ((gdVertexFormat >> 2) & 7)
			+ ((gdVertexFormat >> 7) & 1)
			+ ((gdVertexFormat >> 5) & 1);

		addlStride *= 4;
		stride += addlStride;
	}

	return stride;
}

uint32_t RZVertexFormatElementOffset(uint32_t gdVertexFormat, uint32_t gdElementType, uint32_t count) {
	static uint32_t elementTypeOffsetMap[] = { 12, 4, 4, 12, 4, 4, 4, 8, 12, 16 };
	static uint32_t elementTypeVertexFormatMap[] = { 0xf, 0x0, 0x3, 0x1f, 0x3f, 0x7f, 0xff, 0x3ff, 0x3fff, 0x3ffff };

	uint32_t offset = elementTypeOffsetMap[gdElementType] * count;
	if (gdVertexFormat < 0x80000000) {
		gdVertexFormat = RZMakeVertexFormat(gdVertexFormat);
	}

	if (gdElementType > 0) {
		offset += RZVertexFormatStride((elementTypeVertexFormatMap[gdElementType] | 0x80000000) & gdVertexFormat);
	}

	return offset;
}

uint32_t RZVertexFormatNumElements(uint32_t gdVertexFormat, uint32_t gdElementType) {
	static uint32_t elementTypeShiftFactor[] = { 0, 2, 5, 6, 7, 8, 10, 14, 18, 22 };
	static uint32_t elementTypeMask[] = { 0x3, 0x7, 0x1, 0x1, 0x1, 0x3, 0xf, 0xf, 0xf, 0xf };

	if (gdVertexFormat < 0x80000000) {
		gdVertexFormat = RZMakeVertexFormat(gdVertexFormat);
	}

	return (gdVertexFormat >> elementTypeShiftFactor[gdElementType]) & elementTypeMask[gdElementType];
}