#pragma once
#include "cIGZUnknown.h"

class cIGZCOMDirector;
class cIGZString;

/**
 * @brief The public interface for internally represented COM libraries
 * @see cGZCOMLibrary
 *
 * Unlike cGZCOMLibrary, this virtual interface is supposed to represent the
 * methods that other directors are allowed to access.
 */
class cIGZCOMLibrary : public cIGZUnknown
{
	public:
		virtual bool GetPath(cIGZString& szCopyTo) = 0;
		virtual cIGZCOMDirector* GetDirector(void) = 0;

		virtual uintptr_t GetHandle(void) = 0;
		virtual bool IsLoaded(void) const = 0;
};