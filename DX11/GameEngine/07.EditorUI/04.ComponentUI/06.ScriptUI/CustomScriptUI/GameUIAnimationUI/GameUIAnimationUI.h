#pragma once

#include "GameEngine/07.EditorUI/04.ComponentUI/06.ScriptUI/CustomScriptUI/CustomScriptUI.h"

class GameUIAnimationUI : public CustomScriptUI
{
private:

public:
    
    GameUIAnimationUI();
    virtual ~GameUIAnimationUI() override;
    
public:
    void Tick_UI() override;
};
