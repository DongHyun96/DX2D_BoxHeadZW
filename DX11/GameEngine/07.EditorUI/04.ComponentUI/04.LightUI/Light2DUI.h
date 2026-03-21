#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"

class Light2DUI : public ComponentUI
{
private:

public:
    
    Light2DUI();
    virtual ~Light2DUI() override;
    
public:
    
    virtual void Tick_UI() override;    
    
};
