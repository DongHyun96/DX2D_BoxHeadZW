#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/06.ScriptUI/ScriptUI.h"

class ObjectInspectorViewer : public Entity
{
private:
    EditorUI*               m_Owner{};
    Ptr<GameObject>         m_TargetObject{};
    Ptr<ComponentUI>        m_arrComUI[static_cast<UINT>(COMPONENT_TYPE::END)]{};
    vector<Ptr<ScriptUI>>   m_vecScriptUI{};
public:
    
    ObjectInspectorViewer();
    virtual ~ObjectInspectorViewer() override;

public:
    
    void Init(EditorUI* _Owner);
    void TickHeaderUI();
    
public:
    
    void SetTargetObject(const Ptr<GameObject>& _Object);
    const Ptr<GameObject>& GetTargetObject() const { return m_TargetObject; }

private:
    
    void TickLayerUI();
    void TickAddScriptUI();
    void TickAddComponentUI();
    void TickCreateThisObjectAsPrefabUI();
    
private:
    
    void CreateChildUI();
    void RefreshScripts();

};
