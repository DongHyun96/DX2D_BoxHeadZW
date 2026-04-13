#include "pch.h"
#include "CPlayerStat.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"
#include "Source/Scripts/UIScript/CProgressBar.h"
#include "Source/Scripts/UIScript/InGameUIManager/CIngameUIManager.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include <algorithm>

#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Module/Util.h"

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

    m_HitHistory.clear();
    m_IsInvincible = false;
    m_InvincibleTimer = 0.f;
    m_FlickerTimer = 0.f;
}

void CPlayerStat::Tick()
{
    CCharacterStat::Tick();

    UpdateInvincibility();
}

bool CPlayerStat::TakeDamage(float _DamageAmount, GameObject* _DamageCauser)
{
    // 무적 상태라면 데미지를 입지 않음
    if (m_IsInvincible) return false;

    // 기본 Damage 입히기 처리 실패했다면 return false 
    if (!CCharacterStat::TakeDamage(_DamageAmount, _DamageCauser)) return false;
    
    // 피격 기록 추가 (1초 내의 피격만 관리)
    float currentTime = TIME;
    m_HitHistory.push_back(currentTime);

    // 1초가 지난 기록은 제거
    m_HitHistory.erase(std::remove_if(m_HitHistory.begin(), m_HitHistory.end(),
        [currentTime](float t) { return currentTime - t > 1.0f; }), m_HitHistory.end());

    // 1초 내에 3번 피격 시 무적 발동
    if (m_HitHistory.size() >= 3)
    {
        m_IsInvincible    = true;
        m_InvincibleTimer = m_InvincibleDuration;
        m_FlickerTimer    = 0.f;
        m_HitHistory.clear();
    }
    
    // TODO : 테스트용 무적
    if (m_bDebugInvincible)
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

void CPlayerStat::UpdateInvincibility()
{
    if (!m_IsInvincible) return;

    m_InvincibleTimer -= DT;
    m_FlickerTimer += DT;

    Ptr<CRenderComponent> pRenderCom = GetOwner()->GetRenderCom();

    if (m_InvincibleTimer <= 0.f)
    {
        m_IsInvincible = false;
        m_InvincibleTimer = 0.f;
        m_FlickerTimer = 0.f;

        // 원래 색상(흰색)으로 복구
        if (pRenderCom.Get())
        {
            Ptr<AMaterial> pMtrl = pRenderCom->CreateDynamicMaterial();
            if (pMtrl.Get())
            {
                pMtrl->SetScalar(VEC4_0, DEF_COLOR_WHITE);
            }
        }
    }
    else
    {
        // 0.1초 간격으로 색상 변경 (빨강 <-> 흰색)
        bool isRed = (int)(m_FlickerTimer * 10.f) % 2 == 0;
        Vec4 color = isRed ? DEF_COLOR_RED : DEF_COLOR_WHITE;

        if (pRenderCom.Get())
        {
            Ptr<AMaterial> pMtrl = pRenderCom->CreateDynamicMaterial();
            if (pMtrl.Get())
            {
                pMtrl->SetScalar(VEC4_0, color);
            }
        }
    }
}
