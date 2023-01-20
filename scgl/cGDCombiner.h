#pragma once
#include "cIGZGDriver.h"

class cGDCombiner
{
public:
	struct ParamOperandPair
	{
		uint8_t SourceType : 4;
		uint8_t OperandType : 4;
	};

public:
	uint8_t RGBCombineMode;
	ParamOperandPair RGBParams[3];

	uint8_t AlphaCombineMode;
	ParamOperandPair AlphaParams[3];

	uint8_t RGBScale;
	uint8_t AlphaScale;
};