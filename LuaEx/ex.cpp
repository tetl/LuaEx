#include "ex.h"
#include "luaex.h"
#include "utils.h"
#include "dotaptrs.h"
#include "CDetour\detour.h"
#include "CDetour\defines.h"
#include <sh_memory.h>
#include "sh_vector.h"
#include <vector>
#include <map>

SH_DECL_MANUALHOOK1_void(ProjThink, 257, 0, 0, const Vector&);

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

static void *IsDeniablePtr;
static void *CreateLinearProjectilePtr;
static void *ProjectileManagerPtr;
static void *ProjectileDirectionPtr;

std::map<HookType, std::vector<HSCRIPT>> hooks; 

static void *ExecuteOrdersPtr;
static void *CreateLinearProjectilePointer;
static void *TrackingPtr;
static void *TrackingSpeedPtr;
static void *SetAbilityByIndexPtr;

BEGIN_EX_SCRIPTDESC_ROOT(ExUnit, "Unit-based script extensions")
	DEFINE_SCRIPTFUNC(SetControllableByPlayer, "Allows Unit to be controlled by PlayerId")
	DEFINE_SCRIPTFUNC(SetAbilityByIndex, "Set Ability to the index of Unit")
END_SCRIPTDESC();

BEGIN_EX_SCRIPTDESC_ROOT(ExAbility, "Ability-based script extensions")
	DEFINE_SCRIPTFUNC(EndCooldown, "Ends the cooldown of Ability")
	DEFINE_SCRIPTFUNC(SetProjectileDirection, "Updates Projectile")
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

void ExUnit::SetAbilityByIndex(HSCRIPT npc, HSCRIPT ability, int index)
{
	static const char * const setAbilityByIndexSignature = "\x55\x8B\xEC\x51\x53\x56\x57\x8B\xF8\x8B\x2A\x2A\x8B\x2A\x2A\x2A\x2A\x2A\x2A\x83\xFA\xFF\x74\x2A\x8B\xC2\x25\xFF\xFF\x00\x00\x8B\xC8\xC1\xE1\x04\x81\x2A\x2A\x2A\x2A\x2A\xC1\xEA\x10\x39\x51\x04";
	SetAbilityByIndexPtr = FindAddress(ADDR_SERVER, setAbilityByIndexSignature, 48);

	void *npcPtr = luavm->GetInstanceValue(npc);
	void *abilityPtr = luavm->GetInstanceValue(ability);

	CALL_REG1_STACK2_VOID(SetAbilityByIndexPtr, eax, npcPtr, index, abilityPtr)
}

void ExAbility::EndCooldown(HSCRIPT ability)
{
	static const char * const endCooldownSignature = "\x55\x8B\xEC\x0F\x57\xC0\x53\x8B\x5D\x08\xF3\x0F\x10\x8B\xE8\x03\x00\x00";
	static void *EndCooldownPtr = FindAddress(ADDR_SERVER, endCooldownSignature, 18);

	void *abilityPtr = luavm->GetInstanceValue(ability);

	CALL_STACK1_VOID(EndCooldownPtr, abilityPtr) //can be bound to a C function but who cares man
}

struct CDotaProjectile
{
	Vector m_vecStartingPos; // 0
	Vector m_vecCurrentPos; // 12
	Vector m_vecVelocity; // 24
	float  m_flUnknown1; // 36
	float  m_flUnknown2; // 40
	float  m_flDistance; // 44
	int    m_iTargetTeam; // 48	
	int    m_iTargetType; // 52
	int    m_iTargetFlags; // 56
	int	   m_iHandle; // 60
	bool   m_bIsBeingDeleted; // 64
	CBaseHandle m_hAttackerNPC; // 68
	char padding[44];
};

struct TrackingProjectileInfo
{
	//tests done against ranged dire creep projectile vs pudge
	int speed; 
	Vector currentpos; //4
	CBaseHandle target; 
	char padding[40];
};

void GetProjectile(int handle, void *ability)
{
	/*CUtlVector<CDotaProjectile*> *projData =  *(CUtlVector<CDotaProjectile*>**)((intp)ProjectileManagerPtr);

	printf("COUNT: %d", projData->Count());

	for(int i = 0; i < projData->Count(); i++)
	{
		projData->Element(i)->m_vecCurrentPos.x = 795;
		projData->Element(i)->m_vecCurrentPos.y = 197;
	}*/
	
	/*CUtlVector<TrackingProjectileInfo> *tprojData = *(CUtlVector<TrackingProjectileInfo>**)((intp)TrackingPtr);
	printf("GOT DATA PTR: %d\n", tprojData);
	
	int *speed = (int*)((intp)ability + 1048);
	printf("Projectile speed: %d\n", *speed);

	printf("COUNT: %d", tprojData->Count());
	for(int i = 0; i < tprojData->Count(); i++)
	{
		
		
	}*/
}


