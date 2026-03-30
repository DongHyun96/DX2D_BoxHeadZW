#include "pch.h"
#include "CStatScript.h"

#include "Source/ScriptMgr.h"
#include "Source/Scripts/CharacterScript/CCharacterAnimHandler.h"
#include "Source/Scripts/CharacterScript/CCharacterScript.h"

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

bool CStatScript::TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos)
{
    if (IsDead()) return false; // 이미 사망처리된 캐릭터
    if (_DamageAmount <= 0.f) return false; // 잘못된 Damage량
    
    m_HP = max(m_HP - _DamageAmount, 0.f);
    
    // PushedOut 이후 Dead 처리가 이루어져야 함
    if (const Ptr<CCharacterScript>& MainCharacterScript = GetOwner()->GetScriptComponent<CCharacterScript>())
    {
        Vec2 PushedOutFaceDirection = _DamageSourcePos - ToVec2(Transform()->GetWorldPos());
        PushedOutFaceDirection.Normalize();
        
        // PushedOut의 경우, PushedOut Animation(정확히는 밀린 장면 Stop 시간 얼마나 보여줄지) time rewind 처리를 여기서 해주어야 한다
        MainCharacterScript->RewindPushedOut(PushedOutFaceDirection);
        GetOwner()->GetScriptComponent<CCharacterAnimHandler>()->RewindPushedOut(PushedOutFaceDirection);
    }
    
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
