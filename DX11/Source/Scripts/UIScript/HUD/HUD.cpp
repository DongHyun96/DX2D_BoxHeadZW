#include "pch.h"
#include "HUD.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/RoundHandler/CRoundHandler.h"
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
