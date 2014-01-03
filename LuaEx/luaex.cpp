/**
* =============================================================================
* D2Fixups
* Copyright (C) 2013 Tyler Edge
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 2.0 or later, as published
* by the Free Software Foundation.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, you are also granted permission to link the code
* of this program (as well as its derivative works) to "Dota 2," the
* "Source Engine, and any Game MODs that run on software by the Valve Corporation.
* You must obey the GNU General Public License in all respects for all other
* code used.  Additionally, this exception is granted to all derivative works.
*/

#include "LuaEx.h"
#include "ex.h"
#include "utils.h"
#include <tier0/platform.h>
#include <tier1/fmtstr.h>
#include <toolframework\itoolentity.h>
#include <sh_memory.h>
#include <assert.h>

#include "CDetour/detour.h"
#include "CDetour/defines.h"

static LuaEx g_LuaEx;
static IScriptManager *scriptmgr = NULL;

IScriptVM *luavm;
IVEngineServer *engine;
IServerGameDLL *gamedll;
IServerGameEnts *gameents;
IServerTools *servertools;
CGlobalVars *gpGlobals;



PLUGIN_EXPOSE(LuaEx, g_LuaEx);

SH_DECL_HOOK1(IScriptManager, CreateVM, SH_NOATTRIB, 0, IScriptVM *, ScriptLanguage_t);
SH_DECL_HOOK1_void(IScriptManager, DestroyVM, SH_NOATTRIB, 0, IScriptVM *);

bool LuaEx::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	InitGlobals(error, maxlen);
	InitHooks();


	for (auto i = ScriptExtensions().begin(); i != ScriptExtensions().end(); ++i)
	{
		(*i)->Init();
	}


	return true;
}

void LuaEx::InitHooks()
{
	SH_ADD_HOOK(IScriptManager, CreateVM, scriptmgr, SH_MEMBER(this, &LuaEx::Hook_CreateVMPost), true);
	SH_ADD_HOOK(IScriptManager, DestroyVM, scriptmgr, SH_MEMBER(this, &LuaEx::Hook_DestroyVM), true);
	SetupDetours();
}

void LuaEx::ShutdownHooks()
{
	SH_REMOVE_HOOK(IScriptManager, CreateVM, scriptmgr, SH_MEMBER(this, &LuaEx::Hook_CreateVMPost), true);
	SH_REMOVE_HOOK(IScriptManager, DestroyVM, scriptmgr, SH_MEMBER(this, &LuaEx::Hook_DestroyVM), true);
}

bool LuaEx::InitGlobals(char *error, size_t maxlen)
{	
	ISmmAPI *ismm = g_SMAPI;
	GET_V_IFACE_CURRENT(GetEngineFactory, scriptmgr, IScriptManager, VSCRIPT_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
	GET_V_IFACE_CURRENT(GetServerFactory, gamedll, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	GET_V_IFACE_CURRENT(GetServerFactory, gameents, IServerGameEnts, INTERFACEVERSION_SERVERGAMEENTS);
	GET_V_IFACE_CURRENT(GetServerFactory, servertools, IServerTools, VSERVERTOOLS_INTERFACE_VERSION);
	gpGlobals = ismm->GetCGlobals();
	return true;
}

IScriptVM* LuaEx::Hook_CreateVMPost(ScriptLanguage_t language)
{
	// We're assuming that there is only one scripting VM and that it's Lua.
	assert(!luavm);
	assert(language == SL_LUA);

	luavm = META_RESULT_ORIG_RET(IScriptVM *);
	
	for (auto i = ScriptExtensions().begin(); i != ScriptExtensions().end(); ++i)
	{
		ScriptExtension *ex = *i;
		HSCRIPT scope = luavm->RegisterInstance(ex->GetScriptDesc(), ex);
		luavm->SetValue(NULL, ex->GetInstanceName(), scope);
		ex->SetHScript(scope);
	}	

	
	RETURN_META_VALUE(MRES_IGNORED, NULL);
}

void LuaEx::Hook_DestroyVM(IScriptVM *pVM)
{
	if (luavm)
	{
		assert(pVM == luavm);

		// No need to unregister instances. VM is toast anyway

		for (auto i = ScriptExtensions().begin(); i != ScriptExtensions().end(); ++i)
		{
			ScriptExtension *ex = *i;
			ex->SetHScript(INVALID_HSCRIPT);
		}
		luavm = NULL;
	}
}

void LuaEx::UnregisterInstances()
{
	for (auto i = ScriptExtensions().begin(); i != ScriptExtensions().end(); ++i)
	{
		ScriptExtension *ex = *i;
		HSCRIPT scope = ex->GetHScript();
		if (scope == INVALID_HSCRIPT)
			continue;

		luavm->RemoveInstance(scope);
		ex->SetHScript(INVALID_HSCRIPT);
	}
}


bool LuaEx::Unload(char *error, size_t maxlen)
{
	if (luavm)
	{
		UnregisterInstances();
	}

	for (auto i = ScriptExtensions().begin(); i != ScriptExtensions().end(); ++i)
	{
		(*i)->Shutdown();
	}

	ShutdownHooks();
	return true;
}

const char * LuaEx::GetAuthor()
{
	return "Tyler Edge";
}

const char * LuaEx::GetName()
{
	return	"LuaEx";
}

const char * LuaEx::GetDescription()
{
	return	"Extends Dota 2's vscript engine";
}

const char * LuaEx::GetURL()
{
	return	"http://example.com";
}

const char * LuaEx::GetLicense()
{
	return	"IDK";
}

const char * LuaEx::GetVersion()
{
	return "1.0";
}

const char * LuaEx::GetDate()
{
	return __DATE__;
}

const char * LuaEx::GetLogTag()
{
	return "LX";
}

/*DETOUR_DECL_NAKED(IsDeniable, bool){
    CBaseEntity *unit;

    START_NAKED_ARG1(eax, unit)

	DetourHandler handler;
	handler = ex->IsDeniableCalled();

	bool retn;

	switch(handler.result)
	{
		case DET_IGNORE:
			NONSTANDARD_CALL_REG1_RET(IsDeniable_Actual, al, eax, unit)
			break;
		case DET_SUPERCEDE:
			retn = handler.value.boolType;
			break;
	}

	 END_NAKED_RETURN(al, retn);
}*/
