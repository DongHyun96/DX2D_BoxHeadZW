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

bool CPlayerStat::ApplyBoost(float _BoostAmount)
{
    if (IsDead()) return false;
    
    m_Boost = min(m_Boost + _BoostAmount, m_BoostMax);
    return true;
}
