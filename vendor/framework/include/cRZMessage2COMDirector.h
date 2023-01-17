#pragma once
#include "cIGZMessageTarget2.h"
#include "cRZCOMDllDirector.h"
#include "GZCLSIDDefs.h"

class cRZMessage2COMDirector : public cRZCOMDllDirector, public cIGZMessageTarget2
{
	public:
		bool QueryInterface(uint32_t riid, void** ppvObj) {
			if (riid == GZCLSID::kcIGZMessageTarget2) {
				*ppvObj = static_cast<cIGZMessageTarget2*>(this);
				AddRef();
				return true;
			}
			else {
				return cRZMessage2COMDirector::QueryInterface(riid, ppvObj);
			}
		}

		uint32_t AddRef(void) {
			return cRZCOMDllDirector::AddRef();
		}

		uint32_t Release(void) {
			return cRZCOMDllDirector::Release();
		}
};