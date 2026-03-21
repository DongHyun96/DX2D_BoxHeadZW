#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"

class SoundUI : public AssetUI
{
public:
    
    SoundUI();
    virtual ~SoundUI() override;
    
public:
    
    void Tick_UI() override;
    
};
