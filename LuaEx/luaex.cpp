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
#include <tier0/platform.h>
#include <tier1/fmtstr.h>
#include <sh_memory.h>

#include "CDetour/detour.h"
#include "CDetour/defines.h"

static LuaEx g_LuaEx;
static IScriptManager *scriptmgr = NULL;
static Ex *ex = new Ex();

IScriptVM *luavm;

const char *setControllableSignature = "\x55\x8B\xEC\x51\x56\x89\x86\x64\x0B\x00\x00\xE8\x2A\x2A\x2A\x2A\x80\x7D\x08\x00\x75\x2A\x83\xBE\x64\x0B\x00\x00\xFF\x74\x2A\x8B\x8E\x20\x01";
void *SetControllablePtr;

PLUGIN_EXPOSE(LuaEx, g_LuaEx);

SH_DECL_HOOK1(IScriptManager, CreateVM, SH_NOATTRIB, 0, IScriptVM *, ScriptLanguage_t);

bool LuaEx::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	InitGlobals(error, maxlen);
	InitHooks();

	SetControllablePtr = FindAddress(setControllableSignature, 35);

	return true;
}

void LuaEx::InitHooks()
{
	SH_ADD_HOOK(IScriptManager, CreateVM, scriptmgr, SH_MEMBER(this, &LuaEx::Hook_CreateVM), false);
}

void LuaEx::ShutdownHooks()
{
	SH_REMOVE_HOOK(IScriptManager, CreateVM, scriptmgr, SH_MEMBER(this, &LuaEx::Hook_CreateVM), false);
}

bool LuaEx::InitGlobals(char *error, size_t maxlen)
{	
	ISmmAPI *ismm = g_SMAPI;
	GET_V_IFACE_CURRENT(GetEngineFactory, scriptmgr, IScriptManager, VSCRIPT_INTERFACE_VERSION);
	return true;
}

IScriptVM* LuaEx::Hook_CreateVM(ScriptLanguage_t language)
{
	luavm = SH_CALL(scriptmgr, &IScriptManager::CreateVM)(language);
	luavm->RegisterInstance(ex, "Ex");
	RETURN_META_VALUE(MRES_SUPERCEDE, luavm);
}

void *LuaEx::FindAddress(const char *sig, size_t len) //Quality sigscanner by Nick Hastings
{
	bool found;
	char *ptr, *end;

	LPCVOID startAddr = g_SMAPI->GetServerFactory(false);

	MEMORY_BASIC_INFORMATION mem;
 
	if (!startAddr)
		return NULL;
 
	if (!VirtualQuery(startAddr, &mem, sizeof(mem)))
		return NULL;
 
	IMAGE_DOS_HEADER *dos = reinterpret_cast<IMAGE_DOS_HEADER *>(mem.AllocationBase);
	IMAGE_NT_HEADERS *pe = reinterpret_cast<IMAGE_NT_HEADERS *>((intp)dos + dos->e_lfanew);
 
	if (pe->Signature != IMAGE_NT_SIGNATURE)
	{
		// GetDllMemInfo failedpe points to a bad location
		return NULL;
	}

	ptr = reinterpret_cast<char *>(mem.AllocationBase);
	end = ptr + pe->OptionalHeader.SizeOfImage - len;

	while (ptr < end)
	{
		found = true;
		for (size_t i = 0; i < len; i++)
		{
			if (sig[i] != '\x2A' && sig[i] != ptr[i])
			{
				found = false;
				break;
			}
		}

		if (found)
			return ptr;

		ptr++;
	}

	return NULL;
}



bool LuaEx::Unload(char *error, size_t maxlen)
{
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
