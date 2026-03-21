#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"

class ColliderRectUI : public ComponentUI
{
public:
    
    ColliderRectUI();
    virtual ~ColliderRectUI() override;

public:
    virtual void Tick_UI() override;
    
};

