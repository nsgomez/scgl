#pragma once
#include "cIGZUnknown.h"
#include <list>

class cIGZApp;
class cIGZCOM;
class cIGZExceptionNotification;
class cIGZFrameWorkHooks;
class cIGZSystemService;

/**
 * @brief A root service for Gonzo applications
 *
 * Although the framework does not strictly act like a systme service, it is
 * the core interface for adding, removing, retrieving, and enumerating all
 * system services in the game and processes events that occur on every single
 * tick and propagates it to the child services.
 *
 * The framework also has a special reference for the application interface
 * and handles exception reporting and special I/O streams that should be
 * easily accessible by directors and services.
 */
class cIGZFrameWork : public cIGZUnknown
{
	public:
		enum FrameworkState
		{
			kStatePreFrameWorkInit = 1,
			kStatePreAppInit = 3,
			kStatePostAppInit = 6,
			kStatePreAppShutdown = 10,
			kStatePostAppShutdown = 11,
			kStatePostSystemServiceShutdown = 12
		};

		/**
		 * @brief Adds a service to the game registry
		 *
		 * Adding a system service allows other directors to retrieve the
		 * service from the framework, which can be used for libraries that
		 * implement common code for different directors.
		 *
		 * For services that rely on processing on every active or idle tick,
		 * the service must be specifically registered for it.
		 *
		 * @see AddToTick(cIGZSystemService* pService)
		 * @see AddToOnIdle(cIGZSystemService* pService)
		 */
		virtual bool AddSystemService(cIGZSystemService* pService) = 0;
		
		/**
		 * @brief Unregisters a system service
		 */
		virtual bool RemoveSystemService(cIGZSystemService* pService) = 0;

		/**
		 * @brief Gets a pointer to a system service using a given interface
		 * @param srvid The service ID to get the pointer for
		 * @param riid The interface ID to cast the service to
		 * @param ppService A pointer to the pointer to store the reference in
		 * @return Whether the service was successfully retrieved
		 * @see cIGZUnknown::QueryInterface(uint32_t riid, void** ppvObj)
		 */
		virtual bool GetSystemService(uint32_t srvid, uint32_t riid, void** ppService) = 0;

		/**
		 * @brief Uses a cIGZUnknownEnumerator to iterate over system services
		 *
		 * This function is generally unusable since the structure for the
		 * UnknownEnumerator is... well... unknown.
		 */
		virtual bool EnumSystemServices(void* enumerator, cIGZUnknown* pUnknown, uint32_t dwUnknown) = 0; // ??? - enumerator is a function but what is prototype?

		/**
		 * @brief Subscribes an object to framework event callbacks
		 */
		virtual bool AddHook(cIGZFrameWorkHooks* pHooks) = 0;

		/**
		 * @brief Unsubscribes an object from framework event callbacks
		 */
		virtual bool RemoveHook(cIGZFrameWorkHooks* pHooks) = 0;

		/**
		 * @brief Activates the on-tick event for a system service
		 *
		 * Unless the service has a real need to perform an action on every
		 * single tick, it's preferable to subscribe to messages for
		 * performance considerations
		 */
		virtual bool AddToTick(cIGZSystemService* pService) = 0;

		/**
		 * @brief Deactivates the on-tick event for a system service
		 */
		virtual bool RemoveFromTick(cIGZSystemService* pService) = 0;

		/**
		 * @brief Activates the on-idle event for a system service
		 * @see AddToTick(cIGZSystemService* pService)
		 */
		virtual bool AddToOnIdle(cIGZSystemService* pService) = 0;

		/**
		 * @brief Deactivates the on-idle event for a system service
		 */
		virtual bool RemoveFromOnIdle(cIGZSystemService* pService) = 0;
		
		/**
		 * @return The interval at which the idle tick occurs, in milliseconds
		 */
		virtual int32_t GetOnIdleInterval(void) = 0;
		virtual bool SetOnIdleInterval(int32_t nInterval) = 0;

		/**
		 * @brief Called whenever the game has an active tick
		 * @param dwTimeElapsed Time since the last tick, in milliseconds
		 */
		virtual bool OnTick(uint32_t dwTimeElapsed) = 0;

		/**
		 * @brief Called whenever the game has an idle tick
		 */
		virtual bool OnIdle(void) = 0;

		/**
		 * @return Whether or not tick events are enabled
		 */
		virtual bool IsTickEnabled(void) = 0;

		/**
		 * @brief Sets whether tick events are enabled
		 * @param bTick Whether to enable tick events or not
		 * @return The "this" pointer
		 */
		virtual cIGZFrameWork* ToggleTick(bool bTick) = 0;

		/** 
		 * @brief Prepares the game for shutdown for some given reason
		 * @return nQuitReason
		 */
		virtual int32_t Quit(int32_t nQuitReason) = 0;

		/**
		 * @brief Immediately terminates the game for some given reason
		 */
		virtual void AbortiveQuit(int32_t nQuitReason) = 0;
		
		/**
		 * @return The command line passed to the application
		 */
		virtual char* CommandLine(void) = 0;
		
		/**
		 * @return Whether the game is in an installation-only mode
		 */
		virtual bool IsInstall(void) = 0;

		/**
		 * @return A pointer to the parent COM
		 */
		virtual cIGZCOM* GetCOMObject(void) = 0;

		virtual FrameworkState GetState(void) = 0;

		virtual void* GetDebugStream(void) = 0;
		virtual int32_t DefaultDebugStream(void) = 0;
		virtual int32_t DebugStream(void) = 0;
		virtual bool SetDebugStream(void* pIGZDebugStream) = 0;
		virtual bool SetDebugLevel(int32_t nLevel) = 0;
		virtual int32_t GetDebugLevel(void) = 0;

		virtual int32_t StdOut(void) = 0;
		virtual int32_t StdErr(void) = 0;
		virtual int32_t StdIn(void) = 0;

		virtual void* GetStream(void) = 0;
		virtual bool SetStream(int32_t nUnknown, cIGZUnknown* pUnknown) = 0;

		/**
		 * @brief Sets an application to associate with the framework
		 */
		virtual bool SetApplication(cIGZApp* const pIGZApp) = 0;
		
		/**
		 * @return The application associated with this framework
		 */
		virtual cIGZApp* const Application(void) = 0;

		/**
		 * @brief Reports a fatal error using the text provided
		 */
		virtual void ReportException(char const* szExcText) = 0;
		virtual cIGZExceptionNotification* ExceptionNotificationObj(void) = 0;
};