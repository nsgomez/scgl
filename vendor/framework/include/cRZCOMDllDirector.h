#pragma once
#include "cIGZCOMDirector.h"
#include "cIGZFrameWorkHooks.h"

class cRZCOMSlimDllDirector : public cIGZCOMDirector, public cIGZFrameWorkHooks
{
	public:
		typedef void (*DummyFunctionPtr)();
		typedef cIGZUnknown* (*FactoryFunctionPtr1)();
		typedef bool (*FactoryFunctionPtr2)(uint32_t, void**);

	public:
		cRZCOMSlimDllDirector(uint32_t classId, FactoryFunctionPtr2 classFactory);
		virtual ~cRZCOMSlimDllDirector(void);

	public:
		virtual uint32_t GetDirectorID(void) const = 0;

		virtual bool QueryInterface(uint32_t riid, void** ppvObj);
		virtual uint32_t AddRef(void);
		virtual uint32_t Release(void);

		virtual uint32_t RemoveRef(void);
		virtual uint32_t RefCount(void);

	public:
		bool InitializeCOM(cIGZCOM* pCOM, const cIGZString& sLibraryPath);
		bool OnStart(cIGZCOM* pCOM);
		bool GetLibraryPath(cIGZString& sLibraryPath);

		virtual bool GetClassObject(uint32_t rclsid, uint32_t riid, void** ppvObj);
		bool OnUnload(void) { return true; }
		cIGZFrameWork* FrameWork(void);
		cIGZCOM* GZCOM(void);
		void EnumClassObjects(ClassObjectEnumerationCallback pCallback, void* pContext);
		bool CanUnloadNow(void);
		void AddDirector(cIGZCOMDirector* pCOMDirector);
		uint32_t GetHeapAllocatedSize(void);

	public:
		bool PreFrameWorkInit(void);
		bool PreAppInit(void);
		bool PostAppInit(void);
		bool PreAppShutdown(void);
		bool PostAppShutdown(void);
		bool PostSystemServiceShutdown(void);
		bool AbortiveQuit(void);
		bool OnInstall(void);

	protected:
		enum FactorFunctionType {
			kFactorFunctionType1 = 1,
			kFactorFunctionType2 = 2
		};

		enum GZIIDList {
			GZIID_cIGZFrameWorkHooks = 0x03FA40BF,
			kGZIID_cIGZCOMDirector = 0xA21EE941
		};

	protected:
		uint32_t mnRefCount;
		uint32_t mDirectorID;
		char* msLibraryPath;
		cIGZCOM* mpCOM;
		cIGZFrameWork* mpFrameWork;

		uint32_t classId;
		FactoryFunctionPtr2 classFactory;
};

cRZCOMSlimDllDirector* RZGetCOMDllDirector();
inline cIGZFrameWork* RZGetFrameWork() { return RZGetCOMDllDirector()->FrameWork(); }
inline cIGZFrameWork* RZGetFramework() { return RZGetCOMDllDirector()->FrameWork(); }