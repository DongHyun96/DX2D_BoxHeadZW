#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/RenderUI.h"

class SpriteRenderUI : public RenderUI
{
private:
    
    float m_PreviewZoom     = 1.0f;
    float m_MinPreviewZoom  = 0.1f;
    float m_MaxPreviewZoom  = 4.0f;
    
public:
    
    SpriteRenderUI();
    virtual ~SpriteRenderUI() override;
    
public:
    
    void Tick_UI() override;
    
};
