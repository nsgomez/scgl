#pragma once
#include "cIGZCOMLibrary.h"
#include "cRZBaseString.h"
#include <string>

class cIGZCOMDirector;

/**
 * @brief Internal representation of a GZCOM library
 *
 * Used by the game itself when loading GZCOM libraries when trying to load
 * them and keeping track of directors that were successfully loaded by the
 * game.
 * 
 * Custom directors should NOT inherit from this class. However, it can be
 * used when iterating over the libraries that have been loaded via cIGZCOM.
 */
class cGZCOMLibrary : public cIGZCOMLibrary
{
	public:
		typedef void* GZLibraryHandle;

		cGZCOMLibrary(void);
		cGZCOMLibrary(const cIGZString& sLibraryPath);
		virtual ~cGZCOMLibrary(void);

		bool QueryInterface(uint32_t riid, void** ppvObj);
		uint32_t AddRef(void);
		uint32_t Release(void);

		bool Load(void);
		bool Free(void);

		void GetPath(cIGZString& sPath) const;
		bool SetPath(const cIGZString& sPath);

		/**
		 * Gets the OS-specific handle for this library.
		 *
		 * On Windows, this will be an HMODULE. On UNIX types, this will be an
		 * uninterpretable void pointer.
		 */
		GZLibraryHandle GetHandle() const;
		void SetHandle(GZLibraryHandle handle);

		cIGZCOMDirector* GetDirector(void) const;
		void SetDirector(cIGZCOMDirector* pDirector);

		bool IsLoaded() const;
		void SetLoaded(bool bUse);

		/**
		 * Compares libraries by filename using std::string::compare
		 * @param rhs A reference to the library to compare against
		 * @return Whether this library's filename precedes the other's
		 */
		bool operator< (const cGZCOMLibrary& rhs) const;

		/**
		 * @see operator<(const cGZCOMLibrary& rhs)
		 */
		bool operator> (const cGZCOMLibrary& rhs) const;

		/**
		 * @see operator<(const cGZCOMLibrary& rhs)
		 */
		bool operator== (const cGZCOMLibrary& rhs) const;

	protected:
		bool mbLoaded;
		uint32_t mnRefCount;
		cIGZCOMDirector* mpDirector;
		cRZBaseString msLibraryPath;
		GZLibraryHandle mHandle;
};