#pragma once
#include "cIGZMessage2.h"
#include "cRZUnknown.h"

class cRZMessage2 : public cIGZMessage2, public cRZUnknown
{
	public:
		bool QueryInterface(uint32_t riid, void** ppvObj);
		uint32_t AddRef(void) { return cRZUnknown::AddRef(); }
		uint32_t Release(void) { return cRZUnknown::Release(); }

	public:
		cRZMessage2();

		bool Create(uint32_t dwType, void** ppUnknown) const override;

		uint32_t GetType(void) const override;
		uint32_t SetType(uint32_t dwType) override;

		bool operator==(cIGZMessage2 const& msgOther) const override;
		bool operator<(cIGZMessage2 const& msgOther) const override;

	protected:
		uint32_t m_dwType;
};