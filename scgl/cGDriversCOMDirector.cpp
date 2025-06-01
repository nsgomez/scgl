/*
 *  SCGL - a free OpenGL driver for SimCity 4's SimGL interface
 *  Copyright (C) 2025  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

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