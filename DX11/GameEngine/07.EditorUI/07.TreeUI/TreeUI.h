#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

/// <summary>
/// TreeNode
/// </summary>
struct TreeNode : public Entity
{
public:
    
    string                  Str{};                  // 띄울 문자열
    DWORD_PTR               Data{};                 // TreeNode에 저장할 Data (제목과 같은 항목에는 Data가 들어가지 않음)
    TreeNode*               Parent{};
    vector<Ptr<TreeNode>>   vecChildNode{};
    
    bool                    Framed{};               // Frame 처리로 보여줄지
    bool                    IsFolderNode{};         // 폴더 노드인지
    bool                    TextDimmed{};           // 텍스트를 살짝 어둡게 표시할지
    
    class TreeUI*           m_Owner{};              // 노드를 소유하고 있는 TreeUI
    
private:

    bool OpenRequested{}; // 다음 프레임에 한 번 강제 오픈 (ReNew 이 후, 이전에 골랐던 TreeNode를 다시 열기 위함)
    bool WasOpen{}; // 직전 프레임 Open 상태 캐시
    
private:
    
    string Key{}; // 보여주려는 이름 뒤에 붙을 고유 문자열 키값

public:
    
    TreeNode();
    
public:
    void Tick();
    
public:

    void RequestOpen() { OpenRequested = true; }
    bool IsOpen() const { return WasOpen; }
    
    void AddChildNode(const Ptr<TreeNode>& _Node)
    {
        vecChildNode.push_back(_Node);
        _Node->Parent = this;
    }
    
    void SetFramed(bool _Frame) { Framed = _Frame; }
    void SetIsFolderStyleNode(bool _IsFolderStyle) { IsFolderNode = _IsFolderStyle; }
    void SetTextDimmed(bool _Dimmed) { TextDimmed = _Dimmed; }
    
    
private:
    
    void ClickCheck();
    void DragCheck();
    void DropCheck();
    
};







/// <summary>
/// TreeUI
/// </summary>
class TreeUI : public EditorUI
{
private:
    
    vector<Ptr<TreeNode>>   m_vecNode{};
    
    Ptr<TreeNode>           m_Selected{}; // 트리 소속 노드들 중에서 선택된 노드
    Ptr<TreeNode>           m_DragNode{}; // 트리 소속 노드들 중에서 드래그 중인 노드
    Ptr<TreeNode>           m_DropNode{}; // 트리 소속 노드들 중에서 드랍당한 노드
    
    // 선택되었을 때, Delegate 호출할 객체와 멤버변수
    EditorUI*               m_SelectedInst{};
    DELEGATE_1              m_SelectedInstMemFunc{};
    
    string                  m_DropKey{};
    
    // Drag&Drop 발생 시 Delegate 호출할 객체와 멤버변수
    EditorUI*               m_DragDropInst{};
    DELEGATE_2              m_DragDropInstMemFunc{};
    
private: // Multi-select

    bool m_IsDragging{};
    
    vector<Ptr<TreeNode>>   m_SelectedNodes{};
    Ptr<TreeNode>           m_SelectAnchor{};
    vector<Ptr<TreeNode>>   m_OrderedNodes{};

    vector<DWORD_PTR>       m_DragPayload{};
    vector<DWORD_PTR>       m_DropPayload{};
    
private: // DoubleClick 콜백 관련
    
    EditorUI*               m_DoubleClickedInst{};
    DELEGATE_1              m_DoubleClickedInstMemFunc{};
    
public:

    TreeUI();
    virtual ~TreeUI() override;
    

private:
    void Tick_UI() override;

public:

    GET_SET(string, DropKey)
    
    Ptr<TreeNode> GetSelected() const { return m_Selected; }
    const vector<Ptr<TreeNode>>& GetSelectedNodes() const { return m_SelectedNodes; }
    Ptr<TreeNode> FindNodeByData(DWORD_PTR _Data) const;
    
public:
    /// <summary>
    /// Select 이벤트 발생 시, Callback받을 Delegate 함수 등록
    /// </summary>
    /// <param name="_Inst"> : Callback 받을 EditorUI 객체 </param>
    /// <param name="_MemFunc"> : Callback 받을 멤버함수 </param>
    void AddDynamicSelect(EditorUI* _Inst, DELEGATE_1 _MemFunc) { m_SelectedInst = _Inst; m_SelectedInstMemFunc = _MemFunc; }
    
