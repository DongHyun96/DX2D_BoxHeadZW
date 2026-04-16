#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"

class LevelUI : public AssetUI
{
private:
    
    ALevel* m_Level{};
    
public:
    
    LevelUI();
    virtual ~LevelUI() override;
    
public:
    
    void Tick_UI() override;
    
};
