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

void CStatScript::Init()
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_HP, L"CurrentHP", false, 1.f, false);
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_HPMax, L"MaxHP", false, 1.f, true);
}

void CStatScript::Begin()
{
    // TODO : 게임 전체 불러오기, 저장 처리로 한다면 이 Line 지울것
    m_HP = m_HPMax;
}

void CStatScript::Tick()
{
}

void CStatScript::InitSpawn()
{
    m_HP = m_HPMax;
}

bool CStatScript::TakeDamage(float _DamageAmount, GameObject* _DamageCauser)
{
    if (IsDead()) return false; // 이미 사망처리된 오브젝트
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

void CStatScript::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_HP, sizeof(float), 1, _File);
    fwrite(&m_HPMax, sizeof(float), 1, _File);
}

void CStatScript::LoadFromLevelFile(FILE* _File)
{
    fread(&m_HP, sizeof(float), 1, _File);
    fread(&m_HPMax, sizeof(float), 1, _File);
}
