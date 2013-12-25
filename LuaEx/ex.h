#include <ISmmPlugin.h>
#include <vscript/ivscript.h>
#include "dotaptrs.h"

class ScriptExtension
{
public:
	ScriptExtension();

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

class ExUnit : public ScriptExtension
{
public:
	void SetControllableByPlayer(HSCRIPT npc, int playerId, bool something);
};

class ExAbility : public ScriptExtension
{
public:
	void EndCooldown(HSCRIPT ability);
};

class Ex : public ScriptExtension
{
public:
	void ApplyDamage(HSCRIPT attackingUnit, HSCRIPT damagedUnit, HSCRIPT ability, float damage, int typeField);
};
