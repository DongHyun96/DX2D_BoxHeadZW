#include "pch.h"
#include "Outliner.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/06.Component/Script/CEditorCamMoveScript.h"
#include "GameEngine/07.EditorUI/01.Inspector/Inspector.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"
#include "GameEngine/07.EditorUI/10.ConfirmUI/ConfirmUI.h"
#include "ImGui/imgui_internal.h"

Outliner::Outliner()
    : EditorUI("Outliner")
{
    m_Tree = new TreeUI;
    m_Tree->SetSeparator(false);
    m_Tree->AddDynamicSelect(this, static_cast<DELEGATE_1>(&Outliner::OnSelectGameObject));
    m_Tree->AddDynamicDoubleClicked(this, static_cast<DELEGATE_1>(&Outliner::OnDoubleClickedGameObject));
    m_Tree->SetDropKey("Outliner"); // Self DragDrop 사용
    m_Tree->AddDynamicDragDrop(this, static_cast<DELEGATE_2>(&Outliner::OnAddChild));
    
    AddChildUI(m_Tree.Get());

    /*Ptr<TreeNode> Test1Node = m_Tree->AddItem(nullptr, "Test_1");
    m_Tree->AddItem(Test1Node, "Test_1_Child_1");
    m_Tree->AddItem(Test1Node, "Test_1_Child_2");
    m_Tree->AddItem(Test1Node, "Test_1_Child_3");
    
    m_Tree->AddItem(nullptr, "Test_2");
    m_Tree->AddItem(nullptr, "Test_3");*/
    // ReNew();
}

Outliner::~Outliner()
{
}

void Outliner::Tick_UI()
{
    DeleteObjectTick();
    DuplicateObjectTick();
    ChangeObjectNameTick();
    
    Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
    
    if (pCurLevel && pCurLevel->HasChanged())
        ReNew(); // 복원 실패 시, inspector clear 까지 처리
}

void Outliner::DeleteObjectTick()
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantTextInput) return;
    
    const ImGuiInputFlags flags = ImGuiInputFlags_RouteFocused;

    if (!ImGui::Shortcut(ImGuiKey_Delete, flags)) return;
    
    vector<Ptr<GameObject>> ToDelete{};

    const auto& selectedNodes = m_Tree->GetSelectedNodes();
    if (!selectedNodes.empty())
    {
        for (const Ptr<TreeNode>& node : selectedNodes)
        {
            if (node && node->Data != 0)
                ToDelete.push_back(reinterpret_cast<GameObject*>(node->Data));
        }
    }
    else
    {
        Ptr<TreeNode> single = m_Tree->GetSelected();
        if (single && single->Data != 0)
            ToDelete.push_back(reinterpret_cast<GameObject*>(single->Data));
    }

    if (!ToDelete.empty()) OnDeleteKeyPressed(ToDelete);
}

void Outliner::DuplicateObjectTick()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantTextInput) return;
    
    const ImGuiInputFlags flags = ImGuiInputFlags_RouteFocused;

    // Ctrl + D키 조합
    if (!ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_D, flags)) return;
    
    vector<Ptr<GameObject>> vecToDuplicate{};

    const auto& selectedNodes = m_Tree->GetSelectedNodes();
    if (!selectedNodes.empty())
    {
        for (const Ptr<TreeNode>& node : selectedNodes)
        {
            if (node && node->Data != 0)
                vecToDuplicate.push_back(reinterpret_cast<GameObject*>(node->Data));
        }
    }
    else
    {
        Ptr<TreeNode> single = m_Tree->GetSelected();
        if (single && single->Data != 0)
            vecToDuplicate.push_back(reinterpret_cast<GameObject*>(single->Data));
    }

    for (const Ptr<GameObject>& gameObject : vecToDuplicate)
    {
        Ptr<GameObject> ClonedObject = gameObject->Clone();
        if (gameObject->GetParent()) gameObject->GetParent()->AddChild(ClonedObject); // 원본의 부모가 존재한다면, 원본의 부모 밑으로 들어가도록 처리
        else LevelMgr::GetInst()->GetCurLevel()->AddObject(ClonedObject->GetLayerIdx(), ClonedObject); // 최상위 부모 처리
    }
    ReNew();
}

