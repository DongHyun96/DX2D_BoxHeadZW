#pragma once

#include "GameEngine/07.EditorUI/04.ComponentUI/06.ScriptUI/CustomScriptUI/CustomScriptUI.h"

class GameUIAnimationUI : public CustomScriptUI
{
private:

    vector<float> m_NewKeyFrameTimes{};

public:
    
    GameUIAnimationUI();
    virtual ~GameUIAnimationUI() override;
    
public:
    
    void SyncTrackEditCache(class CUIAnimation* _Animation);
    
    void Tick_UI() override;
};
