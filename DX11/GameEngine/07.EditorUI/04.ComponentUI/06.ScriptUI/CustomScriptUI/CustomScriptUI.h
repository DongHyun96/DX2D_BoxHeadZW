#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"

class CustomScriptUI : public ComponentUI
{
private:

    CScript* m_TargetScript{};
    
public:
    CustomScriptUI(const string& _Name);
    virtual ~CustomScriptUI() override;

public:
    
    virtual void OnRemoveComponentConfirmed(bool _Confirmed) override;
    
public:
    
    void SetScript(CScript* _Script) { m_TargetScript = _Script; }
    CScript* GetScript() const { return m_TargetScript; }
    
};

