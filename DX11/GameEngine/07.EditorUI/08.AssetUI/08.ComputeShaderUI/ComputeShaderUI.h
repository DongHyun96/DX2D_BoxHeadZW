#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"

class ComputeShaderUI : public AssetUI
{
public:
    
    ComputeShaderUI();
    virtual ~ComputeShaderUI() override;
    
public:
    
    void Tick_UI() override;
    
};
