#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"
#include "GameEngine/05.GameObject/GameObject.h"


class ComponentUI : public EditorUI
{
protected:
    
    string          m_ComponentTitle{};
    Ptr<GameObject> m_TargetObject{};
    
private:
    
    const COMPONENT_TYPE  m_ComType;
    
public:
    
    ComponentUI(COMPONENT_TYPE _Type, const string& _Name);
    virtual ~ComponentUI() override;

public:
    
    /// <summary>
    /// RemoveComponent 기능
    /// </summary>
    void Tick_UI() override;
    
public:
    
    Ptr<GameObject> GetTargetObject() const { return m_TargetObject; }

    /// <summary>
    /// <para> ComponentUI에 띄울 TargetObject 세팅 </para>
    /// <para> Param으로 들어온 TargetObj로 m_TargetObject는 세팅하되,
    /// <para> 만약 TargetObject가 nullptr이거나, 해당 TargetObject에 대응되는 Component가 존재하지 않다면 자신의 Active를 끈다. </para> 
    /// </summary>
    /// <param name="_TargetObject"></param>
    virtual void SetTargetObject(const Ptr<GameObject>& _TargetObject);
    
private:
    
    virtual void OnRemoveComponentConfirmed(bool _Confirmed);
    
};
