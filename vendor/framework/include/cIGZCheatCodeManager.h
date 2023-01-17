#pragma once
#include "cIGZUnknown.h"

class cIGZMessageTarget;
class cIGZMessageTarget2;
class cIGZString;

/**
 * @brief Service for managing cheat codes
 *
 * The cheat code manager allows COM directors to register and unregister
 * cheat codes at will, and dispatches messages when cheat codes are used. The
 * manager can also handle the preservation of cheat code state for cheats
 * that persist across city saves.
 */
class cIGZCheatCodeManager : public cIGZUnknown
{
	public:
		/**
		 * @brief Adds a new active cheat code to the game
		 * @param clsid A unique ID for the cheat code
		 * @param szCheatName The cheat code string for this cheat
		 * @return Whether the cheat was successfully registered
		 *
		 * Regardless of whether the game is in city or region mode, this
		 * method will add a new active cheat code to the game. When the cheat
		 * code is used by the player, a message (particularly a
		 * cIGZMessage2Standard) will be dispatched to notification targets of
		 * this service.
		 *
		 * The message for issued cheats will have a type set to 0x230e27ac,
		 * with data1 containing the clsid for the cheat code, hence the clsid
		 * should be unique. To receive this message, COM directors must
		 * be subscribed to this class's notifications.
		 *
		 * data2 in the message contains the full cheat string given.
		 *
		 * @see AddNotification(cIGZMessageTarget* pTarget, uint32_t dwMessageID)
		 * @see AddNotification2(cIGZMessageTarget2* pTarget, uint32_t dwMessageID)
		 * @see UnregisterCheatCode(uint32_t clsid)
		 */
		virtual bool RegisterCheatCode(uint32_t clsid, cIGZString const& szCheatName) = 0;

		/**
		 * @brief Deactivates and unregisters a cheat code from the game
		 * @param clsid The ID of the cheat code to remove
		 * @return Whether the cheat was unregistered
		 */
		virtual bool UnregisterCheatCode(uint32_t clsid) = 0;
		
		/**
		 * @brief Allows a message target to receive cheat code messages
		 * @param pTarget The message target to subscribe to these messages
		 * @param dwMessageID Reserved; should simply be set to zero
		 * @return Whether the target was successfully subscribed
		 *
		 * Subscribing to cheat code notifications via this method will result
		 * in messages of type cGZMessage being sent to the subscriber.
		 */
		virtual bool AddNotification(cIGZMessageTarget* pTarget, uint32_t dwMessageID) = 0;

		/**
		 * @brief Unsubscribes a message target from cheat code notifications
		 * @param pTarget The message target to unsubscribe
		 * @param dwMessageID Reserved; should simply be set to zero
		 * @return Whether the target was successfully unsubscribed
		 *
		 * Only works for subscribers awaiting cGZMessage types of messages.
		 * @see AddNotification(cIGZMessageTarget* pTarget, uint32_t dwMessageID)
		 */
		virtual bool RemoveNotification(cIGZMessageTarget* pTarget, uint32_t dwMessageID) = 0;

		/**
		 * @see AddNotification(cIGZMessageTarget* pTarget, uint32_t dwMessageID)
		 *
		 * Subscribes to cheat code notifications, expecting
		 * cIGZMessage2Standard messages. This is the preferred method of
		 * getting cheat notifications.
		 */
		virtual bool AddNotification2(cIGZMessageTarget2* pTarget, uint32_t dwMessageID) = 0;

		/**
		 * @see RemoveNotification(cIGZMessageTarget* pTarget, uint32_t dwMessageID)
		 *
		 * Unsubscribes from cheat code notifications for directors that are
		 * cIGZMessageTarget2 implementations only.
		 */
		virtual bool RemoveNotification2(cIGZMessageTarget2* pTarget, uint32_t dwMessageID) = 0;

		/**
		 * @brief Toggles whether or not to keep track of used cheats
		 * @return The "this" pointer to the class
		 */
		virtual cIGZCheatCodeManager* EnableCheatCodeHistory(bool bEnable) = 0;

		/**
		 * @brief Clears the list of used cheat codes
		 * @return Whether the cheat list was cleared successfully
		 */
		virtual bool ClearCheatCodeHistory(void) = 0;

