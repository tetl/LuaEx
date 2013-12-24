#include "bytecode.h"

ByteCode_t::~ByteCode_t()
{
#if defined _WIN32
	VirtualFree(pnBytes, NULL, MEM_RELEASE);
#else 
	free(pnBytes);
#endif

	return;
}

ByteCode_t::ByteCode_t(BYTE *pnBytes, size_t nSize, size_t nLength)
{
#if defined _WIN32
	this->pnBytes = (BYTE *)VirtualAlloc(NULL, nSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#else
	void *base = memalign(sysconf(_SC_PAGESIZE), nSize);
	if (mprotect(base, nSize, PROT_READ|PROT_WRITE|PROT_EXEC) != 0)
	{
		free(base);
		this->pnBytes = NULL;
	}
	this->pnBytes = (BYTE *)base;
#endif
	memcpy(this->pnBytes, pnBytes, nLength);

	this->nSize = nSize;
	this->nLength = nLength;

	return;
}

CByteCode::CByteCode()
{
	m_nHeadPosition = 0;
	m_nSize = PAGE_SIZE;
	m_pnBuffer = new BYTE[PAGE_SIZE];

	return;
}

CByteCode::~CByteCode()
{
	delete [] m_pnBuffer;

	return;
}

void CByteCode::Reset()
{
	delete [] m_pnBuffer;

	m_nHeadPosition = 0;
	m_nSize = PAGE_SIZE;
	m_pnBuffer = new BYTE[PAGE_SIZE];

	return;
}

void CByteCode::SetHeadPosition(size_t nNewPosition)
{
	m_nHeadPosition = nNewPosition;

	return;
}

ByteCode_t *CByteCode::Finalize(bool bReset)
{
	ByteCode_t *pRet = new ByteCode_t(m_pnBuffer, m_nSize, m_nHeadPosition);

	if (bReset)
	{
		Reset();
	}

	return pRet;
}

void CByteCode::CheckHead(size_t nWriteInSize)
{
	if (m_nHeadPosition + nWriteInSize >= m_nSize)
	{
		BYTE *pchBufferBackup = new BYTE[m_nSize];
		memcpy(pchBufferBackup, m_pnBuffer, m_nSize);
		
		delete [] m_pnBuffer;
		size_t nNewSize = m_nSize + PAGE_SIZE;
		m_pnBuffer = new BYTE[nNewSize];

		memcpy(m_pnBuffer, pchBufferBackup, m_nSize);

		m_nSize = nNewSize;

		delete [] pchBufferBackup;
	}

	return;
}

void CByteCode::WriteByte(BYTE nByte)
{
	m_pnBuffer[m_nHeadPosition] = nByte;
	m_nHeadPosition ++;

	return;
}

void CByteCode::WriteBytes(BYTE *pnBytes, size_t nSize)
{
	CheckHead(nSize);

	copy_bytes(pnBytes, (BYTE*)&m_pnBuffer[m_nHeadPosition], nSize);

	m_nHeadPosition += nSize;

	return;
}

void CByteCode::WriteAddress(void *pAddr)
{
	*(void **)(&m_pnBuffer[m_nHeadPosition]) = pAddr;
	m_nHeadPosition += 4;

	return;
}

void CByteCode::WritePush(BYTE nPrimary)
{
	CheckHead(1);

	WriteByte(0x50 + nPrimary);

	return;
}

void CByteCode::WriteMov(BYTE nByte)
{
	CheckHead(2);

	WriteByte(0x8B);
	WriteByte(nByte);

	return;
}

void CByteCode::WriteCall(void *pCallBack)
{
	CheckHead(5);

	WriteByte(0xE8);
	WriteAddress(pCallBack);

	return;
}

void CByteCode::WriteFarJmp(void *pCallBack)
{
	CheckHead(6);
	
	WriteByte(0xFF);
	WriteByte(0x25);
	WriteAddress(pCallBack);
	
	return;
}

void CByteCode::WriteNearJmp(void *pCallBack)
{
	CheckHead(5);
	
	WriteByte(0xE9);
	WriteAddress(pCallBack);
	
	return;
}

void CByteCode::WriteShortJmp(BYTE nPO, BYTE nByte)
{
	CheckHead(2);

	WriteByte(0x70 + nPO);
	WriteByte(nByte);

	return;
}

void CByteCode::WriteNop(size_t nCount)
{
	CheckHead(nCount);

	for (size_t i = 0; i < nCount; i ++)
	{
		WriteByte(0x90);
	}

	return;
}
