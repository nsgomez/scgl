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
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include "cGDriver.h"

namespace nSCGL
{
	char const* cGDriver::GetDriverInfo(void) const {
		return driverInfo.c_str();
	}

	uint32_t cGDriver::GetGZCLSID(void) const {
		return kSCGLGDriverGZCLSID;
	}

	uint32_t cGDriver::GetError(void) {
		DriverError retError = lastError;
		lastError = DriverError::OK;
		return static_cast<uint32_t>(retError);
	}

	void cGDriver::SetLastError(DriverError err) {
		lastError = err;
	}
}