#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"
#include "GameEngine/07.EditorUI/07.TreeUI/TreeUI.h"

class ContentUI : public EditorUI
{
private:

    Ptr<TreeUI> m_Tree{};

private: // Renaming 관련
    
    bool m_RenameMode{};
    char m_RenameBuf[128]{};
    DWORD_PTR m_RenameTarget{};
    
private: // Asset 삭제 관련
    
    bool                    m_DeleteFlowActive{};
    vector<Ptr<Asset>>      m_DeleteQueue{};
    size_t                  m_DeleteCursor{};
    vector<wstring>         m_CurrentReferencers{};
    int                     m_DeleteSucceeded{};
    int                     m_DeleteSkipped{};
    
    
    
public:
    
    ContentUI();
    virtual ~ContentUI() override;
    
private:
    void Tick_UI() override;

private:
    
    void ChangeAssetNameTick();
    void DeleteAssetTick();
    
    /// <summary> RenameModeEnd 처리 반복 코드 </summary> 
    void RenameModeEndBoilerPlate();

public:
    
    void ReNew();
    
private:
    
    void OnSelectAsset(DWORD_PTR _Asset);
    

    
    
    
    
    
    
    

private:
    
    void OnDeleteKeyPressed(const vector<Ptr<Asset>>& _Assets);
    
    void OnDeleteSelectionConfirm(bool _Yes);
    void OnDeleteSingleConfirm(bool _Yes);
    void ProcessDeleteQueue();
    void ResetDeleteFlow();

    /// <summary>
    /// TargetAsset을 참조 중인 객체들 탐색
    /// </summary>
    void CollectReferencers(const Ptr<Asset>& _Target, vector<wstring>& _OutRefs) const;
    void CollectReferencersFromAsset(const Ptr<Asset>& _Owner, const Ptr<Asset>& _Target, vector<wstring>& _OutRefs) const;
    void CollectReferencersFromObject(const Ptr<GameObject>& _Obj, const Ptr<Asset>& _Target, vector<wstring>& _OutRefs) const;

    void GatherAllObjects(vector<Ptr<GameObject>>& _OutObjects) const;
    void CollectObjectRecursive(const Ptr<GameObject>& _Obj, unordered_set<GameObject*>& _Visited, vector<Ptr<GameObject>>& _Out) const;

    int ClearAllReferencesToAsset(const Ptr<Asset>& _Target);
    int ClearReferencesFromAsset(const Ptr<Asset>& _Owner, const Ptr<Asset>& _Target);
    int ClearReferencesFromObject(const Ptr<GameObject>& _Obj, const Ptr<Asset>& _Target);

    bool DeleteAssetNow(const Ptr<Asset>& _Asset);

    string BuildDeleteConfirmText() const;
    string BuildReferencerConfirmText(const Ptr<Asset>& _Target, const vector<wstring>& _Refs) const;
    
};
