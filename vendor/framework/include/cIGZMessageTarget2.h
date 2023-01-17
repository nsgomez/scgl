#pragma once
#include "cIGZUnknown.h"

class cIGZMessage2;

/**
 * @brief A message target that accepts cIGZMessage2 messages
 * @see cIGZMessageTarget
 * @see cIGZMessageServer2::AddNotification(cIGZMessageTarget2* pTarget, uint32_t dwMessageID)
 */
class cIGZMessageTarget2 : public cIGZUnknown
{
	public:
		virtual bool DoMessage(cIGZMessage2* pMessage) = 0;
};