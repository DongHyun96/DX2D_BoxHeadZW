#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"

class LevelUI : public AssetUI
{
public:
    
    LevelUI();
    virtual ~LevelUI() override;
    
public:
    
    void Tick_UI() override;
    
};
