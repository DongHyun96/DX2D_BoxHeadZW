#include "pch.h"
#include "CIngameUIManager.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "ImGui/imgui.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

#include "Source/Scripts/UIScript/CText.h"

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

    for (const Ptr<GameObject> Object : UILayer->GetParentObjects())
    {
        queue<Ptr<GameObject>> q{};
        q.push(Object);
        
        while (!q.empty())
        {
            Ptr<GameObject> Current = q.front();  q.pop();

            if (Current->GetName() == L"AliveCountText")
            {
                m_ZombieAliveCount = Current->GetScriptComponent<CText>().Get(); 
            }

            
            else if (Current->GetName() == L"RoundText")
            {
                                
            }
            else if (Current->GetName() == L"RoundTimeText")
            {
                
            }
            
            
            else if (Current->GetName() == L"RoundIndicatorText")
            {
                
            }
            // ...
            

            for (const Ptr<GameObject>& Child : Current->GetChildren()) q.push(Child);
        }
    }
}

