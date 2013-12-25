#include <ISmmPlugin.h>
#include <vscript/ivscript.h>
#include "dotaptrs.h"

class Ex
{
public:
	Ex();
	DetourHandler IsDeniableCalled();
	void IsDeniable(HSCRIPT npc);
	void SetUnitControllableByPlayer(HSCRIPT npc, int playerId, bool something); 

public:
	inline HSCRIPT GetHScript() const
	{
		return m_hScope;
	}

	inline void SetHScript(HSCRIPT scope)
	{
		m_hScope = scope;
	}

private:
	HSCRIPT m_hScope;
};
