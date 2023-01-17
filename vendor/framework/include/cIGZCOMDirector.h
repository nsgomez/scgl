#pragma once
#include "cIGZUnknown.h"

class cIGZCOM;
class cIGZFrameWork;
class cIGZString;
class cRZCOMDllDirector;

/**
 * @brief A base interface for COM directors
 *
 * COM directors are classes that can be dynamically loaded and unloaded, and
 * provide an interface for hotpluggable code to interact with the game core.
 * Directors also be nested. Any GZCOM-compatible library must provide a
 * director for the game to load.
 */
class cIGZCOMDirector : public cIGZUnknown
{
	public:
		typedef void(*ClassObjectEnumerationCallback)(uint32_t clsid, uint32_t version, void* pContext);

		/**
		 * @brief Initializes the director with the given parameters
		 *
		 * When initializing, the COM director should store the COM reference
		 * and library path in member variables, and should initialize any
		 * child directors as well.
		 */
		virtual bool InitializeCOM(cIGZCOM* pCOM, const cIGZString& sLibraryPath) = 0;

		/**
		 * @brief A callback for after directors are loaded on game start
		 * @return Always true. The effects of returning false are unknown.
		 */
		virtual bool OnStart(cIGZCOM* pCOM) = 0;

		/**
		 * @brief Iterates over the director's class objects with the callback
		 * @param pCallback The function to use when iterating over class IDs
		 * @param pContext Arbitrary data to pass as a void pointer
		 */
		virtual void EnumClassObjects(ClassObjectEnumerationCallback pCallback, void* pContext) = 0;

		/**
		 * @see cIGZCOM::GetClassObject(uint32_t clsid, uint32_t iid, void** ppvObj)
		 */
		virtual bool GetClassObject(uint32_t rclsid, uint32_t riid, void** ppvObj) = 0;

		/**
		 * @brief Signals to the game if it is safe for the director to unload
		 */
		virtual bool CanUnloadNow(void) = 0;

		/**
		 * @brief A callback upon this director being unloaded
		 */
		virtual bool OnUnload(void) = 0;

		/**
		 * @return The number of references to this director
		 */
		virtual uint32_t RefCount(void) = 0;

		/**
		 * @see cIGZUnknown::Release
		 */
		virtual uint32_t RemoveRef(void) = 0;

		/**
		 * @return A reference to the GZCOM framework
		 */
		virtual cIGZFrameWork* FrameWork(void) = 0;

		/**
		 * @return A reference to the GZCOM itself
		 */
		virtual cIGZCOM* GZCOM(void) = 0;
		
		/**
		 * @brief Adds a child director to this director
		 *
		 * Child directors should be added prior to initialization to ensure
		 * that the child directors are also properly initialized.
		 */
		virtual void AddDirector(cIGZCOMDirector* pCOMDirector) = 0;

		/**
		 * @brief Stores the path to this library in the given parameter
		 * @param sLibraryPath A reference to the string to store the path in
		 * @return Whether the library path was successfully stored
		 */
		virtual bool GetLibraryPath(cIGZString& sLibraryPath) = 0;

		/**
		 * @return The number of bytes this director allocated on the heap
		 *
		 * When performing heap allocations such as creating class objects or using
		 * the new keyword or malloc, the director should note it and update this
		 * value. Directors that only use the stack can and should return zero.
		 */
		virtual uint32_t GetHeapAllocatedSize(void) = 0;
};