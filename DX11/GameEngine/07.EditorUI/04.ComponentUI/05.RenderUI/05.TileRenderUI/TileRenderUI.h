#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/RenderUI.h"

class TileRenderUI : public RenderUI
{
public:
    
    TileRenderUI();
    virtual ~TileRenderUI() override;
    
public:
    
    virtual void Tick_UI() override;
    
};
