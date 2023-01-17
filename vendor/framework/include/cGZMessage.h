#pragma once
#include <stdint.h>

/**
 * @brief A primitive type of event message
 *
 * This class is a very primitive message that, when paired with a
 * cIGZMessageServer, allows directors to subscribe to events emitted by other
 * directors and acquire any relevant data from those events.
 
 * This appears to be rarely used, with cIGZMessage2 and cIGZMessage2Standard
 * being preferred due to their extensibility and use of interfaces such as
 * cIGZUnknown, which allows for easy transformation into different message
 * type subsets.
 */
class cGZMessage
{
	public:
		/**
		 * A constructor that initializes a message type and sets data to zero
		 * @param dwMessageType The type to assign initially to the message
		 */
		cGZMessage(uint32_t dwMessageType) {
			this->dwMessageType = dwMessageType;
			this->dwData1 = 0;
			this->dwData2 = 0;
			this->dwData3 = 0;
		}

		uint32_t dwMessageType;
		uint32_t dwData1;
		uint32_t dwData2;
		uint32_t dwData3;
};