void Outliner::ChangeObjectNameTick()
{
    ImGuiIO& io = ImGui::GetIO();
    const ImGuiInputFlags flags = ImGuiInputFlags_RouteFocused;

    // F2 눌렀을 때 Rename 모드 진입
    if (!io.WantTextInput && ImGui::Shortcut(ImGuiKey_F2, flags))
    {
        Ptr<TreeNode> node = m_Tree->GetSelected();
        if (node && node->Data != 0)
        {
            m_RenameMode = true;
            m_RenameTarget = node->Data;

            Ptr<GameObject> obj = reinterpret_cast<GameObject*>(node->Data);
            string name = obj->GetName().empty() ? "UnNamed" : string(obj->GetName().begin(), obj->GetName().end());

            memset(m_RenameBuf, 0, sizeof(m_RenameBuf));
            strncpy_s(m_RenameBuf, name.c_str(), sizeof(m_RenameBuf) - 1);

            ImGui::SetKeyboardFocusHere();
        }
    }

    // Rename 모드 처리
    if (m_RenameMode)
    {
        ImGui::SetNextItemWidth(200.f);

        if (ImGui::InputText("##RenameObject", m_RenameBuf, sizeof(m_RenameBuf),
            ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Ptr<GameObject> obj = reinterpret_cast<GameObject*>(m_RenameTarget);
            if (obj)
            {
                string newName = m_RenameBuf;
                wstring wname(newName.begin(), newName.end());
                obj->SetName(wname);
            }

            m_RenameMode = false;
            m_RenameTarget = 0;
            ReNew();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            m_RenameMode = false;
            m_RenameTarget = 0;
        }
    }
}

bool Outliner::ReNew()
{
    /*// 매 프레임 현재 Level에 존재하는 GameObject 정보 업데이트
    m_Tree->Clear();

    // Stop 중인 경우, Editor 전용 오브젝트들 띄우기
    if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP)
    {
        for (const Ptr<GameObject>& EditorObject : EditorMgr::GetInst()->GetGameObjects())
            AddGameObject(nullptr, EditorObject);
    }
    
    Ptr<ALevel> CurLevel = LevelMgr::GetInst()->GetCurLevel();

    for (int i = 0; i < MAX_LAYER; ++i)
    {
        for (const Ptr<GameObject>& ParentObject : CurLevel->GetLayer(i)->GetParentObjects())
            AddGameObject(nullptr, ParentObject);
    }*/
    
    
    
    vector<DWORD_PTR> prevSelectedData{};
    for (const Ptr<TreeNode>& node : m_Tree->GetSelectedNodes())
    {
        if (node && node->Data != 0)
            prevSelectedData.push_back(node->Data);
    }
    if (prevSelectedData.empty())
    {
        Ptr<TreeNode> single = m_Tree->GetSelected();
        if (single && single->Data != 0)
            prevSelectedData.push_back(single->Data);
    }

    vector<DWORD_PTR> prevOpenedData{};
    m_Tree->GetOpenedNodeData(prevOpenedData);

    m_Tree->Clear();

    if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP)
    {
        for (const Ptr<GameObject>& editorObject : EditorMgr::GetInst()->GetGameObjects())
            AddGameObject(nullptr, editorObject);
    }

    Ptr<ALevel> curLevel = LevelMgr::GetInst()->GetCurLevel();
    for (int i = 0; i < MAX_LAYER; ++i)
    {
        for (const Ptr<GameObject>& parentObject : curLevel->GetLayer(i)->GetParentObjects())
            AddGameObject(nullptr, parentObject);
    }

    for (DWORD_PTR data : prevOpenedData)
    {
        Ptr<TreeNode> node = m_Tree->FindNodeByData(data);
        if (node) m_Tree->ExpandToNode(node);
    }

    bool hasRestoredSelection = false;
    for (DWORD_PTR data : prevSelectedData)
    {
        Ptr<TreeNode> node = m_Tree->FindNodeByData(data);
        if (!node) continue;

        m_Tree->ExpandToNode(node);
        m_Tree->RegisterSelectedEx(node, hasRestoredSelection, false);
        hasRestoredSelection = true;
    }

    if (!hasRestoredSelection)
    {
        m_GizmoSelectedObject = nullptr;
        EditorMgr::GetInst()->SetTargetObjectToInspectors(nullptr);
    }

    return hasRestoredSelection;
    
    
}

