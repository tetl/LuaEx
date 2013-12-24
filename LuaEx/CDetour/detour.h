#ifndef _INCLUDE_ROUTINEEDITS_CDETOUR_H_
#define _INCLUDE_ROUTINEEDITS_CDETOUR_H_



#include "bytecode.h"

class CDetour
{
public :
	CDetour(BYTE *pTarget, void *pDetourFunction, void **pTrampoline, bool bEnable);
	~CDetour();

	void EnableDetour();
	void DisableDetour();
	bool IsEnabled();

private :
	void SetEnabled(bool bEnabled);

	CByteCode m_ByteCode;

	bool m_bEnabled;

	BYTE *m_pTarget;
	void *m_pDetourFunction;
	void *m_pReentryPoint;

	void **m_pTrampoline;

	ByteCode_t *m_pSavedBytes;
	ByteCode_t *m_pPatch;
	ByteCode_t *m_pReentryRoutine;
};

inline bool CDetour::IsEnabled()
{
	return this->m_bEnabled;
}

inline void CDetour::SetEnabled(bool bEnabled)
{
	this->m_bEnabled = bEnabled;

	return;
}

#endif//_INCLUDE_ROUTINEEDITS_CDETOUR_H_