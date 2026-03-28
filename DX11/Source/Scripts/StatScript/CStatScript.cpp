#include "pch.h"
#include "CStatScript.h"

#include "Source/ScriptMgr.h"

CStatScript::CStatScript()
    : CScript(static_cast<int>(SCRIPT_TYPE::STATSCRIPT))
{
}

CStatScript::CStatScript(SCRIPT_TYPE _ScriptType)
    : CScript(static_cast<int>(_ScriptType))
{
}

CStatScript::~CStatScript()
{
}

void CStatScript::Tick()
{
}

bool CStatScript::TakeDamage(float _DamageAmount, Vec3 _DamageSourcePos)
{
    if (IsDead()) return false; // 이미 사망처리된 캐릭터
    if (_DamageAmount <= 0.f) return false; // 잘못된 Damage량
    
    m_HP = max(m_HP - _DamageAmount, 0.f);
    return true;
}

bool CStatScript::ApplyHeal(float _HealAmount)
{
    if (IsDead()) return false; // 이미 죽은 상태이면 heal 처리 x TODO : Testing 환경이라면 이 장치 풀기

    m_HP = min(m_HP + _HealAmount, m_HPMax);
    return true;
}
