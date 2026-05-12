#include "pch.h"
#include "CIngameUIManager.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "ImGui/imgui.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

#include "Source/Scripts/UIScript/CText.h"
#include "Source/Scripts/UIScript/CProgressBar.h"
// #include "Source/Scripts/UIScript/HUD/HUD.h"

CIngameUIManager::CIngameUIManager()
    : CScript(SCRIPT_TYPE::INGAMEUIMANAGER)
{
    
}

CIngameUIManager::CIngameUIManager(const CIngameUIManager& _Origin)
    : CScript(_Origin)
    , m_AmmoCountUIArea(nullptr)
    , m_RoundIndicators(nullptr)
    , m_ZombieAliveCount(nullptr)
    , m_CrossHair(nullptr)
    , m_AccTime(0.f)
    , m_bPrevGameStart(false)
{
}

CIngameUIManager::~CIngameUIManager()
{
    m_AmmoCountUIArea = nullptr;
    m_RoundIndicators = nullptr;
}

void CIngameUIManager::Begin()
{
    GameManager::GetInst()->SetIngameUIManager(this);
    InitMembers();
}

void CIngameUIManager::Tick()
{
    bool bGameStart = GameManager::GetInst()->GetIsGameStart();

    if (!bGameStart)
    {
        m_AccTime += DT;

        if (m_RoundIndicators->RoundWaitText)
        {
            // 게임 시작 전이면 "Enterkey to start" 텍스트 표시 및 깜빡임 연출
            m_RoundIndicators->RoundWaitText->SetText(L"EnterKey to start");
            m_RoundIndicators->RoundWaitText->Transform()->SetRelativePosY(-150.f); // 화면 중앙보다 살짝 하단
            
            float Alpha = (sinf(m_AccTime * 5.f) + 1.f) * 0.5f;
            m_RoundIndicators->RoundWaitText->SetAlpha(Alpha);
        }
    }
    else
    {
        // 게임이 시작된 직후 한 번만 원래 상태로 복구 시도 (또는 라운드 시스템에 맡김)
        if (!m_bPrevGameStart)
        {
            if (m_RoundIndicators->RoundWaitText)
            {
                m_RoundIndicators->RoundWaitText->SetText(L"Waiting for next round...");
                m_RoundIndicators->RoundWaitText->SetAlpha(0.f);
                // 위치는 OnRoundWaitStart 등에서 다시 잡힐 것이므로 굳이 여기서 초기화 안 해도 됨
            }
        }
    }

    m_bPrevGameStart = bGameStart;
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

    if (!m_AmmoCountUIArea) m_AmmoCountUIArea = new AmmoCountUIArea;
    if (!m_RoundIndicators) m_RoundIndicators = new RoundIndicators;

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
            m_RoundIndicators->RoundText = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"RoundWaitText") // Waiting for Next round...
        {
            m_RoundIndicators->RoundWaitText = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"RoundWaitTimeText")
        {
            m_RoundIndicators->RoundWaitTimeText = Current->GetScriptComponent<CText>().Get();
        }
        
        else if (Name == L"AmmoLeftText") //
        {
            m_AmmoCountUIArea->MainAmmoCount = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"GrenadeCountText") //
        {
            m_AmmoCountUIArea->GrenadeCount = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"AirStrikeCountText") //
        {
            m_AmmoCountUIArea->AirStrikeCount = Current->GetScriptComponent<CText>().Get();
        }
        else if (Name == L"HPBar") // 
        {
            m_AmmoCountUIArea->HPBar = Current->GetScriptComponent<CProgressBar>().Get();
        }
        else if (Name == L"RoundAmmoIcon") //
        {
            m_AmmoCountUIArea->RoundAmmoIcon = Current.Get();
        }
        
        // Weapon Icons
        else if (Name == L"PistolIcon")     m_AmmoCountUIArea->WeaponIcons[PLAYER_HANDSTATE::PISTOL] = Current.Get();
        else if (Name == L"UziIcon")        m_AmmoCountUIArea->WeaponIcons[PLAYER_HANDSTATE::UZI] = Current.Get();
        else if (Name == L"ShotgunIcon")    m_AmmoCountUIArea->WeaponIcons[PLAYER_HANDSTATE::SHOTGUN] = Current.Get();
        else if (Name == L"MingunIcon")    m_AmmoCountUIArea->WeaponIcons[PLAYER_HANDSTATE::MINIGUN] = Current.Get();
        else if (Name == L"RocketIcon")     m_AmmoCountUIArea->WeaponIcons[PLAYER_HANDSTATE::ROCKET] = Current.Get();

        // Structure Icons
        else if (Name == L"BarricadeIcon")      m_AmmoCountUIArea->StructureIcons[PLAYER_STRUCTURE_TYPE::BARRICADE] = Current.Get();
        else if (Name == L"BarrelIcon")         m_AmmoCountUIArea->StructureIcons[PLAYER_STRUCTURE_TYPE::BARREL] = Current.Get();
        else if (Name == L"Turret_MGIcon")      m_AmmoCountUIArea->StructureIcons[PLAYER_STRUCTURE_TYPE::TURRET_MACHINE_GUN] = Current.Get();
        else if (Name == L"Turret_MortarIcon")  m_AmmoCountUIArea->StructureIcons[PLAYER_STRUCTURE_TYPE::TURRET_MORTAR] = Current.Get();
        else if (Name == L"Turret_RocketIcon")  m_AmmoCountUIArea->StructureIcons[PLAYER_STRUCTURE_TYPE::TURRET_ROCKET] = Current.Get();

        for (const Ptr<GameObject>& Child : Current->GetChildren())
        {
            q.push(Child);
        }
    }
}

