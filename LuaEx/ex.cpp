#include "ex.h"
#include "utils.h"
#include "dotaptrs.h"
#include "CDetour\detour.h"
#include "CDetour\defines.h"
#include <vector>
#include <map>

struct CUnitOrders
{
        int                m_iPlayerID;
        int                m_iUnknown;
        CUtlVector<int>        m_SelectedUnitEntIndexes;
        int                m_iOrderType;
        int                m_iTargetEntIndex;
        int                m_iAbilityEntIndex;
        Vector        m_TargetPos;
        bool        m_bQueueOrder;
};

enum HookType
{
	Hook_ExecuteOrders
};

enum DetourResult
{
	DET_IGNORE,
	DET_SUPERCEDE
};

union DetourReturn
{
	int intType;
	bool boolType;
	CBaseEntity *entType;
};

struct DetourHandler
{
public:
	DetourResult result;
	DetourReturn value;
};

static ExUnit s_exUnit;
static ExAbility s_exAbility;
static Ex s_ex;
static ExHook s_exHook;

CDetour *CreateLinearProjectileDetour;

std::map<HookType, std::vector<HSCRIPT>> hooks; 

static void *ExecuteOrdersPtr;
static void *CreateLinearProjectilePointer;

BEGIN_EX_SCRIPTDESC_ROOT(ExUnit, "Unit-based script extensions")
	DEFINE_SCRIPTFUNC(SetControllableByPlayer, "Allows Unit to be controlled by PlayerId")
END_SCRIPTDESC();

BEGIN_EX_SCRIPTDESC_ROOT(ExAbility, "Ability-based script extensions")
	DEFINE_SCRIPTFUNC(EndCooldown, "Ends the cooldown of Ability")
END_SCRIPTDESC();

BEGIN_EX_SCRIPTDESC_ROOT(Ex, "General script extensions")
	DEFINE_SCRIPTFUNC(ApplyDamage, "Applies TypeField-type Damage to DamagedUnit credited to AttackingUnit. Ability is used scrictly for logging. TODO: Document typeField, experiment with passing NULLs for stuff")
END_SCRIPTDESC();

BEGIN_EX_SCRIPTDESC_ROOT(ExHook, "Registry and management of high-level game hooks")
	DEFINE_SCRIPTFUNC(ExecuteOrders, "ExecuteOrders")
END_SCRIPTDESC();

ScriptExtension::ScriptExtension() : m_hScope(INVALID_HSCRIPT)
{
	ScriptExtensions().push_back(this);
}

std::list<ScriptExtension *> &ScriptExtensions()
{
	static std::list<ScriptExtension *> *l = new std::list<ScriptExtension *>();
	return *l;
}

void ExUnit::SetControllableByPlayer(HSCRIPT npc, int playerId, bool something)
{
	static const char * const setControllableSignature = "\x55\x8B\xEC\x51\x56\x89\x86\x64\x0B\x00\x00\xE8\x2A\x2A\x2A\x2A\x80\x7D\x08\x00\x75\x2A\x83\xBE\x64\x0B\x00\x00\xFF\x74\x2A\x8B\x8E\x20\x01";
	static void *SetControllablePtr = FindAddress(ADDR_SERVER, setControllableSignature, 35);

	void *npcPtr = luavm->GetInstanceValue(npc);
	
	CALL_REG2_STACK1_VOID(SetControllablePtr, esi, npcPtr, eax, playerId, 0)
}

void ExAbility::EndCooldown(HSCRIPT ability)
{
	static const char * const endCooldownSignature = "\x55\x8B\xEC\x0F\x57\xC0\x53\x8B\x5D\x08\xF3\x0F\x10\x8B\xE8\x03\x00\x00";
	static void *EndCooldownPtr = FindAddress(ADDR_SERVER, endCooldownSignature, 18);

	void *abilityPtr = luavm->GetInstanceValue(ability);

	CALL_STACK1_VOID(EndCooldownPtr, abilityPtr) //can be bound to a C function but who cares man
}

void Ex::ApplyDamage(HSCRIPT attackingUnit, HSCRIPT damagedUnit, HSCRIPT ability, float damage, int typeField)
{
	static const char * const applyDamageSignature = "\x55\x8B\xEC\x51\x53\x8B\x5D\x08\x56\x85\xFF\x0F\x84\x2A\x2A\x2A\x2A\x83\xBF\x2A\x2A\x2A\x2A\x2A\x0F\x8E\x2A\x2A\x2A\x2A\xB8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x8B\xF0\x85\xDB\x74\x2A\x8B\x03\x8B\x50\x08\x8B\xCB\xFF\xD2\x8B\x00\x89\x46\x04\xEB";
	static void *ApplyDamagePtr = FindAddress(ADDR_SERVER, applyDamageSignature, 61);

	void *abilityPtr = luavm->GetInstanceValue(ability);
	void *attackerPtr = luavm->GetInstanceValue(attackingUnit);
	void *damagedPtr = luavm->GetInstanceValue(damagedUnit);

	CALL_REG1_STACK5_VOID(ApplyDamagePtr, edi, damagedPtr, 0, typeField, damage, abilityPtr, attackerPtr);
}

void ExHook::ExecuteOrders(HSCRIPT callback)
{
	//ExecuteOrdersDetour->EnableDetour();
	std::vector<HSCRIPT> *hookList = &hooks[Hook_ExecuteOrders];
	hookList->push_back(callback);
}

DETOUR_DECL_NAKED(CreateLinearProjectileHook, bool )
{
	printf("Detour hit\n\n\n");

	CBaseEntity *unit;

    START_NAKED_ARG1(eax, unit)
	/*BEGIN_HOOK_CALLS(Hook_ExecuteOrders)
		luavm->Call(*it, INVALID_HSCRIPT, false, NULL, 0); //*it comes from boilerplate in the begin macro, does this suck?
	END_HOOK_CALLS*/
	END_NAKED_RETURN(al, 0)
}

void SetupDetours()
{
	const char *executeOrdersSignature = "\x55\x8B\xEC\x83\xE4\xC0\x81\xEC\xB4\x01\x00\x00\xA1\x2A\x2A\x2A\x2A\x53\x56\x66\x0F\x57\xC0\x66\x0F\x13\x84\x24\xD4\x00\x00";
	ExecuteOrdersPtr = FindAddress(ADDR_SERVER, executeOrdersSignature, 31);

	const char *createLinearProjectileSignature = "\x55\x8B\xEC\x51\x56\x8B\xF0\x8B\x86\x2A\x2A\x2A\x2A\xC1\x2A\x2A\x24\x01\x74\x2A\xB0\x01\x5E\x8B\xE5\x5D\xC3"; //actually IsDeniable
	CreateLinearProjectilePointer = FindAddress(ADDR_SERVER, createLinearProjectileSignature, 27);

	CreateLinearProjectileDetour = &DETOUR_CREATE_STATIC_PTR(CreateLinearProjectileHook, CreateLinearProjectilePointer);
	CreateLinearProjectileDetour->EnableDetour();
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