bool init = false;

void ExAbility::SetProjectileDirection(int handle, HSCRIPT ability)
{
	printf("HANDLE: %d\n", handle);
	printf("TRACKING VEC POINTER: %d\n", TrackingPtr);
	GetProjectile(handle, luavm->GetInstanceValue(ability));
	//auto vec = &vector;
	//CALL_REG2_STACK1_VOID(ProjectileDirectionPtr, edi, handle, esi, vec, ProjectileManagerPtr)
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

DETOUR_DECL_STATIC1_STDCALL_NAKED(CreateLinearProjectileHook, int, void*, kv)
{
	void *dude;
	START_NAKED_ARG1(edi, dude)
	printf("OHSHIT");
	int retn;
	CALL_REG1_STACK1_RET(CreateLinearProjectileHook_Actual, eax, edi, ProjectileManagerPtr, kv)

	END_NAKED_RETURN(eax, retn)
}

static CDetour *CreateLinearProjectileDetour;
static CDetour *IsDeniableDetour;

void SetupDetours()
{
	const char *createLinearProjectileSignature = "\x55\x8B\xEC\x83\xE4\xF0\x81\xEC\x24\x01\x00\x00\x53\x56\x8B\x75\x08\xF3\x0F\x10\x46\x1C\x0F\x2E\x2A\x2A\x2A\x2A\x2A\x9F\x57\xF6\xC4\x44\x7B\x2A\x68";
	CreateLinearProjectilePtr = FindAddress(ADDR_SERVER, createLinearProjectileSignature, 27);
	CreateLinearProjectileDetour = new DETOUR_CREATE_STATIC_PTR(CreateLinearProjectileHook, CreateLinearProjectilePtr);
//	CreateLinearProjectileDetour->EnableDetour();

	const char *projectileManagerPointerSignature = "\x0D\x2A\x2A\x2A\x2A\x83\x3C\x81\x00\x74\x2A\x8B\x3C\x81\x39\x57\x3C\x74\x2A\x40\x3B\xC6\x7C\x2A\x52\x68\x2A\x2A\x2A\x2A\xFF\x15\x2A\x2A\x2A\x2A\x83\xC4\x08\xEB";
	ProjectileManagerPtr = (char*)(FindAddress(ADDR_SERVER, projectileManagerPointerSignature, 40)) + 1;

	const char *trackingPointerSignature = "\x3D\x2A\x2A\x2A\x2A\x33\xD2\x8D\x9B\x00\x00\x00\x00\x8B\x4C\x3A\x24\x83\xF9\xFF\x74\x2A\x8B\xC1\x25\xFF\xFF\x00\x00\xC1\xE0\x04\x05\x2A\x2A\x2A\x2A\xC1\xE9\x10\x39\x48";
	TrackingPtr = (void*)((intp)FindAddress(ADDR_SERVER, trackingPointerSignature, 42)  + 1);

	const char *updateProjectileDirectionSignature = "\x55\x8B\xEC\xF3\x0F\x10\x46\x08\x83\xEC\x0C\x0F\x2E\x2A\x2A\x2A\x2A\x2A\x9F\xF6\xC4\x44\x7B\x2A\x68\x2A\x2A\x2A\x2A\xFF\x15\x2A\x2A\x2A\x2A\x83\xC4\x04\x8B\x46\x08\xF3";
	ProjectileDirectionPtr = FindAddress(ADDR_SERVER, updateProjectileDirectionSignature, 42);

	const char *trackingSpeedSignature = "\x57\x8B\xBE\x18\x04\x00\x00\x56\xE8\x2A\x2A\x2A\x2A\x83\xF8\xFF\x75\x2A\x68\x2A\x2A\x2A\x2A\xFF\x15\x2A\x2A\x2A\x2A\x83\xC4\x04\xEB\x2A\x8B\x15\x2A\x2A\x2A\x2A\x8D\x0C\xC5\x00\x00\x00\x00";
	TrackingSpeedPtr = FindAddress(ADDR_SERVER, trackingSpeedSignature, 47);

	//CreateLinearProjectileDetour = &DETOUR_CREATE_STATIC_PTR(CreateLinearProjectileHook, CreateLinearProjectilePointer);
	//CreateLinearProjectileDetour->EnableDetour();
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

