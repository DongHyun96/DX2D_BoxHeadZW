#include "pch.h"
#include "CIngameUIManager.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "ImGui/imgui.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

#include "Source/Scripts/UIScript/CText.h"
#include "Source/Scripts/UIScript/CProgressBar.h"

CIngameUIManager::CIngameUIManager()
    : CScript(SCRIPT_TYPE::INGAMEUIMANAGER)
{
}

CIngameUIManager::~CIngameUIManager()
{
}

void CIngameUIManager::Begin()
{
    GameManager::GetInst()->SetIngameUIManager(this);
    InitMembers();
}

void CIngameUIManager::Tick()
{
}

void CIngameUIManager::InitMembers()
{
    Layer* UILayer = LevelMgr::GetInst()->GetCurLevel()->GetLayer(MAX_LAYER - 1);

    Ptr<GameObject> PlayerHUD{};
    
    for (const Ptr<GameObject>& RootObject : UILayer->GetParentObjects())
    {
        if (RootObject->GetName() == L"GameUI")
        {
            for (const Ptr<GameObject>& Child : RootObject->GetChildren())
                if (Child->GetName() == L"PlayerHUD") PlayerHUD = Child;
            break;
        }
    }

    if (!PlayerHUD) return;

    queue<Ptr<GameObject>> q{};
    q.push(PlayerHUD);

    while (!q.empty())
    {
        Ptr<GameObject> Current = q.front(); q.pop();

        const wstring& Name = Current->GetName();

        if (Name == L"AliveCountText")
        {
            m_ZombieAliveCount = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"RoundText")
        {
            m_RoundIndicators.RoundText = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"RoundWaitText") // Waiting for Next round...
        {
            m_RoundIndicators.RoundWaitText = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"RoundWaitTimeText")
        {
            m_RoundIndicators.RoundWaitTimeText = Current->GetScriptComponent<CText>().Get();
        }
        
        else if (Name == L"AmmoLeftText") //
        {
            m_AmmoCountUIArea.MainAmmoCount = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"GrenadeCountText") //
        {
            m_AmmoCountUIArea.GrenadeCount = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"AirStrikeCountText") //
        {
            m_AmmoCountUIArea.AirStrikeCount = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"HPBar") // 
        {
            m_AmmoCountUIArea.HPBar = Current->GetScriptComponent<CProgressBar>().Get();
        }
        else if (Name == L"RoundAmmoIcon") //
        {
            m_AmmoCountUIArea.RoundAmmoIcon = Current.Get();
        }
        
        // Weapon Icons
        else if (Name == L"PistolIcon")     m_AmmoCountUIArea.WeaponIcons[PLAYER_HANDSTATE::PISTOL] = Current.Get();
        else if (Name == L"UziIcon")        m_AmmoCountUIArea.WeaponIcons[PLAYER_HANDSTATE::UZI] = Current.Get();
        else if (Name == L"ShotgunIcon")    m_AmmoCountUIArea.WeaponIcons[PLAYER_HANDSTATE::SHOTGUN] = Current.Get();
        else if (Name == L"MingunIcon")    m_AmmoCountUIArea.WeaponIcons[PLAYER_HANDSTATE::MINIGUN] = Current.Get();
        else if (Name == L"RocketIcon")     m_AmmoCountUIArea.WeaponIcons[PLAYER_HANDSTATE::ROCKET] = Current.Get();

        // Structure Icons
        else if (Name == L"BarricadeIcon")      m_AmmoCountUIArea.StructureIcons[PLAYER_STRUCTURE_TYPE::BARRICADE] = Current.Get();
        else if (Name == L"BarrelIcon")         m_AmmoCountUIArea.StructureIcons[PLAYER_STRUCTURE_TYPE::BARREL] = Current.Get();
        else if (Name == L"Turret_MGIcon")      m_AmmoCountUIArea.StructureIcons[PLAYER_STRUCTURE_TYPE::TURRET_MACHINE_GUN] = Current.Get();
        else if (Name == L"Turret_MortarIcon")  m_AmmoCountUIArea.StructureIcons[PLAYER_STRUCTURE_TYPE::TURRET_MORTAR] = Current.Get();
        else if (Name == L"Turret_RocketIcon")  m_AmmoCountUIArea.StructureIcons[PLAYER_STRUCTURE_TYPE::TURRET_ROCKET] = Current.Get();

        for (const Ptr<GameObject>& Child : Current->GetChildren())
        {
            q.push(Child);
        }
    }
}

