#include "ex.h"

BEGIN_SCRIPTDESC_ROOT(ExUnit, "Unit-based script extensions")
	DEFINE_SCRIPTFUNC(SetControllableByPlayer, "Allows Unit to be controlled by PlayerId")
END_SCRIPTDESC();

BEGIN_SCRIPTDESC_ROOT(ExAbility, "Ability-based script extensions")
	DEFINE_SCRIPTFUNC(EndCooldown, "Ends the cooldown of Ability")
END_SCRIPTDESC();

ScriptExtension::ScriptExtension() : m_hScope(INVALID_HSCRIPT)
{
}

void ExUnit::SetControllableByPlayer(HSCRIPT npc, int playerId, bool something)
{
	void *npcPtr = luavm->GetInstanceValue(npc);
	
	CALL_REG2_STACK1_VOID(SetControllablePtr, esi, npcPtr, eax, playerId, 0)
}

void ExAbility::EndCooldown(HSCRIPT ability)
{
	void *abilityPtr = luavm->GetInstanceValue(ability);

	CALL_STACK1_VOID(EndCooldownPtr, abilityPtr) //can be bound to a C function but who cares man
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

