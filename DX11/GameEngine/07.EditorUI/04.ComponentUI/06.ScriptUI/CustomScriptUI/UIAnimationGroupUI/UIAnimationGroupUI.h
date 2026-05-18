#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/06.ScriptUI/CustomScriptUI/CustomScriptUI.h"

class UIAnimationGroupUI : public CustomScriptUI
{
private:
    char    m_AddAnimKeyBuffer[256] = "";
    wstring m_SelectedAnimKey       = L"";

public:
    
    UIAnimationGroupUI();
    virtual ~UIAnimationGroupUI() override;
    
public:
    void Tick_UI() override;
    void SetTargetObject(const Ptr<GameObject>& _TargetObject) override;
    
private:
    
    void Tick_AddAnimationToGroup(class CUIAnimationGroup* _AnimGroup);
    void Tick_ShowAnimList(CUIAnimationGroup* _AnimGroup);
    
};
