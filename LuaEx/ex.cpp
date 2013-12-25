#include "ex.h"

BEGIN_SCRIPTDESC_ROOT(Ex, "More Scripting More Fun")
	DEFINE_SCRIPTFUNC(SetUnitControllableByPlayer, "Allows Unit to be controlled by PlayerId")
END_SCRIPTDESC();

Ex::Ex() : m_hScope(INVALID_HSCRIPT)
{
}

void Ex::SetUnitControllableByPlayer(HSCRIPT npc, int playerId, bool something)
{
	void *npcPtr = luavm->GetInstanceValue(npc);

	NONSTANDARD_CALL_REG2_STACK1_VOID(SetControllablePtr, esi, npcPtr, eax, playerId, 0)
}

/*void Ex::IsDeniable(HSCRIPT npc)
{
	void *npcPtr = luavm->GetInstanceValue(npc);
	bool returned;
	if(npcPtr)
	{
		__asm
		{
			mov eax, npcPtr
			call isDeniable
		}
	}else printf("NULL POINTER FROM VSCRIPT");
}


DetourHandler Ex::IsDeniableCalled()
{
	ScriptVariant_t scriptReturn;
	if(cbfunc)
	{
		printf("Calling Lua callback...\n");
		luavm->Call<int>(cbfunc, NULL, false, &scriptReturn, 0);
	}
	DetourHandler handler;
	if(!scriptReturn.IsNull())
	{
		handler.result = DET_SUPERCEDE;
		handler.value.boolType = scriptReturn.m_bool;
	}else{
		handler.result = DET_IGNORE;
	}
		
	return handler;
}*/

