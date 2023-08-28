#include "cGDriver.h"
#include "GLSupport.h"

namespace nSCGL
{
	bool cGDriver::QueryInterface(uint32_t riid, void** ppvObj) {
		switch (riid)
		{
		case GZIID_cIGZUnknown:
		case GZIID_cIGZGDriver:
#ifndef NDEBUG
			MessageBoxA(NULL, "SCGL has arrived - attach your debugger now.", "cIGZGDriver - cGDriver::QueryInterface", 0);
#endif
			*ppvObj = static_cast<cIGZGDriver*>(this);
			break;

		case GZIID_cIGZGBufferRegionExtension:
			if (wglCreateBufferRegionARB == nullptr) {
				return false;
			}

			*ppvObj = static_cast<cIGZGBufferRegionExtension*>(this);
			break;

		case GZIID_cIGZGDriverLightingExtension:
			*ppvObj = static_cast<cIGZGDriverLightingExtension*>(this);
			break;

		/*case GZIID_cIGZGDriverVertexBufferExtension:
			MessageBoxA(NULL, "cIGZGDriverVertexBufferExtension", "cGDriver::QueryInterface", 0);
			*ppvObj = static_cast<cIGZGDriverVertexBufferExtension*>(this);
			break;*/

		case GZIID_cIGZGSnapshotExtension:
			*ppvObj = static_cast<cIGZGSnapshotExtension*>(this);
			break;

		default:
			//sprintf_s(buf, "%x", riid);
			//MessageBoxA(NULL, buf, "Unknown interface ID in GDriver", MB_ICONERROR);
			return false;
		}

		AddRef();
		return true;
	}

	uint32_t cGDriver::AddRef(void) {
		return cRZRefCount::AddRef();
	}

	uint32_t cGDriver::Release(void) {
		return cRZRefCount::Release();
	}

	bool cGDriver::FinalRelease(void) {
		NOTIMPL();
		return false;
	}
}