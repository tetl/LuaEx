#include <ISmmPlugin.h>
#include <vscript/ivscript.h>
#include "dotaptrs.h"

#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

class IServerTools;

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
	void UnregisterInstances();

private:
	IScriptVM* Hook_CreateVMPost(ScriptLanguage_t language);
	void Hook_DestroyVM(IScriptVM *pVM);
};

extern LuaEx g_LuaEx;
extern IVEngineServer *engine;
extern IServerGameDLL *gamedll;
extern IServerGameEnts *gameents;
extern IServerTools *servertools;
extern CGlobalVars *gpGlobals;
extern ISmmAPI *g_SMAPI;

PLUGIN_GLOBALVARS();



#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

