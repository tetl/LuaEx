#include <ISmmPlugin.h>
#include <vscript/ivscript.h>
#include "dotaptrs.h"
#include <list>

// These were taken from vscript_shared.h (s/ENT/EX/). They impl the nice virtual GetScriptDesc for us
#define DECLARE_EX_SCRIPTDESC()													ALLOW_SCRIPT_ACCESS(); virtual ScriptClassDesc_t *GetScriptDesc()

#define BEGIN_EX_SCRIPTDESC( className, baseClass, description )					_IMPLEMENT_EX_SCRIPTDESC_ACCESSOR( className ); BEGIN_SCRIPTDESC( className, baseClass, description )
#define BEGIN_EX_SCRIPTDESC_ROOT( className, description )							_IMPLEMENT_EX_SCRIPTDESC_ACCESSOR( className ); BEGIN_SCRIPTDESC_ROOT( className, description )
#define BEGIN_EX_SCRIPTDESC_NAMED( className, baseClass, scriptName, description )	_IMPLEMENT_EX_SCRIPTDESC_ACCESSOR( className ); BEGIN_SCRIPTDESC_NAMED( className, baseClass, scriptName, description )
#define BEGIN_EX_SCRIPTDESC_ROOT_NAMED( className, scriptName, description )		_IMPLEMENT_EX_SCRIPTDESC_ACCESSOR( className ); BEGIN_SCRIPTDESC_ROOT_NAMED( className, scriptName, description )

#define _IMPLEMENT_EX_SCRIPTDESC_ACCESSOR( className )					template <> ScriptClassDesc_t * GetScriptDesc<className>( className * ); ScriptClassDesc_t *className::GetScriptDesc()  { return ::GetScriptDesc( this ); }		
//

class ScriptExtension
{
public:
	ScriptExtension();

public:
	virtual void Init() {}
	virtual void Shutdown() {}

	virtual const char *GetInstanceName() const = 0;

	virtual ScriptClassDesc_t *GetScriptDesc() = 0;

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

std::list<ScriptExtension *> &ScriptExtensions();

// TODO: split these to separate files

class ExUnit : public ScriptExtension
{
	DECLARE_EX_SCRIPTDESC();
public:
	const char *GetInstanceName() const override { return "ExUnit"; }
public:
	void SetControllableByPlayer(HSCRIPT npc, int playerId, bool something);
};
static ExUnit s_exUnit;


class ExAbility : public ScriptExtension
{
	DECLARE_EX_SCRIPTDESC();
public:
	const char *GetInstanceName() const override { return "ExAbility"; }
public:
	void EndCooldown(HSCRIPT ability);
};
static ExAbility s_exAbility;


class Ex : public ScriptExtension
{
	DECLARE_EX_SCRIPTDESC();
public:
	const char *GetInstanceName() const override { return "Ex"; }
public:
	void ApplyDamage(HSCRIPT attackingUnit, HSCRIPT damagedUnit, HSCRIPT ability, float damage, int typeField);
};
static Ex s_ex;