    void AddDynamicDoubleClicked(EditorUI* _Inst, DELEGATE_1 _MemFunc) { m_DoubleClickedInst = _Inst; m_DoubleClickedInstMemFunc = _MemFunc; }

    /// <summary>
    /// Drag & Drop 이벤트 발생 시, Callback 받을 Delegate 함수 등록
    /// </summary>
    /// <param name="_Inst"> : Callback 받을 EditorUI 객체 </param>
    /// <param name="_MemFunc"> : Callback 받을 멤버함수 </param>
    void AddDynamicDragDrop(EditorUI* _Inst, DELEGATE_2 _MemFunc) { m_DragDropInst = _Inst; m_DragDropInstMemFunc = _MemFunc; }
    
    
    /// <summary>
    /// TreeUI에 아이템 추가
    /// </summary>
    /// <param name="_ParentNode"> : nullptr일 경우, 최상위 부모노드로 추가 </param>
    /// <param name="_String"> : 추가할 String </param>
    /// <param name="_Data"></param>
    /// <returns> : 새롭게 생성된 TreeNode 반환 </returns>
    Ptr<TreeNode> AddItem(const Ptr<TreeNode>& _ParentNode, const string& _String, DWORD_PTR _Data = 0);

    /// <summary> Select된 TreeNode 등록 </summary>
    void RegisterSelected(const Ptr<TreeNode>& _Node);
    void RegisterSelectedEx(const Ptr<TreeNode>& _Node, bool _Ctrl, bool _Shift);
    

    bool IsSelected(const TreeNode* _Node) const;
    
    bool IsDragging() const { return m_IsDragging; }
    void SetIsDragging(bool _Dragging) { m_IsDragging = _Dragging; }
    
    void BeginDrag(const Ptr<TreeNode>& _Node);
    void BuildDragPayloadForNode(const Ptr<TreeNode>& _Node);
    
    
    /// <summary> Drag를 시작한 TreeNode 등록 </summary>
    void RegisterDragged(const Ptr<TreeNode>& _Node) { m_DragNode = _Node; }
    
    /// <summary> Drop당한 TreeNode 등록 </summary>
    void RegisterDropped(const Ptr<TreeNode>& _Node) { m_DropNode = _Node; }

    void BuildDragPayload();
    void SetDropPayload(const ImGuiPayload* _Payload);
    
    void Clear();
    
    const vector<DWORD_PTR>& GetDragPayloads() const { return m_DragPayload; }
    const vector<DWORD_PTR>& GetDropPayloads() const { return m_DropPayload; }

    /// <summary>
    /// 해당 Payload가 MultiDrag drop에 의한 Multi data인지 조사
    /// </summary>
    /// <param name="_Payload"></param>
    /// <returns> : Multi data이면 return true </returns>
    static bool IsPayloadMultiData(const ImGuiPayload* _Payload);

    void ExpandToNode(const Ptr<TreeNode>& _Node);
    
private:
    
    void BuildOrderedNodes();
    void CollectOrdered(const Ptr<TreeNode>& _Node);
    Ptr<TreeNode> FindNodeByDataRecursive(const Ptr<TreeNode>& _Node, DWORD_PTR _Data) const;
    int GetOrderedIndex(const TreeNode* _Node) const;
    
public:
    
    void NotifyDoubleClicked();
    
private:
    
    void CollectOpenedNodeDataRecursive(const Ptr<TreeNode>& _Node, vector<DWORD_PTR>& _OutData) const;

public:
    
    void GetOpenedNodeData(vector<DWORD_PTR>& _OutData) const;
    
    UINT GetNodeCount() const { return m_vecNode.size(); }
    
    
};
