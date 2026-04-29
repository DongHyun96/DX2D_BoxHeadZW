#pragma once

#include "GameEngine/07.EditorUI/04.ComponentUI/06.ScriptUI/CustomScriptUI/CustomScriptUI.h"

class CUIAnimation;
struct UIAnimTrack;
class GameObject;

class GameUIAnimationUI : public CustomScriptUI
{
private:
    // UI 상태 관리 변수들 (Selection)
    int m_SelectedTrackIdx = -1;
    int m_SelectedKeyIdx   = -1;
    
    // 타임라인 시각화용 줌 스케일 (1초당 픽셀 수)
    float m_TimelineScale = 150.0f; 

    // 트랙/키프레임 삭제 지연 처리
    int m_RemoveTrackIdx    = -1;
    int m_RemoveKeyTrackIdx = -1;
    int m_RemoveKeyIdx      = -1;

public:
    GameUIAnimationUI();
    virtual ~GameUIAnimationUI() override;
    
    void Tick_UI() override;

private:
    void RenderToolbar(CUIAnimation* _Animation, bool _bCanEdit);
    void RenderMainEditor(CUIAnimation* _Animation, bool _bCanEdit);
    void RenderBottomInspector(CUIAnimation* _Animation, bool _bCanEdit);
    
    void HandlePendingRemovals(CUIAnimation* _Animation);
    void ClearSelectionIfInvalid(CUIAnimation* _Animation);
};