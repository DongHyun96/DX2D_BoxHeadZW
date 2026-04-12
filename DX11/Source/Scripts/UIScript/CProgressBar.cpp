#include "pch.h"
#include "CProgressBar.h"
#include "Source/ScriptMgr.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"

CProgressBar::CProgressBar()
    : CGameUI(PROGRESSBAR)
    , m_fRatio(1.f)
{
}

CProgressBar::CProgressBar(const CProgressBar& _Origin)
    : CGameUI(_Origin)
    , m_fRatio(_Origin.m_fRatio)
{
}

CProgressBar::~CProgressBar()
{
}

void CProgressBar::Tick()
{
    CGameUI::Tick();

    // Ratio에 따라 가로 크기(Scale) 조절 예시
    // 주의: Pivot이 (0.0, 0.5) 등으로 설정되어 있어야 왼쪽에서부터 차오르는 효과가 남
    Vec3 vScale = Transform()->GetRelativeScale();
    // vScale.x = m_fFullWidth * m_fRatio; // 이런 식의 로직이 필요할 수 있음
}
