#pragma once
#include "cIGZUnknown.h"

class cIGZCommandParameterSet;

class cIGZCommandDispatcher : public cIGZUnknown
{
	public:
		virtual bool Init(void) = 0;
		virtual bool Shutdown(void) = 0;

		virtual bool ExecuteCommand(uint32_t dwCommandID, cIGZCommandParameterSet* pParamUnknown, cIGZCommandParameterSet* pCommandParams) = 0;

		virtual ~cIGZCommandDispatcher() { }
};