#include "pch.h"
#include "TreeUI.h"

// ==========================================
// TreeNode
// ==========================================
TreeNode::TreeNode()
    : Parent(nullptr)
    , m_Owner(nullptr)
    , Framed(false)
{
    // 각 노드가 표기하려는 이름이 같을 수가 있기 때문에, 보여주려는 이름 뒤에 붙을 고유 문자열을 미리 생성해둔다.
    char szKey[50]{};
    sprintf_s(szKey, 50, "##TreeNode%d", GetEntityInstID());
    Key = szKey;
}

void TreeNode::Tick()
{
    UINT Flags =   ImGuiTreeNodeFlags_SpanFullWidth
                 | ImGuiTreeNodeFlags_OpenOnDoubleClick // 더블클릭으로만 열리게끔
                 | ImGuiTreeNodeFlags_OpenOnArrow;       // 화살표 누르면 열리기
    
    // 노드가 자식노드를 보유하고 있지 않으면 Leaf 플래그 추가
    
    if (vecChildNode.empty())           Flags |= ImGuiTreeNodeFlags_Leaf;
    if (m_Owner->IsSelected(this))      Flags |= ImGuiTreeNodeFlags_Selected;
    if (Framed)                         Flags |=  ImGuiTreeNodeFlags_Framed;

    const bool FolderVisual = IsFolderNode && !Framed;
    
    // string NodeName = (FolderVisual ? "[DIR] " : "") + Str + Key;
    string NodeName = FolderVisual ? "[" + Str + "]" + Key : Str + Key;
    
    if (Framed && vecChildNode.empty())
        NodeName = "   " + NodeName;
    
    const bool UseCustomTextColor = FolderVisual || TextDimmed;
    if (UseCustomTextColor)
    {
        ImVec4 textColor = FolderVisual
            ? ImVec4(0.95f, 0.78f, 0.22f, 1.f)
            : ImGui::GetStyleColorVec4(ImGuiCol_Text);

        if (TextDimmed)
        {
            constexpr float dimFactor = 0.35f;
            textColor.x *= dimFactor;
            textColor.y *= dimFactor;
            textColor.z *= dimFactor;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    }

    // Renew 이 후, 이전에 골랐었던 TreeNode에 대해 강제 오픈 처리를 위한 단계
    if (OpenRequested)
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        OpenRequested = false;
    }
    
    const bool Opened = ImGui::TreeNodeEx(NodeName.c_str(), Flags);
    WasOpen = Opened;

    if (UseCustomTextColor)
        ImGui::PopStyleColor();

    if (Opened)
    {
        ClickCheck();
        DragCheck();
        DropCheck();

        for (const Ptr<TreeNode>& ChildNode : vecChildNode)
            ChildNode->Tick();

        ImGui::TreePop();
    }
    else // 닫혀있을 경우 (자식이 없을 경우, 클릭판정(떼어질 때로) 처리를 여기서 해주어야 함)
    {
        ClickCheck();
        DragCheck();
        DropCheck();
    }
}

void TreeNode::ClickCheck()
{
    if (m_Owner->IsDragging()) return;
    
    if (!ImGui::IsItemHovered()) return;

    // 클릭 시작된 순간으로 처리 안한 이유는, Inspector창의 이전 정보를 유지하기 위함
    // 클릭 한번 처리 되었을 때의 처리
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        ImGuiIO& io = ImGui::GetIO();
        m_Owner->RegisterSelectedEx(this, io.KeyCtrl, io.KeyShift);
    }
    
    // 더블 클릭 되었을 때, m_Owner TreeUI에 등록된 더블클릭 함수 포인터 콜백 객체가 있다면 콜백 처리
    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        m_Owner->NotifyDoubleClicked();
}

