#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

/// <summary>
/// 현재 Level의 CollisionMatrix 수정 및, Layer이름 수정 기능 담당 UI
/// </summary>
class CollisionMatrixUI : public EditorUI
{
private:
    bool m_NameInit{};
    char m_LayerNameBuf[MAX_LAYER][64]{};

public:
    CollisionMatrixUI();
    virtual ~CollisionMatrixUI() override;

    virtual void Tick_UI() override;
    
    void RefreshFromLevel();
};
