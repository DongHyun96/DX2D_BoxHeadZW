#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/07.EditorUI/01.Inspector/ObjectInspectorViewer.h"
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"


class Inspector : public EditorUI
{
    
private:

    Ptr<Asset>              m_TargetAsset{};
    ObjectInspectorViewer   m_ObjectViewer{};
    Ptr<AssetUI>            m_arrAssetUI[static_cast<UINT>(ASSET_TYPE::END)]{};
    
    bool                    m_LockTarget{};

public:
    
    Inspector();
    Inspector(const string& UIName);
    virtual ~Inspector() override;

public:
    
    virtual void Tick_UI() override;
    
public:
    
    /// <summary>
    /// 자식 UI에도 TargetObject를 일괄적으로 잡아준다.
    /// </summary>
    void SetTargetObject(const Ptr<GameObject>& _Object);
    void SetTargetAsset(const Ptr<Asset>& _Asset);
    
    Ptr<Asset> GetTargetAsset() const { return m_TargetAsset; }
    Ptr<GameObject> GetTargetObject() const { return m_ObjectViewer.GetTargetObject(); }
    

private:
    
    void CreateChildUI();
    
private:
    
    void ClearTargetObject() { m_ObjectViewer.SetTargetObject(nullptr); }
    
    void SetLocked(bool _Lock) { m_LockTarget = _Lock; }
    
    void ToggleLocked() { m_LockTarget = !m_LockTarget; }
    
public:
    
    bool IsLocked() const { return m_LockTarget; }
    
private:
    
    virtual void Deactivate() override;
    
};
