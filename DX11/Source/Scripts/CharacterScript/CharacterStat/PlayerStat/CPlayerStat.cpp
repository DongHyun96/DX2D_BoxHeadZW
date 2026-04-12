#include "pch.h"
#include "CPlayerStat.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"
#include "Source/Scripts/UIScript/CProgressBar.h"
#include "Source/Scripts/UIScript/InGameUIManager/CIngameUIManager.h"

CPlayerStat::CPlayerStat()
    : CCharacterStat(SCRIPT_TYPE::PLAYERSTAT)
{
}

CPlayerStat::~CPlayerStat()
{
}

void CPlayerStat::Begin()
{
    CCharacterStat::Begin();
}

void CPlayerStat::Tick()
{
    CCharacterStat::Tick();
}

bool CPlayerStat::TakeDamage(float _DamageAmount, GameObject* _DamageCauser)
{
    // 기본 Damage 입히기 처리 실패했다면 return false 
    if (!CCharacterStat::TakeDamage(_DamageAmount, _DamageCauser)) return false;
    
    // TODO : 테스트 때문에 무적기 걸어둠
    m_HP = max(1.f, m_HP);
    
    const PLAYER_MAINSTATE NextState = IsDead() ? PLAYER_MAINSTATE::DIE : PLAYER_MAINSTATE::PUSHED_OUT;
    const Ptr<CPlayerScript>& MainPlayerScript = GetOwner()->GetScriptComponent<CPlayerScript>();
    

    MainPlayerScript->SetMainState(NextState);
    GM->GetIngameUIManager()->GetAmmoCountUIAreaRef().HPBar->SetRatio(GetHP() / GetMaxHP());
    return true;
}

bool CPlayerStat::ApplyHeal(float _HealAmount)
{
    bool Result = CCharacterStat::ApplyHeal(_HealAmount);
    GM->GetIngameUIManager()->GetAmmoCountUIAreaRef().HPBar->SetRatio(GetHP() / GetMaxHP());
    return Result;
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
    CCharacterStat::SaveToLevelFile(_File);
}

void CPlayerStat::LoadFromLevelFile(FILE* _File)
{
    CCharacterStat::LoadFromLevelFile(_File);
}
