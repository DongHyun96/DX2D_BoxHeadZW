#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"

class ScriptUI : public ComponentUI
{
private:
    
    Ptr<CScript>    m_TargetScript;
    UINT            m_ItemHeight;


public:
    ScriptUI();
    virtual ~ScriptUI() override;
    
public:
    void SetScript(CScript* _Script);

public:
    virtual void Tick_UI() override;
    
private:
    
    void TickScriptParams();

private:
    void AddItemHeight();
    
    virtual void OnRemoveScriptConfirmed(bool _Confirmed) override;

};
