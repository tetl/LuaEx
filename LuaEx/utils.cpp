#include "utils.h"
#include "luaex.h"
#include <Windows.h>

void *FindAddress(AddressType type, const char *sig, size_t len)
{
	bool found;
	char *ptr, *end;

	MEMORY_BASIC_INFORMATION mem;

	LPCVOID startAddr = NULL;
	switch (type)
	{
	case ADDR_SERVER:
		startAddr = g_SMAPI->GetServerFactory(false);
		break;
	case ADDR_ENGINE:
		startAddr = g_SMAPI->GetEngineFactory(false);
		break;
	}

	if (!startAddr)
		return NULL;

	if (!VirtualQuery(startAddr, &mem, sizeof(mem)))
		return NULL;

	IMAGE_DOS_HEADER *dos = reinterpret_cast<IMAGE_DOS_HEADER *>(mem.AllocationBase);
	IMAGE_NT_HEADERS *pe = reinterpret_cast<IMAGE_NT_HEADERS *>((intptr_t) dos + dos->e_lfanew);

	if (pe->Signature != IMAGE_NT_SIGNATURE)
	{
		// GetDllMemInfo failedpe points to a bad location
		return NULL;
	}

	ptr = reinterpret_cast<char *>(mem.AllocationBase);
	end = ptr + pe->OptionalHeader.SizeOfImage - len;

	while (ptr < end)
	{
		found = true;
		for (size_t i = 0; i < len; i++)
		{
			if (sig[i] != '\x2A' && sig[i] != ptr[i])
			{
				found = false;
				break;
			}
		}

		if (found)
			return ptr;

		ptr++;
	}

	return NULL;
}