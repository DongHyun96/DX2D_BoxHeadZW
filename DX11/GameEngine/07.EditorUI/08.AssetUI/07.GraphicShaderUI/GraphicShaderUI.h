#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"

class GraphicShaderUI : public AssetUI
{
public:
    
    GraphicShaderUI();
    virtual ~GraphicShaderUI() override;
    
public:
    
    void Tick_UI() override;
    
};
