#ifndef _INCLUDE_ROUTINEEDITS_DEFINES_H_
#define _INCLUDE_ROUTINEEDITS_DEFINES_H_

#define	PAGE_SIZE	4096

#if defined PLATFORM_LINUX
#include <sys/mman.h>
#define ALIGN(ar) ((long)ar & ~(PAGE_SIZE-1))
#define	PAGE_EXECUTE_READWRITE	PROT_READ|PROT_WRITE|PROT_EXEC
#endif

#define DETOUR_MEMBER_CALL(name) (this->*name##_Actual)
#define DETOUR_STATIC_CALL(name) (name##_Actual)

#define DETOUR_DECL_NAKED(name, ret) \
ret (__stdcall*name##_Actual)() = NULL; \
__declspec(naked) ret name()

#define DETOUR_DECL_STATIC0(name, ret) \
ret (*name##_Actual)(void) = NULL; \
ret name(void)

#define DETOUR_DECL_STATIC1(name, ret, p1type, p1name) \
ret (*name##_Actual)(p1type) = NULL; \
ret name(p1type p1name)

#define DETOUR_DECL_NAKED(name, ret) \
ret (__stdcall*name##_Actual)() = NULL; \
__declspec(naked) ret name()

#define DETOUR_DECL_STATIC3(name, ret, p1type, p1name, p2type, p2name, p3type, p3name) \
ret (*name##_Actual)(p1type, p2type, p3type) = NULL; \
ret name(p1type p1name, p2type p2name, p3type p3name)

#define DETOUR_DECL_STATIC4(name, ret, p1type, p1name, p2type, p2name, p3type, p3name, p4type, p4name) \
ret (*name##_Actual)(p1type, p2type, p3type, p4type) = NULL; \
ret name(p1type p1name, p2type p2name, p3type p3name, p4type p4name)

#define DETOUR_DECL_STATIC5(name, ret, p1type, p1name, p2type, p2name, p3type, p3name, p4type, p4name, p5type, p5name) \
ret (*name##_Actual)(p1type, p2type, p3type, p4type, p5type) = NULL; \
ret name(p1type p1name, p2type p2name, p3type p3name, p4type p4name, p5type p5name)

#define DETOUR_DECL_MEMBER0(name, ret) \
class name##Class \
{ \
public: \
	ret name(); \
	static ret (name##Class::* name##_Actual)(void); \
}; \
ret (name##Class::* name##Class::name##_Actual)(void) = NULL; \
ret name##Class::name()

#define DETOUR_DECL_MEMBER1(name, ret, p1type, p1name) \
class name##Class \
{ \
public: \
	ret name(p1type p1name); \
	static ret (name##Class::* name##_Actual)(p1type); \
}; \
ret (name##Class::* name##Class::name##_Actual)(p1type) = NULL; \
ret name##Class::name(p1type p1name)

#define DETOUR_DECL_MEMBER1_void(name, p1type, p1name) \
class name##Class \
{ \
public: \
	void name(p1type p1name); \
	static void (name##Class::* name##_Actual)(p1type); \
}; \
void (name##Class::* name##Class::name##_Actual)(p1type) = NULL; \
void name##Class::name(p1type p1name)

#define DETOUR_DECL_MEMBER2(name, ret, p1type, p1name, p2type, p2name) \
class name##Class \
{ \
public: \
	ret name(p1type p1name, p2type p2name); \
	static ret (name##Class::* name##_Actual)(p1type, p2type); \
}; \
ret (name##Class::* name##Class::name##_Actual)(p1type, p2type) = NULL; \
ret name##Class::name(p1type p1name, p2type p2name)

#define DETOUR_DECL_MEMBER3(name, ret, p1type, p1name, p2type, p2name, p3type, p3name) \
class name##Class \
{ \
public: \
	ret name(p1type p1name, p2type p2name, p3type p3name); \
	static ret (name##Class::* name##_Actual)(p1type, p2type, p3type); \
}; \
ret (name##Class::* name##Class::name##_Actual)(p1type, p2type, p3type) = NULL; \
ret name##Class::name(p1type p1name, p2type p2name, p3type p3name)

#define DETOUR_DECL_MEMBER4(name, ret, p1type, p1name, p2type, p2name, p3type, p3name, p4type, p4name) \
class name##Class \
{ \
public: \
	ret name(p1type p1name, p2type p2name, p3type p3name, p4type p4name); \
	static ret (name##Class::* name##_Actual)(p1type, p2type, p3type, p4type); \
}; \
ret (name##Class::* name##Class::name##_Actual)(p1type, p2type, p3type, p4type) = NULL; \
ret name##Class::name(p1type p1name, p2type p2name, p3type p3name, p4type p4name)

#define DETOUR_DECL_MEMBER5(name, ret, p1type, p1name, p2type, p2name, p3type, p3name, p4type, p4name, p5type, p5name) \
class name##Class \
{ \
public: \
	ret name(p1type p1name, p2type p2name, p3type p3name, p4type p4name, p5type p5name); \
	static ret (name##Class::* name##_Actual)(p1type, p2type, p3type, p4type, p5type); \
}; \
ret (name##Class::* name##Class::name##_Actual)(p1type, p2type, p3type, p4type, p5type) = NULL; \
ret name##Class::name(p1type p1name, p2type p2name, p3type p3name, p4type p4name, p5type p5name)

#define DETOUR_DECL_MEMBER6(name, ret, p1type, p1name, p2type, p2name, p3type, p3name, p4type, p4name, p5type, p5name, p6type, p6name) \
class name##Class \
{ \
public: \
	ret name(p1type p1name, p2type p2name, p3type p3name, p4type p4name, p5type p5name, p6type p6name); \
	static ret (name##Class::* name##_Actual)(p1type, p2type, p3type, p4type, p5type, p6type); \
}; \
ret (name##Class::* name##Class::name##_Actual)(p1type, p2type, p3type, p4type, p5type, p6type) = NULL; \
ret name##Class::name(p1type p1name, p2type p2name, p3type p3name, p4type p4name, p5type p5name, p6type p6name)

#define DETOUR_DECL_MEMBER8(name, ret, p1type, p1name, p2type, p2name, p3type, p3name, p4type, p4name, p5type, p5name, p6type, p6name, p7type, p7name, p8type, p8name) \
class name##Class \
{ \
public: \
	ret name(p1type p1name, p2type p2name, p3type p3name, p4type p4name, p5type p5name, p6type p6name, p7type p7name, p8type p8name); \
	static ret (name##Class::* name##_Actual)(p1type, p2type, p3type, p4type, p5type, p6type, p7type, p8type); \
}; \
ret (name##Class::* name##Class::name##_Actual)(p1type, p2type, p3type, p4type, p5type, p6type, p7type, p8type) = NULL; \
ret name##Class::name(p1type p1name, p2type p2name, p3type p3name, p4type p4name, p5type p5name, p6type p6name, p7type p7name, p8type p8name)

#define GET_MEMBER_CALLBACK(name) (BYTE *)GetCodeAddress(&name##Class::name)
#define GET_MEMBER_TRAMPOLINE(name) (void **)(&name##Class::name##_Actual)

#define GET_STATIC_CALLBACK(name) (BYTE *)&name
#define GET_STATIC_TRAMPOLINE(name) (void **)&name##_Actual

#define DETOUR_CREATE_STATIC_PTR(name, ptr) CDetour((BYTE *)ptr, GET_STATIC_CALLBACK(name), GET_STATIC_TRAMPOLINE(name), false);
#define DETOUR_CREATE_MEMBER_PTR(name, ptr) CDetour((BYTE *)ptr, GET_MEMBER_CALLBACK(name), GET_MEMBER_TRAMPOLINE(name), false);

class GenericClass {};
typedef void (GenericClass::*VoidFunc)();

inline void *GetCodeAddr(VoidFunc mfp)
{
	return *(void **)&mfp;
}

#define GetCodeAddress(mfp) GetCodeAddr(reinterpret_cast<VoidFunc>(mfp))

#define INJECTION_STATIC_DECL_START(name, rettype, ...) \
	rettype __cdecl name(__VA_ARGS__) \
{ \
	__asm { \
		push ecx \
		push eax \
		push edx \
		push esp \
	}

#define INJECTION_MEMBER_DECL_START(name, rettype, ...) \
class name##Class \
{ \
public : \
	rettype name(__VA_ARGS__); \
}; \
rettype name##Class::name(__VA_ARGS__) \
{ \
	__asm nop __asm nop \
	__asm push ecx \
	__asm push eax \
	__asm push edx \
	__asm push esp 

#define INJECTION_DECL_END() \
	__asm pop esp \
	__asm pop edx \
	__asm pop eax \
	__asm pop ecx \
	__asm nop __asm nop __asm nop \
	__asm nop __asm nop __asm nop \
}

#endif//_INCLUDE_ROUTINEEDITS_DEFINES_H_