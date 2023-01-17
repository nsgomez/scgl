#pragma once
#include "cIGZUnknown.h"
#include <cassert>

class cRZUnknown : public cIGZUnknown
{
	public:
		cRZUnknown() {
			mnRefCount = 0;
		}

		bool QueryInterface(uint32_t riid, void** ppvObj) override {
			if (riid == 1) {
				*ppvObj = static_cast<cIGZUnknown*>(this);
				AddRef();
				return true;
			}
			else {
				return false;
			}
		}

		uint32_t AddRef(void) override {
			return ++mnRefCount;
		}

		uint32_t Release(void) override {
			assert(mnRefCount > 0);
			if (mnRefCount > 0) {
				--mnRefCount;
			}

			return mnRefCount;
		}

	protected:
		uint32_t mnRefCount;
};