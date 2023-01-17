#pragma once
#include "cIGZUnknown.h"

/**
 * @brief A preferred interface for interclass communication
 *
 * Unlike cGZMessage, this interface provides a more preferred method of
 * communicating between classes and directors that, as an Unknown-based
 * interface, can be cast to different subclasses.
 */
class cIGZMessage2 : public cIGZUnknown
{
	public:
		virtual bool Create(uint32_t dwTypeIID, void** ppUnknown) const = 0;

		virtual uint32_t GetType(void) const = 0;
		virtual uint32_t SetType(uint32_t dwType) = 0;

		/**
		 * @brief Compares messages by their assigned type
		 */
		virtual bool operator==(cIGZMessage2 const& msgOther) const = 0;

		/**
		 * @see operator==(cIGZMessage2 const& msgOther) const
		 */
		virtual bool operator<(cIGZMessage2 const& msgOther) const = 0;
};