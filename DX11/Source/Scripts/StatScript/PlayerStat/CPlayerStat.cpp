#include "pch.h"
#include "CPlayerStat.h"

#include "Source/ScriptMgr.h"

CPlayerStat::CPlayerStat()
    : CStatScript(SCRIPT_TYPE::PLAYERSTAT)
{
}

CPlayerStat::~CPlayerStat()
{
}

void CPlayerStat::Tick()
{
    CStatScript::Tick();
}

bool CPlayerStat::TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos)
{
    // 기본 Damage 입히기 처리 실패했다면 return false 
    if (!CStatScript::TakeDamage(_DamageAmount, _DamageSourcePos)) return false;

    
    
    return true;
}

bool CPlayerStat::ApplyBoost(float _BoostAmount)
{
    if (IsDead()) return false;
    
    m_Boost = min(m_Boost + _BoostAmount, m_BoostMax);
    return true;
}

// TODO : 나중에 전체 저장 및 게임 불러오기 처리로 할거면 제대로 구현할 것
void CPlayerStat::SaveToLevelFile(FILE* _File)
{
    CStatScript::SaveToLevelFile(_File);
}

void CPlayerStat::LoadFromLevelFile(FILE* _File)
{
    CStatScript::LoadFromLevelFile(_File);
}
