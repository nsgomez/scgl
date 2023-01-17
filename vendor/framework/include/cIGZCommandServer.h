#pragma once
#include <vector>
#include "cIGZString.h"
#include "cIGZUnknown.h"

class cIGZCommandDispatcher;
class cIGZCommandGenerator;
class cIGZCommandParameterSet;

class cIGZCommandServer : public cIGZUnknown
{
	public:
		struct cGZCommandInfo
		{
			uint32_t dwCommandID;
			char const *pszCommandName;
			char const *pszParameterInfo;
		};

		virtual bool Init(void) = 0;
		virtual bool Shutdown(void) = 0;

		virtual bool CreateCommandParameterSet(cIGZCommandParameterSet **ppParamSet) = 0;

		virtual bool RegisterCommand(uint32_t dwCommandID, char const *pszCommandName, char const *pszParameterInfo) = 0;
		virtual bool RegisterCommands(cGZCommandInfo *pCmdInfoArray, uint32_t dwCommandCount) = 0;
		virtual bool RegisterCommandParameterInfo(uint32_t dwCommandID, char const *pszParameterInfo) = 0;

		virtual bool UnregisterCommand(uint32_t dwCommandID) = 0;
		virtual bool UnregisterCommands(cGZCommandInfo *pCmdInfoArray, uint32_t dwCommandCount) = 0;

		virtual bool GetCommandInfo(uint32_t dwCommandID, cIGZString& szCommandNameOut, cIGZString& szParameterInfoOut) = 0;
		virtual uint32_t GetCommandID(cIGZString const& szCommandName) = 0;
		virtual uint32_t GetCommandIDFromCommandString(char const *pszCommandString, int32_t nStringLen) = 0;

		virtual bool EnumerateCommands(std::vector<uint32_t>& vecCommandIDs) = 0;

		virtual bool RegisterCommandGenerator(cIGZCommandGenerator *pCommandGen) = 0;
		virtual bool UnregisterCommandGenerator(cIGZCommandGenerator *pCommandGen) = 0;

		virtual cIGZCommandDispatcher* GetCommandDispatcher(void) = 0;
		virtual bool SetCommandDispatcher(cIGZCommandDispatcher* pCommandDispatcher) = 0;

		virtual bool ExecuteCommand(uint32_t dwCommandID, cIGZCommandParameterSet* pParamUnknown, cIGZCommandParameterSet* pCommandParams) = 0;
		virtual bool ExecuteCommand(char const *pszCommandString, int32_t nStringLen, cIGZCommandParameterSet* pParamSet) = 0;
		virtual bool ExecuteCommandFormatted(char const *pszCommandFormat, cIGZCommandParameterSet *pParamUnknown, ...) = 0;

		virtual bool ConvertStringToCommand(char const *pszCommandString, uint32_t dwUnknown, uint32_t& dwCommandIDOut, cIGZCommandParameterSet*& pParamSetOut) = 0;
		virtual bool ConvertCommandToString(uint32_t dwCommandID, cIGZCommandParameterSet *pParamSet, cIGZString& szStringOut) = 0;
		virtual bool ConvertCommandOutputToString(cIGZCommandParameterSet *pParamSet, cIGZString& szStringOut) = 0;

		virtual ~cIGZCommandServer() { }
};