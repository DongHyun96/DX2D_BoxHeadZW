#include "pch.h"
#include "Menu.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/07.EditorUI/01.Inspector/Inspector.h"
#include "GameEngine/07.EditorUI/06.ContentUI/ContentUI.h"
#include "Source/ScriptMgr.h"

Menu::Menu()
: EditorUI("Menu")
{
}

Menu::~Menu()
{
}

void Menu::Tick()
{
    if (ImGui::BeginMainMenuBar())
    {
        FileTick();

        LevelTick();
        
        ViewTick();

        GameObjectTick();

        AssetTick();
        
        CollisionMatrixTick();
        
        ImGui::EndMainMenuBar();
    }
}

void Menu::Tick_UI()
{
}

void Menu::FileTick()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Save All", "Ctrl + Shit + S"))
        {
            TrySaveAllAssets();
            TrySaveCurrentLevel();
        }
        
        if (ImGui::MenuItem("Save current Level", "Ctrl+S"))
            TrySaveCurrentLevel();
        
        
        ImGui::EndMenu();
    }
}

void Menu::LevelTick()
{
    // Level Play 단축키 처리 (Alt + P 조합)
    ImGuiIO& io = ImGui::GetIO();
    const ImGuiInputFlags flags = ImGuiInputFlags_RouteGlobal;
    
    if (!io.WantTextInput && ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_P, flags))
    {
        if (LevelMgr::GetInst()->GetCurLevel() &&
            LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP)
            ChangeLevelState(LEVEL_STATE::PLAY);
    }
    
    // Level Stop 단축키 처리 (esc)
    if (!io.WantTextInput && ImGui::Shortcut(ImGuiKey_Escape, flags))
    {
        if (LevelMgr::GetInst()->GetCurLevel() &&
            LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP)
            ChangeLevelState(LEVEL_STATE::STOP);
    }

    
    if (ImGui::BeginMenu("Level"))
    {
        bool HasLevel = LevelMgr::GetInst()->GetCurLevel().Get();
        
        bool IsPlay{}, IsPause{}, IsStop{};
        
        if (HasLevel)
        {
            LEVEL_STATE CurState = LevelMgr::GetInst()->GetLevelState(); 
            IsPlay  = CurState  == LEVEL_STATE::PLAY;
            IsPause = CurState == LEVEL_STATE::PAUSE;
            IsStop  = CurState == LEVEL_STATE::STOP;
        }
        
        if (ImGui::MenuItem("Play", nullptr, nullptr, HasLevel && !IsPlay))
            ChangeLevelState(LEVEL_STATE::PLAY);
            
        if (ImGui::MenuItem("Pause", nullptr, nullptr, HasLevel && IsPlay))
            ChangeLevelState(LEVEL_STATE::PAUSE);
        
        if (ImGui::MenuItem("Stop", nullptr, nullptr, HasLevel && (IsPlay || IsPause)))
            ChangeLevelState(LEVEL_STATE::STOP);
            
        ImGui::EndMenu();
    }
}

void Menu::ViewTick()
{
    if (ImGui::BeginMenu("View"))
    {
        bool ShowDemo = EditorMgr::GetInst()->IsShowDemo();
            
        if (ImGui::MenuItem("Demo", nullptr, &ShowDemo, true))
        {
            EditorMgr::GetInst()->ShowDemo(ShowDemo);
        }

        // Ptr<EditorUI> pInspector = EditorMgr::GetInst()->FindUI("Inspector");
        // bool InspectorActive = pInspector->IsActive();
            
        Ptr<EditorUI> pOutliner = EditorMgr::GetInst()->FindUI("Outliner");
        bool OutlinerActive = pOutliner->IsActive();
            
        if (ImGui::MenuItem("Outliner", nullptr, &OutlinerActive, true))
        {
            pOutliner->SetActive(OutlinerActive);
        }

        Ptr<EditorUI> pDebugLogUI = EditorMgr::GetInst()->FindUI("DebugLogUI"); 
        bool DebugLogUIActive = pDebugLogUI->IsActive();
        
        if (ImGui::MenuItem("DebugLogger", nullptr, &DebugLogUIActive, true))
            pDebugLogUI->SetActive(DebugLogUIActive);
        
        if (ImGui::MenuItem("Add Inspector"))
            EditorMgr::GetInst()->AddInspector();
            
            
        ImGui::EndMenu();
    }
}