void Outliner::AddGameObject(const Ptr<TreeNode>& _ParentNode, const Ptr<GameObject>& _Object)
{
    const string ObjectNameStr = _Object->GetName().empty() ? "UnNamed" : string(_Object->GetName().begin(), _Object->GetName().end()); 
    
    Ptr<TreeNode> ParentNode = m_Tree->AddItem(_ParentNode, ObjectNameStr, reinterpret_cast<DWORD_PTR>(_Object.Get()));
    
    for (const Ptr<GameObject>& ChildObject : _Object->GetChildren())
        AddGameObject(ParentNode, ChildObject);
}

vector<Ptr<GameObject>> Outliner::GetSelectedObjects() const
{
    vector<Ptr<GameObject>> selectedObjects{};

    const auto& selectedNodes = m_Tree->GetSelectedNodes();
    if (!selectedNodes.empty())
    {
        selectedObjects.reserve(selectedNodes.size());
        for (const Ptr<TreeNode>& node : selectedNodes)
        {
            if (!node || node->Data == 0) continue;
            selectedObjects.push_back(reinterpret_cast<GameObject*>(node->Data));
        }
    }
    else
    {
        Ptr<TreeNode> single = m_Tree->GetSelected();
        if (single && single->Data != 0)
            selectedObjects.push_back(reinterpret_cast<GameObject*>(single->Data));
    }

    return selectedObjects;
}

void Outliner::OnSelectGameObject(DWORD_PTR _Object)
{
    Ptr<GameObject> pSelectedObject = reinterpret_cast<GameObject*>(_Object);
    DebugUtil::AddDebugLog(L"On Select : " + pSelectedObject->GetName());
    EditorMgr::GetInst()->SetTargetObjectToInspectors(pSelectedObject);
    
    m_GizmoSelectedObject = pSelectedObject;
}

void Outliner::OnDoubleClickedGameObject(DWORD_PTR _Object)
{
    Ptr<GameObject> pSelectedObject = reinterpret_cast<GameObject*>(_Object);
    
    if (!pSelectedObject->Transform()) return;

    Vec2 ObjectXYPos = Vec2(pSelectedObject->Transform()->GetWorldPos().x, pSelectedObject->Transform()->GetWorldPos().y);
    RenderMgr::GetInst()->GetEditorCam()->GetOwner()->GetScriptComponent<CEditorCamMoveScript>()->SetTransformPosDest(ObjectXYPos);
}

void Outliner::OnAddChild(DWORD_PTR _Src, DWORD_PTR _Dest)
{
    Ptr<TreeNode> pDragNode = reinterpret_cast<TreeNode*>(_Src);
    Ptr<TreeNode> pDropNode = reinterpret_cast<TreeNode*>(_Dest);
    
    Ptr<GameObject> SrcObj  = reinterpret_cast<GameObject*>(pDragNode->Data);
    Ptr<GameObject> DestObj = (pDropNode) ? reinterpret_cast<GameObject*>(pDropNode->Data) : nullptr;

    // DestObj가 없을 때(공백에 드랍) -> 최상위 오브젝트로 해방 처리
    if (!DestObj)
    {
        if (SrcObj->GetParent()) // 자식타입 오브젝트인 경우
        {
            SrcObj->DisconnectWithParent(); // 최상위 오브젝트로 해방
            SrcObj->RegisterAsParent();
        }
    }
    else
    {
        // SrcObj가 DestObj의 Ancestor이면 안된다.
        Ptr<GameObject> Parent = DestObj->GetParent();
        while (Parent)
        {
            if (Parent == SrcObj) return;
            Parent = Parent->GetParent();
        }
        
        DestObj->AddChild(SrcObj);
    }
}

void Outliner::OnDeleteKeyPressed(const vector<Ptr<GameObject>>& _GameObjects)
{
    Ptr<ConfirmUI> pUI = dynamic_cast<ConfirmUI*>(EditorMgr::GetInst()->FindUI("ConfirmUI").Get());
    assert(pUI.Get());

    pUI->SetWarningText("Are you sure you want to delete GameObject from Level?");
    pUI->AddDelegate(this, static_cast<DELEGATE_BOOL>(&Outliner::OnConfirmedYesOrNo));
    pUI->SetActive(true);
    
    m_PendingToKillObjects.clear();
    m_PendingToKillObjects = _GameObjects;
}

void Outliner::OnConfirmedYesOrNo(bool _bYes)
{
    if (_bYes)
    {
        for (const Ptr<GameObject>& gameObject : m_PendingToKillObjects)
            gameObject->Destroy();
        
        ReNew();
    }
    
    m_PendingToKillObjects.clear();
}
