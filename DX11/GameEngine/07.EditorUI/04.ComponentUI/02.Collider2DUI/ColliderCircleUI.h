#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"

class ColliderCircleUI : public ComponentUI
{
public:
    
    ColliderCircleUI();
    virtual ~ColliderCircleUI() override;
    
public:
    
    virtual void Tick_UI() override;
    
};
