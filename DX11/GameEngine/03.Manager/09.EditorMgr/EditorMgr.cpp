#include "pch.h"
#include "EditorMgr.h"

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/06.Component/Script/CEditorCamMoveScript.h"

#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include "GameEngine/07.EditorUI/01.Inspector/Inspector.h"
#include "GameEngine/07.EditorUI/02.Outliner/Outliner.h"
#include "GameEngine/07.EditorUI/03.Menu/Menu.h"
#include "GameEngine/07.EditorUI/05.ListUI/ListUI.h"
#include "GameEngine/07.EditorUI/06.ContentUI/ContentUI.h"
#include "GameEngine/07.EditorUI/08.AssetUI/03.TextureUI/TextureSpriteCreatorUI.h"
#include "GameEngine/07.EditorUI/09.MainWindowDropDetectorUI/MainWindowDropDetectorUI.h"
#include "GameEngine/07.EditorUI/10.ConfirmUI/ConfirmUI.h"
#include "GameEngine/07.EditorUI/11.CollisionMatrixUI/CollisionMatrixUI.h"
#include "GameEngine/07.EditorUI/12.DebugLogUI/DebugLogUI.h"


EditorMgr::EditorMgr()
{
}

EditorMgr::~EditorMgr()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void EditorMgr::AddInspector()
{
    const string InspectorUIName = "Inspector" + to_string(m_vecInspector.size());
    Ptr<Inspector> NewInspector = new Inspector(InspectorUIName);
    
    AddUI(InspectorUIName, NewInspector.Get());
    m_vecInspector.push_back(NewInspector);
}

bool EditorMgr::RemoveInspector(const Ptr<Inspector>& _Inspector)
{
    if (!_Inspector) return false;

    for (const Ptr<Inspector>& pending : m_PendingRemoveInspector)
    {
        if (pending == _Inspector) return true;
    }

    m_PendingRemoveInspector.push_back(_Inspector.Get());
    return true;
}

void EditorMgr::Init()
{
     // Make process DPI aware and obtain main monitor scale
     ImGui_ImplWin32_EnableDpiAwareness();
     float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

     // Setup Dear ImGui context
     IMGUI_CHECKVERSION();
     ImGui::CreateContext();
     ImGuiIO& io = ImGui::GetIO(); (void)io;
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
     io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
     io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
     //io.ConfigViewportsNoAutoMerge = true;
     //io.ConfigViewportsNoTaskBarIcon = true;
     //io.ConfigDockingAlwaysTabBar = true;
     //io.ConfigDockingTransparentPayload = true;

     // Setup Dear ImGui style
     ImGui::StyleColorsDark();
     // ImGui::StyleColorsLight();

     // Setup scaling
     ImGuiStyle& style = ImGui::GetStyle();
     style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
     style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
     io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
     io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

     // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
     if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
     {
         style.WindowRounding = 0.0f;
         style.Colors[ImGuiCol_WindowBg].w = 1.0f;
     }

     // Setup Platform/Renderer backends
     ImGui_ImplWin32_Init(Device::GetInst()->GetMainWndHwnd());
     ImGui_ImplDX11_Init(DEVICE, CONTEXT);

    // 게임 에디터 UI 만들기
    CreateEditorUI();
    
    // Editor용 게임 오브젝트 만들기
    CreateEditorObject();
}

void EditorMgr::Progress()
{
    Tick();
    Render();
}

void EditorMgr::Tick()
{
    FlushPendingInspectorRemove();
    
    // ==================
    // Editor Object Tick
    // ==================
    {
        for (const Ptr<GameObject>& Object : m_vecEditorObject)
            Object->Tick();
        
        for (const Ptr<GameObject>& Object : m_vecEditorObject)
            Object->FinalTick_Editor();
    }
    
    // =============
    // Editor UI Tick
    // =============
    {
        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        ImGuiIO& io = ImGui::GetIO();
        
        GizmoToggleTick(io);
        
        m_FocusedUI = nullptr;

        if (KEY_TAP(KEY::ENTER)) ImGui::SetWindowFocus(nullptr);
        
        // 단축키 처리
        if (!io.WantTextInput)
        {
            const ImGuiInputFlags flags = ImGuiInputFlags_RouteGlobal;
            Menu* menu = static_cast<Menu*>(m_mapUI["Menu"].Get());
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S, flags))
            {
                menu->TrySaveAllAssets();
                menu->TrySaveCurrentLevel();
            }
            else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, flags))
                menu->TrySaveCurrentLevel();

        }

        // DemoUI
        if (m_bShowDemo) ImGui::ShowDemoWindow(&m_bShowDemo);
        
        // EditorUI
        for (const auto& Pair : m_mapUI)
        {
            if (Pair.second->IsActive()) Pair.second->Tick();
        }
        
        UpdateGizmo();
        
        /*ImGui::Begin("Test");

        ImGui::BeginChild("Child1", Vec2(0.f, 250.f));
        ImGui::Button("Child1Button");
        ImGui::EndChild();
        
        ImGui::BeginChild("Child2");
        ImGui::Button("Child2Button");
        ImGui::EndChild();
        
        ImGui::End();*/
        
        KeyMgr::GetInst()->SetActive(m_FocusedUI == nullptr);
    }
}

