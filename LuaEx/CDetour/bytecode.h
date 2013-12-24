#ifndef _INCLUDE_CBYTECODE_H_
#define _INCLUDE_CBYTECODE_H_

#include "asm.h"
#include "defines.h"
#include <Windows.h>

typedef unsigned char       BYTE;
#define NULL 0

/*
 * This shit is a class that creates code in hex then converts it to a pointer of executable memory
 * Used in my CDetour and CInject
 */

struct ByteCode_t
{
	ByteCode_t(BYTE *pnBytes, size_t nSize, size_t nLength);
	~ByteCode_t();

	BYTE *pnBytes;
	size_t nSize;
	size_t nLength;
};

class CInject;

class CByteCode
{
	friend CInject;

public :
	CByteCode();
	~CByteCode();

	void Reset();
	void SetHeadPosition(size_t nNewPosition);
	ByteCode_t *Finalize(bool bReset);

	void WriteByte(BYTE nByte);
	void WriteBytes(BYTE *pnBytes, size_t nSize);
	void WriteAddress(void *pAddr);
	
	void WritePush(BYTE nPrimary);
	void WriteMov(BYTE nByte);
	void WriteCall(void *pCallBack);
	void WriteFarJmp(void *pCallBack);
	void WriteNearJmp(void *pCallBack);
	void WriteShortJmp(BYTE nPO, BYTE nByte);
	void WriteNop(size_t nCount = 1);

private :
	void CheckHead(size_t nWriteInSize);

	size_t m_nHeadPosition;
	size_t m_nSize;
	BYTE *m_pnBuffer;
};

#endif//_INCLUDE_CBYTECODE_H_