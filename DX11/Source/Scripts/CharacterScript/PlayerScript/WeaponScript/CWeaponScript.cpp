#include "pch.h"
#include "CWeaponScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"


vector<UINT> CWeaponScript::m_HitScanLayer{};

CWeaponScript::CWeaponScript(SCRIPT_TYPE _ScriptType)
    : CScript(_ScriptType)
{
}

CWeaponScript::~CWeaponScript()
{
}

void CWeaponScript::WeaponTick()
{
    m_TimeAfterLastFire = min(m_TimeAfterLastFire + DT, m_FireIntervalTime + 1.f);
}

