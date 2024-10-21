#pragma once
#include <stdint.h>

class cRZRect
{
	public:
		union
		{
			int32_t nX;
			float fX;
		};

		union
		{
			int32_t nY;
			float fY;
		};

		union
		{
			int32_t nWidth;
			float fWidth;
		};

		union
		{
			int32_t nHeight;
			float fHeight;
		};
};