void TreeNode::DragCheck()
{
    /*// 드래그
    if (ImGui::BeginDragDropSource())
    {
        // Drag 사이에 Text를 넣어주면, 드래그중인 마우스 위치에 Text가 따라다니면서 렌더링된다.
        ImGui::Text(Str.c_str());
        
        if (Data != 0) // Data가 Valid할 경우에만 Payload 등록 (머리말 같은 UI는 처리를 안하기 위함)
        {
            // 맨 앞 Parameter는 PayLoad의 키값, Drop받는 쪽에서도 키값을 맞추어야 함
            /*ImGui::SetDragDropPayload(m_Owner->GetParentUI()->GetUIName().c_str(),
                &Data, sizeof(DWORD_PTR));#1#
            
            if (!m_Owner->IsSelected(this))
                m_Owner->RegisterSelectedEx(this, false, false);

            m_Owner->BuildDragPayload();
            const auto& payload = m_Owner->GetDragPayload();

            if (!payload.empty())
            {
                ImGui::SetDragDropPayload(m_Owner->GetParentUI()->GetUIName().c_str(),
                    payload.data(), sizeof(DWORD_PTR) * payload.size());
            }
        }
        
        m_Owner->RegisterDragged(this);
        
        ImGui::EndDragDropSource();
    }*/
    
    if (ImGui::BeginDragDropSource())
    {
        m_Owner->BeginDrag(this);

        ImGui::Text(Str.c_str());

        const auto& payload = m_Owner->GetDragPayloads();
        if (!payload.empty())
        {
            ImGui::SetDragDropPayload(
                m_Owner->GetParentUI()->GetUIName().c_str(),
                payload.data(),
                sizeof(DWORD_PTR) * payload.size()
            );
        }

        ImGui::EndDragDropSource();
    }
}

void TreeNode::DropCheck()
{
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(m_Owner->GetDropKey().c_str()))
        {
            m_Owner->SetDropPayload(Payload);
            m_Owner->RegisterDropped(this);
        }

        ImGui::EndDragDropTarget();
    }
}


namespace
{
    void CollectAssetDataRecursive(const Ptr<TreeNode>& node,
                                   vector<DWORD_PTR>& out,
                                   unordered_set<DWORD_PTR>& dedup)
    {
        if (!node) return;

        // Asset node
        if (node->Data != 0)
        {
            if (dedup.insert(node->Data).second)
                out.push_back(node->Data);
            return;
        }

        // Folder/Group node
        for (const Ptr<TreeNode>& child : node->vecChildNode)
            CollectAssetDataRecursive(child, out, dedup);
    }

    void AppendPayloadFromNode(const Ptr<TreeNode>& node,
                               vector<DWORD_PTR>& out,
                               unordered_set<DWORD_PTR>& dedup)
    {
        if (!node) return;

        // 일반 Asset 노드
        if (node->Data != 0)
        {
            if (dedup.insert(node->Data).second)
                out.push_back(node->Data);
            return;
        }

        // 폴더 노드면 하위 Asset 전체 수집
        if (node->IsFolderNode)
            CollectAssetDataRecursive(node, out, dedup);

        // Data==0 이지만 폴더가 아닌 타입 헤더 노드 등은 무시
    }
}



// ==========================================
// TreeUI
// ==========================================
TreeUI::TreeUI()
    : EditorUI("TreeUI")
    , m_SelectedInst(nullptr)
    , m_SelectedInstMemFunc(nullptr)
{
}

TreeUI::~TreeUI()
{
}

