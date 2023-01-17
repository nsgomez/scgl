#pragma once
#include "cIGZMessage2.h"

class cRZPoint;

/**
 * @brief A type of message sent upon mouse events (i.e. movement)
 */
class cIGZMouseMessage : public cIGZMessage2
{
	public:
		virtual uint32_t Initialize(uint32_t dwMouseEventID, cRZPoint const& sPosition, uint32_t dwKeyFlags, int32_t lWheelDelta) = 0;

		virtual uint32_t EventType(void) const = 0;
		virtual cRZPoint Position(void) const = 0;
		virtual int32_t WheelDelta(void) const = 0;
		virtual uint32_t KeyFlags(void) const = 0;
};