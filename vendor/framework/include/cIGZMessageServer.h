#pragma once
#include "cIGZUnknown.h"

class cGZMessage;
class cIGZMessageTarget;

/**
 * @brief Interface for a server that relays messages posted to it
 *
 * The message server is a system service that allows directors to send
 * messages either to a specific target or to any other director that is
 * subscribed to receive them. The server also allows directors to subscribe
 * and unsubscribe to messages.
 */
class cIGZMessageServer : public cIGZUnknown
{
	public:
		/**
		 * @brief Immediately emits the message given to all subscribers
		 */
		virtual bool MessageSend(cGZMessage const& sMessage) = 0;

		/**
		 * @brief Queues the message for emission on the next tick
		 * @param bHighPriority Puts the message at the front of the queue
		 */
		virtual bool MessagePost(cGZMessage const& sMessage, bool bHighPriority) = 0;

		/**
		 * @brief Subscribes a message target to some given event type
		 * @param pTarget The class to subscribe for events
		 * @param dwMessageID The message type to subscribe to
		 */
		virtual bool AddNotification(cIGZMessageTarget* pTarget, uint32_t dwMessageID) = 0;

		/**
		 * @brief Unsubscribes a message target from some given event type
		 * @param The class to unsubscribe events for
		 * @param dwMessageID The message type to ignore
		 */
		virtual bool RemoveNotification(cIGZMessageTarget* pTarget, uint32_t dwMessageID) = 0;

		/**
		 * @brief Posts a message to a specific message target
		 */
		virtual bool GeneralMessagePostToTarget(cGZMessage const& sMessage, cIGZMessageTarget* pTarget) = 0;

		/**
		 * @brief Cancels any pending message posts
		 *
		 * If any posted messages have not been processed because an active
		 * tick has not occurred yet, this method will cancel the messages
		 * and prevent them from being sent.
		 */
		virtual bool CancelGeneralMessagePostsToTarget(cIGZMessageTarget* pTarget) = 0;

		/**
		 * @brief A service event for processing posted messages on each tick
		 */
		virtual bool OnTick(void) = 0;

		virtual uint32_t GetMessageQueueSize(void) = 0;
		virtual cIGZMessageServer* SetAlwaysClearQueueOnTick(bool bToggle) = 0;
};