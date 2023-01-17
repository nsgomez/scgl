#pragma once
#include "cIGZUnknown.h"

/**
 * @brief A service that manages allocations via the game's memory pool
 */
class cIGZAllocatorService : public cIGZUnknown
{
	public:
		virtual void* Allocate(uint32_t dwSize) = 0;
		virtual bool Deallocate(void* pData) = 0;
		virtual void* Reallocate(void* pData, uint32_t dwNewSize) = 0;
};