#include "detour.h"

CDetour::CDetour(BYTE *pTarget, void *pDetourFunction, void **pTrampoline, bool bEnable)
{
	m_pTarget = (BYTE *)pTarget;
	m_pDetourFunction = pDetourFunction;
	m_pTrampoline = pTrampoline;

	/*
	 * Patch which will be applied to the target function.
	 * The patch simply contains a jump to the detour function.
	 */
	m_ByteCode.WriteFarJmp(&m_pDetourFunction);
	m_pPatch = m_ByteCode.Finalize(true);
	
	SetEnabled(false);

	if (bEnable)
	{
		EnableDetour();
	}

	return;
}

CDetour::~CDetour()
{
	DisableDetour();

	delete m_pPatch;

	return;
}

void CDetour::EnableDetour()
{
	if (m_bEnabled)
	{
		return;
	}

	/*
	 * Figure out how many bytes we need to save
	 * from our target function and calculate the
	 * re-entry point.
	 */
	int nSavedBytes = copy_bytes(m_pTarget, NULL, 6);
	m_pReentryPoint = (BYTE *)(m_pTarget + nSavedBytes);

	/* 
	 * Save the bytes.
	 */
	m_ByteCode.WriteBytes(m_pTarget, nSavedBytes);
	m_pSavedBytes = m_ByteCode.Finalize(true);
	
	/*
	 * Trampoline:
	 * Execute saved bytes, then jump back into original function.
	 */
	m_ByteCode.WriteBytes(m_pTarget, nSavedBytes);
	m_ByteCode.WriteFarJmp(&m_pReentryPoint);
	m_pReentryRoutine = m_ByteCode.Finalize(true);
	*m_pTrampoline = m_pReentryRoutine->pnBytes;

	/*
	 * Apply 'jump' patch to target function.
	 */
#if defined _LINUX
	void *m_pTarget2 = (void *)ALIGN(m_pTarget);
	mprotect(m_pTarget2, sysconf(_SC_PAGESIZE), PAGE_EXECUTE_READWRITE);
#elif defined _WIN32
	DWORD old_prot;
	VirtualProtect(m_pTarget, m_pPatch->nLength, PAGE_EXECUTE_READWRITE, &old_prot);
#endif
	memcpy(m_pTarget, m_pPatch->pnBytes, m_pPatch->nLength);

	SetEnabled(true);
	return;
}

void CDetour::DisableDetour()
{
	if (!m_bEnabled)
	{
		return;
	}
	
	/*
	 * Remove 'jump' patch from target function.
	 */
#if defined _LINUX
	void *m_pTarget2 = (void *)ALIGN(m_pTarget);
	mprotect(m_pTarget2, sysconf(_SC_PAGESIZE), PAGE_EXECUTE_READWRITE);
#elif defined _WIN32
	DWORD old_prot;
	VirtualProtect(m_pTarget, m_pSavedBytes->nLength, PAGE_EXECUTE_READWRITE, &old_prot);
#endif
	memcpy(m_pTarget, m_pSavedBytes->pnBytes, m_pSavedBytes->nLength);

	delete m_pSavedBytes;
	delete m_pReentryRoutine;
	
	SetEnabled(false);
	return;
}