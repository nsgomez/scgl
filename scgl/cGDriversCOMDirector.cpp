#include <cIGZFrameWork.h>
#include <cRZCOMDllDirector.h>
#include "cGDriver.h"

namespace nSCGL
{
	static const uint32_t kSCVKGDriverPluginCOMDirectorID = 0xCB6EC543;

	class cGDriversCOMDirector : public cRZCOMDllDirector
	{
	public:
		uint32_t GetDirectorID() const {
			return kSCVKGDriverPluginCOMDirectorID;
		}

		bool PreFrameWorkInit() {
			return true;
		}

		bool OnStart(cIGZCOM* pCOM) {
			cIGZFrameWork* const pFramework = RZGetFrameWork();
			if (pFramework) {
				if (pFramework->GetState() < cIGZFrameWork::kStatePreAppInit) {
					pFramework->AddHook(this);
				}
				else {
					PreAppInit();
				}
			}
			return true;
		}

		bool InitializeCOM(cIGZCOM* pCOM, const cIGZString& sLibraryPath) {
			AddCls(cGDriver::kSCGLGDriverGZCLSID, cGDriver::FactoryFunctionPtr2);
			return cRZCOMDllDirector::InitializeCOM(pCOM, sLibraryPath);
		}

		void EnumClassObjects(ClassObjectEnumerationCallback pCallback, void* pContext) {
			for (ChildDirectorArray::iterator it(mChildDirectorArray.begin()); it != mChildDirectorArray.end(); ++it) {
				cRZCOMDllDirector* const pDirector = *it;
				pDirector->EnumClassObjects(pCallback, pContext);
			}

			for (ClassObjectMap::iterator it2(mClassObjectMap.begin()); it2 != mClassObjectMap.end(); ++it2) {
				const uint32_t classID = (*it2).first;
				pCallback(classID, 1000000, pContext);
			}
		}
	};
}

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static nSCGL::cGDriversCOMDirector sDirector;
	return &sDirector;
}