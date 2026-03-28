#pragma once
#include "GameEngine/07.EditorUI/08.AssetUI/AssetUI.h"

class FlipbookUI : public AssetUI
{
private:
    int         m_SelectedSpriteIdx = -1;
    bool        m_PreviewPlaying    = true;
    bool        m_PreviewLoop       = true;
    float       m_PreviewFPS{10.f};
    float       m_PreviewAccTime{};
    int         m_PreviewCurFrame{};
    AFlipbook*  m_PreviewTarget{};

    bool        m_ShowEditReferencePoint = true;
    Vec2        m_EditReferencePointUV = Vec2(0.5f, 0.5f);
    
public:
    FlipbookUI();
    virtual ~FlipbookUI() override;

public:
    void Tick_UI() override;

private:
    
    void TickNextSpriteShortCut(const Ptr<AFlipbook>& _Flipbook);
    
    /// <returns> : 수정된 Current Zoom Factor </returns>
    float DrawSpritePreview
    (
        const Ptr<ASprite>& _Sprite,
        float               _MaxPreviewSize,
        bool                _EnableZoom = false,
        float               _CurrentZoomFactor = 1.f,
        bool                _DrawReferencePoint = false
    );
    
    void DrawPreviewSection(const Ptr<AFlipbook>& _Flipbook);
    
    void DrawSpriteTable
    (
        const Ptr<AFlipbook>&   _Flipbook,
        int&                    _RemoveIndex,
        int&                    _InsertIndex,
        vector<Ptr<ASprite>>&   _InsertSprites
    );
    void ApplySpriteTableEdits
    (
        const Ptr<AFlipbook>&       _Flipbook,
        int                         _RemoveIndex,
        int                         _InsertIndex,
        const vector<Ptr<ASprite>>& _InsertSprites
    );
    void DrawUVEditor(const Ptr<AFlipbook>& _Flipbook);
    void DrawAppendSection(const Ptr<AFlipbook>& _Flipbook);
    
private:
    
    void OnConfirmClearSprites(bool _Yes);

};
