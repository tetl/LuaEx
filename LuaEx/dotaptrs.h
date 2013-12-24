#pragma once
extern void* SetControllablePtr;
extern IScriptVM *luavm;

extern enum DetourResult
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

#define NONSTANDARD_CALL_REG2_STACK1_RET(func, retreg, reg1, arg1, reg2, arg2, stack1) \
	__asm mov reg1, arg1 \
	__asm mov reg2, arg2 \
	__asm push stack1 \
	__asm call func \
	__asm mov retn, retreg

#define NONSTANDARD_CALL_REG2_STACK1_VOID(func, reg1, arg1, reg2, arg2, stack1) \
	__asm mov reg1, arg1 \
	__asm mov reg2, arg2 \
	__asm push stack1 \
	__asm call func 

#define NONSTANDARD_CALL_REG1_RET(func, retreg, reg1, arg1) \
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