void EditorMgr::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void EditorMgr::CreateEditorUI()
{
    Ptr<EditorUI> pUI{};

    pUI = new Menu;
    pUI->SetUIName("Menu");
    AddUI(pUI->GetUIName(), pUI);

    
    /*pUI = new Inspector;
    pUI->SetUIName("Inspector");
    AddUI(pUI->GetUIName(), pUI);*/
    AddInspector();
    AddInspector();
    AddInspector();

    
    pUI = new Outliner;
    pUI->SetUIName("Outliner");
    AddUI(pUI->GetUIName(), pUI);
    m_Outliner = dynamic_cast<Outliner*>(pUI.Get());

    pUI = new ListUI;
    pUI->SetModal(true);
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);
    
    pUI = new ConfirmUI;
    pUI->SetUIName("ConfirmUI");
    pUI->SetModal(true);
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);
    
    pUI = new ContentUI;
    pUI->SetUIName("Content");
    AddUI(pUI->GetUIName(), pUI);
    
    pUI = new TextureSpriteCreatorUI;
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);
    
    pUI = new MainWindowDropDetectorUI;
    pUI->SetActive(true);
    AddUI(pUI->GetUIName(), pUI);
    m_MainWindowDropDetectorUI = dynamic_cast<MainWindowDropDetectorUI*>(pUI.Get());
    
    pUI = new CollisionMatrixUI;
    pUI->SetUIName("CollisionMatrixUI");
    pUI->SetActive(false);
    AddUI(pUI->GetUIName(), pUI);
    
    pUI = new DebugLogUI;
    pUI->SetUIName("DebugLogUI");
    AddUI(pUI->GetUIName(), pUI);
}

void EditorMgr::CreateEditorObject()
{
    // Editor Camera object 생성
    // 카메라 역할 오브젝트 
    Ptr<GameObject> pObject = new GameObject;
    pObject->SetName(L"EditorCamera");

    pObject->AddComponent(new CTransform);
    pObject->AddComponent(new CCamera);
    pObject->AddComponent(new CEditorCamMoveScript);

    pObject->Camera()->LayerCheckAll();

    pObject->Camera()->SetType(PROJ_TYPE::ORTHOGRAPHIC);
    pObject->Camera()->SetFar(10000.f);
    pObject->Camera()->SetFOV(90.f);
    pObject->Camera()->SetOrthoScale(1.f);
    
    Vec2 vResolution = Device::GetInst()->GetRenderResolution();
    pObject->Camera()->SetAspectRatio(vResolution.x / vResolution.y); // 종횡비(AspectRatio)
    pObject->Camera()->SetWidth(vResolution.x);

    
    m_vecEditorObject.push_back(pObject);
    
    // Editor용 카메라로서 RenderMgr에 등록
    RenderMgr::GetInst()->RegisterEditorCamera(pObject->Camera());
}

void EditorMgr::FlushPendingInspectorRemove()
{
    if (m_PendingRemoveInspector.empty()) return;

    for (const Ptr<Inspector>& inspector : m_PendingRemoveInspector)
    {
        for (vector<Ptr<Inspector>>::iterator it = m_vecInspector.begin(); it != m_vecInspector.end(); ++it)
        {
            if ((*it) == inspector)
            {
                m_vecInspector.erase(it);
                break;
            }
        }

        for (map<string, Ptr<EditorUI>>::iterator it = m_mapUI.begin(); it != m_mapUI.end(); ++it)
        {
            if (it->second.Get() == inspector.Get())
            {
                m_mapUI.erase(it);
                break;
            }
        }
    }

    m_PendingRemoveInspector.clear();
}

void EditorMgr::AddUI(const string& _UIName, const Ptr<EditorUI>& _UI)
{
    Ptr<EditorUI> pUI = FindUI(_UIName);
    assert(!pUI);
    m_mapUI.insert(make_pair(_UIName, _UI));
}

Ptr<EditorUI> EditorMgr::FindUI(const string& _UIName)
{
    map<string, Ptr<EditorUI>>::iterator iter = m_mapUI.find(_UIName);
    if (iter == m_mapUI.end()) return nullptr;
    return iter->second;
}