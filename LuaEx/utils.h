#include "luaex.h"

enum AddressType
{
	ADDR_SERVER,
	ADDR_ENGINE,
};

void *FindAddress(AddressType type, const char *sig, size_t len);

inline int IndexOfEdict(const edict_t *pEdict)
{
	return (int) (pEdict - gpGlobals->pEdicts);
}

inline edict_t *PEntityOfEntIndex(int iEntIndex)
{
	if (iEntIndex >= 0 && iEntIndex < gpGlobals->maxEntities)
	{
		return (edict_t *)(gpGlobals->pEdicts + iEntIndex);
	}
	return NULL;
}


#define BEGIN_HOOK_CALLS(hooktype) \
	std::vector<HSCRIPT> *hookList = &hooks[hooktype]; \
	for(std::vector<HSCRIPT>::iterator it = hookList->begin(); it != hookList->end(); it++) \
	{ 

#define END_HOOK_CALLS			}


#define CALL_HOOKS0(hooktype) \
	std::vector<HSCRIPT> *hookList = &hooks[hooktype]; \
	for(std::vector<HSCRIPT>::iterator it = hookList->begin(); it != hookList->end(); it++) \
	{ \
		luavm->Call(*it); \
	}

#define CALL_HOOKS1(hooktype) \
	std::vector<HSCRIPT> *hookList = &hooks[hooktype]; \
	for(std::vector<HSCRIPT>::iterator it = hookList->begin(); it != hookList->end(); it++) \
	{ \
		luavm->Call(*it); \
	}

#define CALL_HOOKS2(hooktype) \
	std::vector<HSCRIPT> *hookList = &hooks[hooktype]; \
	for(std::vector<HSCRIPT>::iterator it = hookList->begin(); it != hookList->end(); it++) \
	{ \
		luavm->Call(*it); \
	}

#define CALL_HOOKS3(hooktype) \
	std::vector<HSCRIPT> *hookList = &hooks[hooktype]; \
	for(std::vector<HSCRIPT>::iterator it = hookList->begin(); it != hookList->end(); it++) \
	{ \
		luavm->Call(*it); \
	}

#define CALL_REG1_STACK5_VOID(func, reg1, arg1, stack1, stack2, stack3, stack4, stack5) \
	__asm mov reg1, arg1 \
	__asm push stack1 \
	__asm push stack2 \
	__asm push stack3 \
	__asm push stack4 \
	__asm push stack5 \
	__asm call func 

#define CALL_STACK3_VOID(func, stack1, stack2, stack3) \
	__asm push stack1 \
	__asm push stack2 \
	__asm push stack3 \
	__asm call func 

#define CALL_REG1_STACK1_RET(func, retreg, reg1, arg1, stack1) \
	__asm mov reg1, arg1 \
	__asm push stack1 \
	__asm call func \
	__asm mov retn, retreg

#define CALL_REG1_STACK2_VOID(func, reg1, arg1, stack1, stack2) \
	__asm mov reg1, arg1 \
	__asm push stack1 \
	__asm push stack2 \
	__asm call func

#define CALL_STACK1_VOID(func, stack1) \
	__asm push stack1 \
	__asm call func

#define CALL_REG2_STACK1_RET(func, retreg, reg1, arg1, reg2, arg2, stack1) \
	__asm mov reg1, arg1 \
	__asm mov reg2, arg2 \
	__asm push stack1 \
	__asm call func \
	__asm mov retn, retreg

#define CALL_REG2_STACK1_VOID(func, reg1, arg1, reg2, arg2, stack1) \
	__asm mov reg1, arg1 \
	__asm mov reg2, arg2 \
	__asm push stack1 \
	__asm call func 

#define CALL_REG1_RET(func, retreg, reg1, arg1) \
	__asm mov reg1, arg1 \
	__asm call func \
	__asm mov retn, retreg

#define START_NAKED_ARG1(reg1, var1) \
	__asm	push        ebp \
    __asm   mov         ebp, esp \
	__asm   sub         esp, 64 \
    __asm   mov         var1, reg1 \
	__asm	pushad \
	__asm	pushfd 
    

#define END_NAKED() \
	__asm	popfd \
	__asm	popad \
	__asm	leave \
	__asm	retn 

#define END_NAKED_RETURN(retreg, retvar) \
	__asm	popfd \
	__asm	popad \
	__asm	mov retreg, retvar \
	__asm	leave \
	__asm	retn 
