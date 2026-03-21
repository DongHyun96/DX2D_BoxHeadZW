#include "pch.h"
#include "Inspector.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/07.TaskMgr/TaskMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"

Inspector::Inspector()
    : EditorUI("Inspector")
{
    CreateChildUI();
    SetTargetObject(nullptr);
}

Inspector::Inspector(const string& UIName)
    : EditorUI(UIName)
{
    CreateChildUI();
    SetTargetObject(nullptr);
}

Inspector::~Inspector()
{
}

void Inspector::Tick_UI()
{
    ImGui::Separator();
    
    ImGui::Checkbox("Lock", &m_LockTarget);
    ImGuiIO& io = ImGui::GetIO();
    const ImGuiInputFlags flags = ImGuiInputFlags_RouteFocused;
    
    if (!io.WantTextInput && ImGui::Shortcut(ImGuiKey_L, flags)) // Lock Toggling (L단축키)
        m_LockTarget = !m_LockTarget;
    
    
    if (!m_ObjectViewer.GetTargetObject()) return;

    m_ObjectViewer.TickHeaderUI();

    
}

void Inspector::SetTargetObject(const Ptr<GameObject>& _Object)
{
    if (m_LockTarget && (m_ObjectViewer.GetTargetObject() || m_TargetAsset)) return;
    
    m_ObjectViewer.SetTargetObject(_Object);
    
    // AssetUI 비활성화
    for (Ptr<AssetUI>& assetUI : m_arrAssetUI)
        if (assetUI) assetUI->SetActive(false);
}

void Inspector::SetTargetAsset(const Ptr<Asset>& _Asset)
{
    if (m_LockTarget && (m_ObjectViewer.GetTargetObject() || m_TargetAsset)) return;
        
    ClearTargetObject();
    SetTargetObject(nullptr);
    
    m_TargetAsset = _Asset;

    if (!_Asset)
    {
        for (Ptr<AssetUI>& assetUI : m_arrAssetUI)
            assetUI->SetActive(false);            
    }
    else
    {
        ASSET_TYPE Type = m_TargetAsset->GetType();
        m_arrAssetUI[static_cast<UINT>(Type)]->SetActive(true);
        m_arrAssetUI[static_cast<UINT>(Type)]->SetTargetAsset(m_TargetAsset);
    }
}


void Inspector::Deactivate()
{
    EditorMgr::GetInst()->RemoveInspector(this);
}
