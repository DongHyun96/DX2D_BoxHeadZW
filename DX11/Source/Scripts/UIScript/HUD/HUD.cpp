#include "pch.h"
#include "HUD.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/RoundHandler/CRoundHandler.h"
#include "Source/Scripts/UIScript/CProgressBar.h"
#include "Source/Scripts/UIScript/CText.h"
#include "Source/Scripts/UIScript/InGameUIManager/CIngameUIManager.h"
#include "Source/Scripts/UIScript/UIAnimation/CUIAnimationGroup.h"

void AmmoCountUIArea::UpdateToGun(PLAYER_HANDSTATE _HandState, int _MainAmmoCount)
{
    for (auto weapon_icon : WeaponIcons)
        weapon_icon.second->SetActive(false);
        
    for (auto structure_icon : StructureIcons)
        structure_icon.second->SetActive(false);
        
    WeaponIcons[_HandState]->SetActive(true);
    RoundAmmoIcon->SetActive(true);
        
    if (_HandState == PLAYER_HANDSTATE::PISTOL)
    {
        MainAmmoCount->SetText(L"INF");
        MainAmmoCount->SetColor(DEF_COLOR_WHITE);
    }
    else UpdateCurrentAmmoCount(_MainAmmoCount);
}

void AmmoCountUIArea::UpdateCurrentAmmoCount(int _AmmoCount)
{
    MainAmmoCount->SetText(to_wstring(_AmmoCount));
    MainAmmoCount->SetColor((_AmmoCount <= 0) ? Vec4(1.f, 0.f, 0.f, 1.f) : DEF_COLOR_WHITE);
}

void AmmoCountUIArea::UpdateToStructure(PLAYER_STRUCTURE_TYPE _Type, int _Count)
{
    for (auto weapon_icon : WeaponIcons)
        weapon_icon.second->SetActive(false);
        
    for (auto structure_icon : StructureIcons)
        structure_icon.second->SetActive(false);
        
    RoundAmmoIcon->SetActive(false);
        
    StructureIcons[_Type]->SetActive(true);

    UpdateCurrentAmmoCount(_Count);
    
    // Game Log 추가
    switch (_Type) 
    {
    case PLAYER_STRUCTURE_TYPE::BARRICADE:          GM->GetIngameUIManager()->AddGameLog(L"SWITCHED TO BARRICADE");     return;
    case PLAYER_STRUCTURE_TYPE::BARREL:             GM->GetIngameUIManager()->AddGameLog(L"SWITCHED TO BARREL");        return;
    case PLAYER_STRUCTURE_TYPE::TURRET_MACHINE_GUN: GM->GetIngameUIManager()->AddGameLog(L"SWITCHED TO TURRET_MG");     return;
    case PLAYER_STRUCTURE_TYPE::TURRET_MORTAR:      GM->GetIngameUIManager()->AddGameLog(L"SWITCHED TO TURRET_MORTAR"); return;
    case PLAYER_STRUCTURE_TYPE::TURRET_ROCKET:      GM->GetIngameUIManager()->AddGameLog(L"SWITCHED TO TURRET_ROCKET"); return;
    }
}

void AmmoCountUIArea::UpdateCurrentGrenadeCount(int _Count)
{
    const wstring CountText = L"X " + to_wstring(_Count);
    GrenadeCount->SetText(CountText);
    GrenadeCount->SetColor((_Count <= 0) ? DEF_COLOR_RED : DEF_COLOR_WHITE);
}

void AmmoCountUIArea::UpdateCurrentAirStrikeCount(int _Count)
{
    const wstring CountText = L"X " + to_wstring(_Count);
    AirStrikeCount->SetText(CountText);
    AirStrikeCount->SetColor((_Count <= 0) ? DEF_COLOR_RED : DEF_COLOR_WHITE);
}

bool AmmoCountUIArea::UpdateHPBar(float _HP, float _MaxHP)
{
    if (_MaxHP <= 0.f) return false;

    // 힐 적용인지 Damage 입는지에 따라 처리
    const float PrevRatio = HPBar->GetRatio();
    const float NewRatio  = _HP / _MaxHP;

    // MainHPBar의 경우, 힐 적용이 되었든, Damage처리가 되었든 LerpDest 적용은 동일
    MainHPBarRatioDest = NewRatio;

    if (NewRatio > PrevRatio) // 힐 적용
    {
        // Damage Bar 관련 처리
        HPBarDamage->SetRatio(0.f);     // 아예 DamageBar 안보이게끔 처리
        bEnableDamageBarLerp = false;   // DamageBar Lerp 적용 끄기
    }
    else // Damage를 입었을 때
    {
        HPBarDamage->SetRatio(PrevRatio); // 이전 HPBar의 Ratio부터 시작해서 DamageBar Lerp 처리 시작
        bEnableDamageBarLerp = true;
        DamageHPBarRatioDest = NewRatio;
    }
    
    return true;
}

void AmmoCountUIArea::Tick()
{
    // Damage Bar Lerp 처리
    if (bEnableDamageBarLerp)
        LerpProgressBar(HPBarDamage, 5.f, DamageHPBarRatioDest);
    
    // Main HP Bar Lerp 처리
    LerpProgressBar(HPBar, 12.f, MainHPBarRatioDest);
}

void AmmoCountUIArea::LerpProgressBar(CProgressBar* _TargetProgressBar, float _LerpAlphaSpeed, float _DestRatio)
{
    float Ratio = _TargetProgressBar->GetRatio();
    Ratio = Lerp(Ratio, _DestRatio, DT * _LerpAlphaSpeed);
    _TargetProgressBar->SetRatio(Ratio);
}

void RoundIndicators::OnRoundStateChanged(ROUND_STATE _NextRoundState)
{
    switch (_NextRoundState)
    {
    case ROUND_STATE::WAIT_FOR_GAMESTART:
    {
        RoundWaitText->SetText(L"EnterKey to start");
        RoundIndicatorAnimGroup->PlayAnimation(L"WaitingGameStartAnim", UIAnimEndHandling::LOOP);
    }
        break;
    case ROUND_STATE::WAIT_FOR_ROUNDSTART:
        RoundIndicatorAnimGroup->PlayAnimation(L"RoundWaitingStartAnim");
        RoundWaitText->SetText(L"Waiting for next round...");
        break;
    case ROUND_STATE::ROUND_GOING:
    {
        RoundIndicatorAnimGroup->PlayAnimation(L"RoundStartAnim");
        RoundText->SetText(L"Round  " + to_wstring(GM->GetRoundHandler()->GetRoundNumber()));  
    }
        break;
    case ROUND_STATE::GAME_OVER:
    {
        RoundIndicatorAnimGroup->PlayAnimation(L"GameOverAnim");
        RoundText->SetText(L"Game Over");
    }
        break;
    }
}

void RoundIndicators::SetRoundWaitingDisplaySec(float RoundWaitTime)
{
    int RoundWaitTimeToInt = static_cast<int>(RoundWaitTime);
    RoundWaitTimeText->SetText(to_wstring(RoundWaitTimeToInt + 1));
}