void Menu::GameObjectTick()
{
    if (ImGui::BeginMenu("GameObject"))
    {
            
		if (ImGui::MenuItem("Add new GameObject"))
		{
		    Ptr<GameObject> NewGameObject = new GameObject;
		    NewGameObject->SetName(L"Default Object");
		    
		    if (LevelMgr::GetInst()->GetCurLevel() && LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP)
		    {
		        NewGameObject->m_LayerIdx = 0; // Default Layer 부여
		        LevelMgr::GetInst()->GetCurLevel()->AddObject(0, NewGameObject);
		        LevelMgr::GetInst()->GetCurLevel()->SetChanged();
		    }
		}
        
        ImGui::EndMenu();
    }
}

void Menu::AssetTick()
{
    if (ImGui::BeginMenu("Asset"))
    {
        if (ImGui::BeginMenu("Create Asset"))
        {
            Ptr<Asset> NewAsset{};
            if (ImGui::MenuItem("Create Material"))
                NewAsset = AssetMgr::GetInst()->CreateNewAsset<AMaterial>(L"Default Material").Get();

            if (ImGui::MenuItem("Create Sprite"))
                NewAsset = AssetMgr::GetInst()->CreateNewAsset<ASprite>(L"Default Sprite").Get();

            if (ImGui::MenuItem("Create Flipbook"))
                NewAsset = AssetMgr::GetInst()->CreateNewAsset<AFlipbook>(L"Default Flipbook").Get();

            if (ImGui::MenuItem("Create TileMap"))
                NewAsset = AssetMgr::GetInst()->CreateNewAsset<ATileMap>(L"Default TileMap").Get();
            
            if (ImGui::MenuItem("Create Level"))
                NewAsset = AssetMgr::GetInst()->CreateNewAsset<ALevel>(L"Default Level").Get();

            // 새로운 Asset 생성 성공, AssetMgr에 새로이 생성된 Asset 등록 처리
            if (NewAsset) AssetMgr::GetInst()->AddAsset(NewAsset->GetKey(), NewAsset);
            
            ImGui::EndMenu();
        }	

        ImGui::EndMenu();
    }
}

void Menu::CollisionMatrixTick()
{
    if (ImGui::BeginMenu("Edit Collision"))
    {
        if (ImGui::MenuItem("Edit CollisionMat"))
        {
            Ptr<EditorUI> CollisionMatUI = EditorMgr::GetInst()->FindUI("CollisionMatrixUI");
            CollisionMatUI->SetActive(true);
        }
        
        ImGui::EndMenu();
    }
}

void Menu::TrySaveAllAssets()
{
    if (FAILED(AssetMgr::GetInst()->SaveAllAssets()))
    {
        wstring DebugLog = L"[Menu::SaveAllAssets] Failed to save some assets during save all ";
        DebugLog += L"[";
        DebugLog += TimeMgr::GetInst()->GetLocalTimeWString();
        DebugLog += L"]";
        DebugUtil::AddDebugLog(DebugLog);
    }
    else
    {
        // Save All Assets Succeeded
                
        wstring DebugLog = L"Saved All Assets! ";
        DebugLog += L"[";
        DebugLog += TimeMgr::GetInst()->GetLocalTimeWString();
        DebugLog += L"]";
        DebugUtil::AddDebugLog(DebugLog);
    }
}

void Menu::TrySaveCurrentLevel()
{
    // Level이 Stop 상태가 아니라면 전체 저장 불가능하도록 처리
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP)
        DebugUtil::AddDebugLog(L"[Menu::SaveCurrentLevel] Stop Level before saving");
        
    
    if (FAILED(LevelMgr::GetInst()->GetCurLevel()->SaveBySelfRelativePath()))
        DebugUtil::AddDebugLog(L"[Menu::SaveCurrentLevel] Failed to save Current Level");
    else
    {
        wstring DebugLog = L"Current Level saved! ";
        DebugLog += L"[";
        DebugLog += TimeMgr::GetInst()->GetLocalTimeWString();
        DebugLog += L"]";
        DebugUtil::AddDebugLog(DebugLog);
    }
}
