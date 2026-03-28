#pragma once
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/RenderUI.h"

class FlipbookRenderUI : public RenderUI
{
private:
    
    wstring m_CurSelectedCategory{}; // FlipbookRenderUI 내에서 사용할 SelectedCategory 정보
    
private:
    
    char m_NewCategoryNameBuf[128]{};
    
private:
    
    // Editing 중 Select 한 NumberKey Shortcut (-1인 경우 아무 키 x)
    int m_NumShortCutIdxTapped   = -1;
    int m_CategoryShortCutTapped = -1;
    
private: // 현재 선택된 Category 내에서의 Flipbook vector 관련 데이터
    
    int         m_SelectedFlipbookIdx = -1;

    bool        m_PreviewPlaying    = true;
    bool        m_PreviewLoop       = true;
    float       m_PreviewFPS        = 10.f;
    float       m_PreviewAccTime    = 0.f;
    int         m_PreviewCurFrame   = 0;
    AFlipbook*  m_PreviewTarget     = nullptr;

public:
    FlipbookRenderUI();
    virtual ~FlipbookRenderUI() override;

public:
    virtual void Tick_UI() override;
    
private:
    
    void TickCheckNumberKey(const Ptr<CFlipbookRender>& _FlipbookRender);
    
    /// <summary>
    /// 실질적으로 CFlipbookRender에 미리 처음에 세팅될 데이터들 세팅 (Category, 카테고리 내에서의 어떤 플립북을 선택할지 등) 
    /// </summary>
    void TickSelectingLevelBeginningInfo(const Ptr<CFlipbookRender>& _FlipbookRender);
    
    void TickPreview(const Ptr<CFlipbookRender>& _FlipbookRender);

private:
    
    /// <summary>
    /// Editor Preview용 카테고리 지정 
    /// </summary>
    void TickSelectPreviewCategory(const Ptr<CFlipbookRender>& _FlipbookRender);
    
    void TickAddNewCategory(const Ptr<CFlipbookRender>& _FlipbookRender);
    void TickRemoveCategory(const Ptr<CFlipbookRender>& _FlipbookRender);
    
    
    void TryAppendFromPayload(const ImGuiPayload* _Payload, const Ptr<CFlipbookRender>& _FlipbookRender);
    void DrawFlipbookList(const Ptr<CFlipbookRender>& _FlipbookRender);
    void DrawPreviewSection(const Ptr<CFlipbookRender>& _FlipbookRender);
    void DrawSpritePreview(const Ptr<ASprite>& _Sprite, float _MaxPreviewSize);
    
private:
    
    void OnRemoveCategoryConfirmed(bool _Yes);
    
};
