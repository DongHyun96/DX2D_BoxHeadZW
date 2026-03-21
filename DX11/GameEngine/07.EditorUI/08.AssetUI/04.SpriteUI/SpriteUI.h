#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"

class SpriteUI : public AssetUI
{
private:
    
    float m_PreviewZoom     = 1.0f;
    float m_MinPreviewZoom  = 0.1f;
    float m_MaxPreviewZoom  = 4.0f;
    
    
public:
    SpriteUI();
    virtual ~SpriteUI() override;
    
public:
    virtual void Tick_UI() override;
    
public:
    
    static void DrawSpritePreview(const Ptr<ASprite>& _Sprite, float& _PreviewZoom, const float& _MinPreviewZoom, const float& _MaxPreviewZoom);

};