		/**
		 * @brief Stores the cheat code save data in an unknown structure
		 */
		virtual bool GetCheatCodeSaveData(void*, uint32_t& dwSize) = 0;

		/**
		 * @brief Sets the data to save for cheat code history
		 * @see GetCheatCodeSaveData(void*, uint32_t& dwSize)
		 */
		virtual bool SetCheatCodeSaveData(void const*, uint32_t dwSize) = 0;

		/**
		 * @brief Gets a list of all cheat strings registered in-game
		 * @param ppStringsOut A pointer to an array for storing cIGZString pointers
		 * @param dwSize A reference that stores the number of strings retrieved
		 * @return The number of strings retrieved
		 */
		virtual int32_t GetListOfAllCheatStrings(cIGZString** ppStringsOut, uint32_t& dwSize) = 0;
		
		virtual bool HasUserUsedCheat(uint32_t clsid) = 0;
		virtual bool GetCheatState(uint32_t clsid) = 0;

		/**
		 * @brief Gets a list of cheat code IDs that are enabled
		 * @param pCLSIDsOut A pointer to an array of unsigned ints of enabled cheats
		 * @param dwCountOut A reference that stores the number of cheats enabled
		 * @return The number of cheats enabled
		 */
		virtual int32_t GetCheatsWithEnabledStates(uint32_t* pCLSIDsOut, uint32_t& dwCountOut) = 0;
		
		virtual bool SetCheatState(uint32_t clsid, bool bState) = 0;
		virtual bool SetStateShouldBeSaved(uint32_t clsid, bool bToggle) = 0;
		virtual bool ClearAllEnabledCheatStates(void) = 0;

		/**
		 * @brief Forces the game to dispatch a cheat notification
		 * @param szCheatString The cheat code string to send
		 * @param dwCheatID The ID of the cheat code to dispatch
		 * @return Whether the message was dispatched successfully
		 *
		 * Can be used to force a cheat code to be issued without user
		 * intervention.
		 */
		virtual bool SendCheatNotifications(cIGZString const& szCheatString, uint32_t dwCheatID) = 0;
		
		/**
		 * @brief Gets a cheat ID given a cheat code name
		 * @param szCheatName The cheat code name to search for
		 * @param dwCheatID A reference to an integer; stores the cheat ID found
		 * @return Whether the cheat code was successfully found
		 */
		virtual bool DoesCheatCodeMatch(cIGZString const& szCheatName, uint32_t& dwCheatID) = 0;

		/**
		 * @brief Processes a full cheat string
		 * @see DoesCheatCodeMatch(cIGZString const& szCheatName, uint32_t& dwCheatID)
		 */
		virtual bool DoDefaultCheatCodeProcessing(cIGZString const& szCheatLine, uint32_t& dwCheatID) = 0;

		/**
		 * @brief Sets whether or not to enable cheat codes
		 * @param bEnable Whether cheat codes are enabled
		 * @param szCompressionKey The key to use when encrypting cheat data
		 * @return Whether the operation succeeded or not
		 *
		 * szCompressionKey cannot be longer than 64 characters.
		 */
		virtual bool EnableCheatCodes(bool bEnable, cIGZString const& szCompressionKey) = 0;
		virtual bool GetCheatCodesEnabled(void) = 0;

		/**
		 * @brief Gets the compression key/password for saving cheat state
		 * @param pszPassword Pointer to an array of characters; the key is saved here
		 * @param dwLen Reference to an integer; length of password saved here
		 * @return Whether the cheat code password was successfully retrieved
		 */
		virtual bool GetCheatCodePassword(uint8_t* pszPassword, uint32_t& dwLen) = 0;

		/**
		 * @brief Sets the compression key/password for cheat state
		 * @param pszPassword A character array representing the password
		 * @param dwLen The length of the password given
		 * @return Whether the cheat code password was successfully saved
		 */
		virtual bool SetCheatCodePassword(uint8_t* pszPassword, uint32_t dwLen) = 0;

		/**
		 * @brief Unknown. Does not appear to be used for anything.
		 */
		virtual bool SetEnableDisableStrings(cIGZString const&, cIGZString const&) = 0;
};