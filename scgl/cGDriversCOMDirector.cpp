#include <cIGZFrameWork.h>
#include <cRZCOMDllDirector.h>
#include "cGDriver.h"

namespace nSCGL
{
	static const uint32_t kSCVKGDriverPluginCOMDirectorID = 0xCB6EC543;

	class cGDriversCOMDirector : public cRZCOMSlimDllDirector
	{
	public:
		cGDriversCOMDirector() :
			cRZCOMSlimDllDirector(cGDriver::kSCGLGDriverGZCLSID, cGDriver::FactoryFunctionPtr2)
		{
		}

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

		void EnumClassObjects(ClassObjectEnumerationCallback pCallback, void* pContext) {
			pCallback(classId, 1000000, pContext);
		}
	};
}

cRZCOMSlimDllDirector* RZGetCOMDllDirector() {
	static nSCGL::cGDriversCOMDirector sDirector;
	return &sDirector;
}