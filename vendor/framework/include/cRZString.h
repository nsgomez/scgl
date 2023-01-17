#pragma once
#include <string>
#include "cIGZString.h"

class cRZString : public std::string, public cIGZString
{
public:
	cRZString();
	cRZString(const char* src);
	cRZString(const char* src, uint32_t len);
	cRZString(cIGZString const& src);
	cRZString(cIGZString const* src);
	virtual ~cRZString();

public:
	virtual bool QueryInterface(uint32_t iid, void** outPtr) override;
	virtual uint32_t AddRef(void) override;
	virtual uint32_t Release(void) override;

public:
	virtual void FromChar(char const* otherStr) override;
	virtual void FromChar(char const* otherStr, uint32_t length) override;

	virtual char const* ToChar() const override;
	virtual char const* Data() const override;
	virtual uint32_t Strlen() const override;

	virtual bool IsEqual(cIGZString const* other, bool caseSensitive = true) const override;
	virtual bool IsEqual(cIGZString const& other, bool caseSensitive = true) const override;
	virtual bool IsEqual(char const* other, uint32_t otherLen, bool caseSensitive = true) const override;

	virtual int CompareTo(cIGZString const& other, bool caseSensitive = true) const override;
	virtual int CompareTo(char const* other, uint32_t otherLen = -1, bool caseSensitive = true) const override;
	int CompareTo(cRZString const& other) const;

	virtual cIGZString& operator=(cIGZString const& other) override;
	cRZString& operator=(cRZString const& other);
	virtual void Copy(cIGZString const& src) override;

	virtual void Resize(uint32_t newLength) override;

	virtual cIGZString& Append(char const* src, uint32_t srcLen) override;
	virtual cIGZString& Append(cIGZString const& src) override;

	virtual cIGZString& Insert(uint32_t position, char const* src, uint32_t srcLen) override;
	virtual cIGZString& Insert(uint32_t position, cIGZString const& src) override;

	virtual cIGZString& Replace(uint32_t position, char const* src, uint32_t srcLen) override;
	virtual cIGZString& Replace(uint32_t position, cIGZString const& src) override;

	virtual cIGZString& Erase(uint32_t position = 0, uint32_t length = -1) override;

	virtual uint32_t Find(char const* needle, uint32_t position = 0, bool caseSensitive = true) const override;
	virtual uint32_t Find(cIGZString const& needle, uint32_t position = 0, bool caseSensitive = true) const override;
	uint32_t Find(cRZString const& needle, uint32_t position = 0);

	virtual uint32_t RFind(char const* needle, uint32_t position = 0, bool caseSensitive = true) const override;
	virtual uint32_t RFind(cIGZString const& needle, uint32_t position = 0, bool caseSensitive = true) const override;
	uint32_t RFind(cRZString const& needle, uint32_t position = 0);

	virtual void Sprintf(char const* format, ...) override;

public:
	bool BeginsWith(char const* needle, uint32_t needleLen) const;
	bool EndsWith(char const* needle, uint32_t needleLen) const;

	void Trim();
	void LTrim();
	void RTrim();

	void MakeLower();
	void MakeUpper();

	void Strcat(char const* str);
	void Strncpy(char const* str, uint32_t count);

public:
	bool operator<  (const cIGZString& other) const;
	bool operator>  (const cIGZString& other) const;
	bool operator== (const cIGZString& other) const;

	bool operator<  (char const* other) const;
	bool operator>  (char const* other) const;
	bool operator== (char const* other) const;

protected:
	uint32_t refCount;
};