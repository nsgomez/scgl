#include "../include/cIGZCOM.h"
#include "../include/cIGZString.h"
#include "../include/cRZCOMDllDirector.h"
#include <assert.h>
#include <cstdlib>
#include <cstring>

#if defined(_WIN32)
#define EXPORT __declspec(dllexport)
#elif defined(__APPLE__)
#define EXPORT __attribute__((visibility("default")))
#endif

extern "C" EXPORT cIGZCOMDirector* GZDllGetGZCOMDirector(void) {
	return static_cast<cIGZCOMDirector*>(RZGetCOMDllDirector());
}

cIGZCOM* GZCOM(void) {
	return RZGetCOMDllDirector()->GZCOM();
}

cRZCOMSlimDllDirector::cRZCOMSlimDllDirector(uint32_t classId, FactoryFunctionPtr2 classFactory)
	: mnRefCount(0),
	  mDirectorID(0),
	  msLibraryPath(nullptr),
	  mpCOM(nullptr),
	  mpFrameWork(nullptr),
	  classId(classId),
	  classFactory(classFactory)
{
	// Empty
}

cRZCOMSlimDllDirector::~cRZCOMSlimDllDirector(void) {
	if (msLibraryPath != nullptr) {
		free(msLibraryPath);
	}
}

bool cRZCOMSlimDllDirector::QueryInterface(uint32_t riid, void** ppvObj) {
	switch (riid) {
		case kGZIID_cIGZCOMDirector:
			*ppvObj = static_cast<cIGZCOMDirector*>(this);
			AddRef();
			return true;

		case GZIID_cIGZFrameWorkHooks:
			*ppvObj = static_cast<cIGZFrameWorkHooks*>(this);
			AddRef();
			return true;

		case GZIID_cIGZUnknown:
			*ppvObj = static_cast<cIGZUnknown*>(static_cast<cIGZCOMDirector*>(this));
			AddRef();
			return true;
	}

	return false;
}

cIGZFrameWork* cRZCOMSlimDllDirector::FrameWork() {
	return mpFrameWork;
}

uint32_t cRZCOMSlimDllDirector::AddRef() {
	return ++mnRefCount;
}

uint32_t cRZCOMSlimDllDirector::Release() {
	return RemoveRef();
}

uint32_t cRZCOMSlimDllDirector::RemoveRef() {
	assert(mnRefCount > 0);
	if (mnRefCount > 0) {
		--mnRefCount;
	}

	return mnRefCount;
}

uint32_t cRZCOMSlimDllDirector::RefCount() {
	return mnRefCount;
}

bool cRZCOMSlimDllDirector::InitializeCOM(cIGZCOM* pCOM, const cIGZString& sLibraryPath) {
	if (pCOM != nullptr) {
		mpCOM = pCOM;
		mpFrameWork = pCOM->FrameWork();
		msLibraryPath = _strdup(sLibraryPath.Data());

		return true;
	}

	return false;
}

bool cRZCOMSlimDllDirector::OnStart(cIGZCOM* pCOM) {
	return true;
}

bool cRZCOMSlimDllDirector::GetClassObject(uint32_t clsid, uint32_t iid, void** ppvObj) {
	if (this->classId != clsid) {
		return false;
	}

	return classFactory(iid, ppvObj);
}

void cRZCOMSlimDllDirector::EnumClassObjects(ClassObjectEnumerationCallback pCallback, void* pContext) {
	pCallback(classId, 0, pContext);
}

bool cRZCOMSlimDllDirector::GetLibraryPath(cIGZString& sLibraryPath) {
	sLibraryPath.FromChar(msLibraryPath);
	return true;
}

void cRZCOMSlimDllDirector::AddDirector(cIGZCOMDirector* pDirector) {
	// Not implemented
}

bool cRZCOMSlimDllDirector::CanUnloadNow() {
	return true;
}

uint32_t cRZCOMSlimDllDirector::GetHeapAllocatedSize(void) {
	return 0;
}

cIGZCOM* cRZCOMSlimDllDirector::GZCOM() {
	return mpCOM;
}

bool cRZCOMSlimDllDirector::PreFrameWorkInit() { return true; }
bool cRZCOMSlimDllDirector::PreAppInit() { return true; }
bool cRZCOMSlimDllDirector::PostAppInit() { return true; }
bool cRZCOMSlimDllDirector::PreAppShutdown() { return true; }
bool cRZCOMSlimDllDirector::PostAppShutdown() { return true; }
bool cRZCOMSlimDllDirector::PostSystemServiceShutdown() { return true; }
bool cRZCOMSlimDllDirector::AbortiveQuit() { return true; }
bool cRZCOMSlimDllDirector::OnInstall() { return true; }