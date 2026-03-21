#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/RenderUI.h"

class BillboardRenderUI : public RenderUI
{
public:

    BillboardRenderUI();
    virtual ~BillboardRenderUI() override;
    
public:
    
    void Tick_UI() override;
};

