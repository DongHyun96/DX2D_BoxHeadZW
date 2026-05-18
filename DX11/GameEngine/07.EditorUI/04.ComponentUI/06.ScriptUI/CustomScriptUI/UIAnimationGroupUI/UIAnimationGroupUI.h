#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/06.ScriptUI/CustomScriptUI/CustomScriptUI.h"

class UIAnimationGroupUI : public CustomScriptUI
{
private:

public:
    
    UIAnimationGroupUI();
    virtual ~UIAnimationGroupUI() override;
    
public:
    void Tick_UI() override;
    void OnRemoveComponentConfirmed(bool _Confirmed) override;
    void SetTargetObject(const Ptr<GameObject>& _TargetObject) override;
    
};
