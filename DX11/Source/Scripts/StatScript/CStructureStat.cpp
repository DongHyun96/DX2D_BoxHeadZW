#include "pch.h"
#include "CStructureStat.h"

#include "Source/ScriptMgr.h"

CStructureStat::CStructureStat()
    : CStatScript(SCRIPT_TYPE::STRUCTURESTAT)
{
}

CStructureStat::~CStructureStat()
{
}

bool CStructureStat::TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos)
{
    if (!CStatScript::TakeDamage(_DamageAmount, _DamageSourcePos)) return false;
    
    if (IsDead()) Destroy();
    return true;
}