void TreeUI::Tick_UI()
{
    /*BuildOrderedNodes();

    for (const Ptr<TreeNode>& Node : m_vecNode)
        Node->Tick();

    if ((m_DragNode && m_DropNode) || (m_DragNode && ImGui::IsMouseReleased(ImGuiMouseButton_Left))) 
    {
        if (m_DragDropInst && m_DragDropInstMemFunc)
            (m_DragDropInst->*m_DragDropInstMemFunc)(reinterpret_cast<DWORD_PTR>(m_DragNode.Get()), reinterpret_cast<DWORD_PTR>(m_DropNode.Get()));

        m_DragNode = m_DropNode = nullptr;
    }*/
    
    BuildOrderedNodes(); // Shift 선택 쓰면 유지

    for (const Ptr<TreeNode>& Node : m_vecNode)
        Node->Tick();

    if (m_DragNode && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        m_IsDragging = false;

    if ((m_DragNode && m_DropNode) || (m_DragNode && ImGui::IsMouseReleased(ImGuiMouseButton_Left))) 
    {
        if (m_DragDropInst && m_DragDropInstMemFunc)
            (m_DragDropInst->*m_DragDropInstMemFunc)(
                reinterpret_cast<DWORD_PTR>(m_DragNode.Get()),
                reinterpret_cast<DWORD_PTR>(m_DropNode.Get())
            );

        m_DragNode = m_DropNode = nullptr;
    }
}

Ptr<TreeNode> TreeUI::FindNodeByData(DWORD_PTR _Data) const
{
    if (_Data == 0) return nullptr;

    for (const Ptr<TreeNode>& node : m_vecNode)
    {
        Ptr<TreeNode> found = FindNodeByDataRecursive(node, _Data);
        if (found) return found;
    }

    return nullptr;
}

Ptr<TreeNode> TreeUI::AddItem(const Ptr<TreeNode>& _ParentNode, const string& _String, DWORD_PTR _Data)
{
    Ptr<TreeNode> pNewNode  = new TreeNode;
    pNewNode->Str           = _String;
    pNewNode->m_Owner       = this;
    pNewNode->Data          = _Data;
    
    // 최상위 부모노드로 추가
    if (!_ParentNode) m_vecNode.push_back(pNewNode);
    else              _ParentNode->AddChildNode(pNewNode); // 특정 노드 및에 자식으로 추가
    
    return pNewNode;
}

void TreeUI::RegisterSelected(const Ptr<TreeNode>& _Node)
{
    RegisterSelectedEx(_Node, false, false);
    /*m_Selected = _Node;
    if (m_SelectedInst && _Node && _Node->Data) (m_SelectedInst->*m_SelectedInstMemFunc)(_Node->Data);*/
}

void TreeUI::RegisterSelectedEx(const Ptr<TreeNode>& _Node, bool _Ctrl, bool _Shift)
{
    if (!_Node) return;

    m_Selected = _Node;

    if (!_Ctrl && !_Shift)
    {
        m_SelectedNodes.clear();
        m_SelectedNodes.push_back(_Node);
        m_SelectAnchor = _Node;
    }
    else if (_Shift)
    {
        if (!m_SelectAnchor) m_SelectAnchor = _Node;

        int a = GetOrderedIndex(m_SelectAnchor.Get());
        int b = GetOrderedIndex(_Node.Get());
        if (a < 0 || b < 0)
        {
            m_SelectedNodes.clear();
            m_SelectedNodes.push_back(_Node);
        }
        else
        {
            if (!_Ctrl) m_SelectedNodes.clear();

            int lo = (a < b) ? a : b;
            int hi = (a < b) ? b : a;

            for (int i = lo; i <= hi; ++i)
            {
                const Ptr<TreeNode>& n = m_OrderedNodes[i];
                if (find(m_SelectedNodes.begin(), m_SelectedNodes.end(), n) == m_SelectedNodes.end())
                    m_SelectedNodes.push_back(n);
            }
        }
    }
    else if (_Ctrl)
    {
        vector<Ptr<TreeNode>>::iterator it = find(m_SelectedNodes.begin(), m_SelectedNodes.end(), _Node);
        if (it != m_SelectedNodes.end()) m_SelectedNodes.erase(it);
        else m_SelectedNodes.push_back(_Node);

        m_SelectAnchor = _Node;
    }

    if (m_SelectedInst && _Node && _Node->Data) 
        (m_SelectedInst->*m_SelectedInstMemFunc)(_Node->Data);
}

bool TreeUI::IsSelected(const TreeNode* _Node) const
{
    if (!_Node) return false;
    for (const Ptr<TreeNode>& n : m_SelectedNodes)
        if (n.Get() == _Node) return true;
    return false;
}

void TreeUI::BeginDrag(const Ptr<TreeNode>& _Node)
{
    m_IsDragging = true;
    m_DragNode = _Node;
    BuildDragPayloadForNode(_Node);
}

void TreeUI::BuildDragPayloadForNode(const Ptr<TreeNode>& _Node)
{
    m_DragPayload.clear();
    if (!_Node) return;

    unordered_set<DWORD_PTR> dedup{};
    dedup.reserve(64);

    if (IsSelected(_Node.Get()))
    {
        for (const Ptr<TreeNode>& n : m_SelectedNodes)
            AppendPayloadFromNode(n, m_DragPayload, dedup);
    }
    else AppendPayloadFromNode(_Node, m_DragPayload, dedup);
}

void TreeUI::BuildDragPayload()
{
    m_DragPayload.clear();

    unordered_set<DWORD_PTR> dedup{};
    dedup.reserve(64);

    for (const Ptr<TreeNode>& n : m_SelectedNodes)
        AppendPayloadFromNode(n, m_DragPayload, dedup);
}

void TreeUI::SetDropPayload(const ImGuiPayload* _Payload)
{
    m_DropPayload.clear();
    if (!_Payload || _Payload->DataSize <= 0) return;

    const size_t count = _Payload->DataSize / sizeof(DWORD_PTR);
    m_DropPayload.resize(count);
    memcpy(m_DropPayload.data(), _Payload->Data, count * sizeof(DWORD_PTR));
}

void TreeUI::Clear()
{
    m_vecNode.clear();
    m_Selected = nullptr;
    m_DragNode = nullptr;
    m_DropNode = nullptr;
    m_SelectedNodes.clear();
    m_SelectAnchor = nullptr;
    m_OrderedNodes.clear();
    m_DragPayload.clear();
    m_DropPayload.clear();
}

bool TreeUI::IsPayloadMultiData(const ImGuiPayload* _Payload)
{
    const int count = _Payload->DataSize / sizeof(DWORD_PTR);
    return count > 1;
}

void TreeUI::ExpandToNode(const Ptr<TreeNode>& _Node)
{
    if (!_Node) return;

    TreeNode* cur = _Node.Get();
    while (cur)
    {
        cur->RequestOpen();
        cur = cur->Parent;
    }
}

void TreeUI::BuildOrderedNodes()
{
    m_OrderedNodes.clear();
    for (const Ptr<TreeNode>& n : m_vecNode)
        CollectOrdered(n);
}

void TreeUI::CollectOrdered(const Ptr<TreeNode>& _Node)
{
    if (!_Node) return;
    m_OrderedNodes.push_back(_Node);
    for (const Ptr<TreeNode>& c : _Node->vecChildNode)
        CollectOrdered(c);
}

Ptr<TreeNode> TreeUI::FindNodeByDataRecursive(const Ptr<TreeNode>& _Node, DWORD_PTR _Data) const
{
    if (!_Node) return nullptr;

    if (_Node->Data == _Data)
        return _Node;

    for (const Ptr<TreeNode>& child : _Node->vecChildNode)
    {
        Ptr<TreeNode> found = FindNodeByDataRecursive(child, _Data);
        if (found) return found;
    }

    return nullptr;
}

int TreeUI::GetOrderedIndex(const TreeNode* _Node) const
{
    for (int i = 0; i < static_cast<int>(m_OrderedNodes.size()); ++i)
        if (m_OrderedNodes[i].Get() == _Node) return i;
    return -1;
}

void TreeUI::NotifyDoubleClicked()
{
    if (m_DoubleClickedInst)
        (m_DoubleClickedInst->*m_DoubleClickedInstMemFunc)(m_Selected->Data);
}

void TreeUI::CollectOpenedNodeDataRecursive(const Ptr<TreeNode>& _Node, vector<DWORD_PTR>& _OutData) const
{
    if (!_Node) return;

    if (_Node->IsOpen() && _Node->Data != 0)
        _OutData.push_back(_Node->Data);

    for (const Ptr<TreeNode>& child : _Node->vecChildNode)
        CollectOpenedNodeDataRecursive(child, _OutData);
}

void TreeUI::GetOpenedNodeData(vector<DWORD_PTR>& _OutData) const
{
    _OutData.clear();
    for (const Ptr<TreeNode>& node : m_vecNode)
        CollectOpenedNodeDataRecursive(node, _OutData);
}
