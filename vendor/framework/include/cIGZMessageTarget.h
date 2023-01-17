#pragma once
#include "cIGZUnknown.h"

class cGZMessage;

/**
* @brief A message target that accepts cGZMessage messages
* @see cIGZMessageServer::AddNotification(cIGZMessageTarget* pTarget, uint32_t dwMessageID)
*/
class cIGZMessageTarget : public cIGZUnknown
{
	public:
		virtual bool DoMessage(cGZMessage& sMessage) = 0;
};