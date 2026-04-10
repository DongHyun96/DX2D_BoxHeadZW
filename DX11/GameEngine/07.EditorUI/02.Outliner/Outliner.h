#pragma once
#include <unordered_set>
#include "GameEngine/07.EditorUI/EditorUI.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

/// 레벨안에 어떤 게임오브젝트가 들어있는지 계층구조로 표현
class Outliner : public EditorUI
{
private:

    Ptr<TreeUI> m_Tree{};

    unordered_set<DWORD_PTR> m_ActiveDelegateRegisteredObjects{}; // Active 콜백 등록된 GameObject 주소
    unordered_set<DWORD_PTR> m_CurrentObjectDataInTree{};         // 현재 Tree에 존재하는 GameObject 주소

    bool m_bShowOnlyActiveObjects{};  // Active한 오브젝트만 표시 옵션

private:
    
    Ptr<GameObject>         m_GizmoSelectedObject{};    // Gizmo 처리 SelectedObject
    
    vector<Ptr<GameObject>> m_PendingToKillObjects{};   // 삭제 요청이 들어왔고, Confirm창 Yes 누르면 곧 레벨에서 삭제될 오브젝트들
    
private: // Renaming 관련
    
    bool        m_RenameMode{};
    char        m_RenameBuf[128]{};
    DWORD_PTR   m_RenameTarget{};
    
public:
    
    Outliner();
    virtual ~Outliner() override;
    
public:
    
    virtual void Tick_UI() override;
    
private:
    
    void DeleteObjectTick();
    void DuplicateObjectTick();
    void ChangeObjectNameTick();

public:
    
    /// <summary> Tree 갱신, 현재 레벨의 최신 상태를 Tree 에 표시 </summary>
    void ReNew();
    
private:
    
    void AddGameObject(const Ptr<TreeNode>& _ParentNode, const Ptr<GameObject>& _Object);
    void RegisterActiveStateDelegate(const Ptr<GameObject>& _Object);
    void OnGameObjectActiveChanged(const Ptr<GameObject>& _Object);
    void CleanupInactiveDelegateRegistrations();

    /// <summary>
    /// <para> m_Tree에서 게임오브젝트 선택 시, callback 받는 함수 </para>
    /// <para> 선택된 게임오브젝트를 InspectorUI의 TargetObject로 세팅한다. </para>
    /// </summary>
    /// <param name="_Object"> : 선택된 게임오브젝트 </param>
    void OnSelectGameObject(DWORD_PTR _Object);

    /// <summary>
    /// <para> TreeUI의 Double click이 발생했을 때, callback 받는 함수 </para>
    /// <para> 해당 오브젝트의 Transform component가 존재한다면, 해당 위치로 EditorCam을 보낸다. </para>
    /// </summary>
    /// <param name="_Object"></param>
    void OnDoubleClickedGameObject(DWORD_PTR _Object);

    /// <summary>
    /// m_Tree에서 게임 오브젝트 drag & drop 이벤트 발생 시, callback 받는 함수
    /// </summary>
    /// <param name="_Src"> : Drag한 Src TreeNode object </param>
    /// <param name="_Dest"> : Drop 지점 Dest TreeNode object </param>
    void OnAddChild(DWORD_PTR _Src, DWORD_PTR _Dest);
    
public:
    
    const Ptr<GameObject>& GetSelectedObject() const { return m_GizmoSelectedObject; }
    vector<Ptr<GameObject>> GetSelectedObjects() const;
    
private:

    /// <summary>
    /// Input으로 들어온 GameObject들 현재 Level에서 제거 
    /// </summary>
    /// <param name="_GameObjects"></param>
    void OnDeleteKeyPressed(const vector<Ptr<GameObject>>& _GameObjects);
    
    void OnConfirmedYesOrNo(bool _bYes);
    
    
};
