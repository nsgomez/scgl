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