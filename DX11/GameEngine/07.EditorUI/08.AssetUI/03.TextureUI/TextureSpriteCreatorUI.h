#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

class TextureSpriteCreatorUI : public EditorUI
{
private:

    Ptr<ATexture> m_TargetTexture{};
    
    Vec2 m_TotalFrameXYCount{}; // Atlas 내에서의 Sprite Frame count x, y
    string m_InputSpriteName{}; // Sprite의 이름이 될 string
    
public:
    
    TextureSpriteCreatorUI();
    virtual ~TextureSpriteCreatorUI() override;
    
public:
    
    void Tick_UI() override;

public:
    
    SET(Ptr<ATexture>, TargetTexture)
    
private:
    
    virtual void Activate() override;
    
};
