#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/ComponentUI.h"

class ColliderPointUI : public ComponentUI
{
public:
    
    ColliderPointUI();
    virtual ~ColliderPointUI() override;

public:
    virtual void Tick_UI() override;
    
};
