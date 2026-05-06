#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"

class CustomScriptUI : public ComponentUI
{
private:

    CScript*            m_TargetScript{};
    const SCRIPT_TYPE   m_CustomScriptType{};
    
public:
    
    CustomScriptUI(const string& _Namem, SCRIPT_TYPE _CustomScripType);
    virtual ~CustomScriptUI() override;

public:
    
    virtual void OnRemoveComponentConfirmed(bool _Confirmed) override;

public:
    
    virtual void SetTargetObject(const Ptr<GameObject>& _TargetObject) override;

protected:
    
    CScript* GetScript() const { return m_TargetScript; }
    
};

