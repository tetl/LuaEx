#include <ISmmPlugin.h>
#include <vscript/ivscript.h>
#include "dotaptrs.h"

class Ex
{
public:
	DetourHandler IsDeniableCalled();
	void IsDeniable(HSCRIPT npc);
	void SetUnitControllableByPlayer(HSCRIPT npc, int playerId, bool something); 
};
