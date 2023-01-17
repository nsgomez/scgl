#pragma once
#include "cIGZUnknown.h"

/**
 * @brief An interface for framework hook event subscribers
 */
class cIGZFrameWorkHooks : public cIGZUnknown
{
	public:
		virtual bool PreFrameWorkInit(void) = 0;
		virtual bool PreAppInit(void) = 0;
		virtual bool PostAppInit(void) = 0;
		virtual bool PreAppShutdown(void) = 0;
		virtual bool PostAppShutdown(void) = 0;
		virtual bool PostSystemServiceShutdown(void) = 0;
		virtual bool AbortiveQuit(void) = 0;
		virtual bool OnInstall(void) = 0;
};