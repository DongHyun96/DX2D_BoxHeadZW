#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/RenderUI.h"

class FlipbookRenderUI : public RenderUI
{
private:
    int         m_SelectedFlipbookIdx = -1;

    bool        m_PreviewPlaying = true;
    bool        m_PreviewLoop = true;
    float       m_PreviewFPS = 10.f;
    float       m_PreviewAccTime = 0.f;
    int         m_PreviewCurFrame = 0;
    AFlipbook*  m_PreviewTarget = nullptr;

public:
    FlipbookRenderUI();
    virtual ~FlipbookRenderUI() override;

public:
    virtual void Tick_UI() override;

private:
    void TryAppendFromPayload(const ImGuiPayload* _Payload, const Ptr<CFlipbookRender>& _FlipbookRender);
    void DrawFlipbookList(const Ptr<CFlipbookRender>& _FlipbookRender);
    void DrawPreviewSection(const Ptr<CFlipbookRender>& _FlipbookRender);
    void DrawSpritePreview(const Ptr<ASprite>& _Sprite, float _MaxPreviewSize);
    
};
