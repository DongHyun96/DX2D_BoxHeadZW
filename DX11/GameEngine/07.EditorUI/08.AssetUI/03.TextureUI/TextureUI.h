#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"

class TextureUI : public AssetUI
{
public:
    TextureUI();
    virtual ~TextureUI() override;
    
public:
    virtual void Tick_UI() override;

};

