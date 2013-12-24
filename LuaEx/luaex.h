#include <ISmmPlugin.h>
#include <vscript/ivscript.h>
#include "dotaptrs.h"

#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

class LuaEx : public ISmmPlugin
{
public: //ISmmPlugin
	bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);
	bool Unload(char *error, size_t maxlen);
	const char *GetAuthor();
	const char *GetName();
	const char *GetDescription();
	const char *GetURL();
	const char *GetLicense();
	const char *GetVersion();
	const char *GetDate();
	const char *GetLogTag();

private:
	bool InitGlobals(char *error, size_t maxlen);
	void InitHooks();
	void ShutdownHooks();

private:
	void *FindAddress(const char* sig, size_t len);

private:
	IScriptVM* Hook_CreateVM(ScriptLanguage_t language);
};

extern LuaEx g_LuaEx;

PLUGIN_GLOBALVARS();



#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

