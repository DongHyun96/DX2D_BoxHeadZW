#include "pch.h"
#include "CProgressBar.h"
#include "Source/ScriptMgr.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"

CProgressBar::CProgressBar()
    : CGameUI(PROGRESSBAR)
    , m_fRatio(1.f)
{
}

CProgressBar::~CProgressBar()
{
}

void CProgressBar::Init()
{
    CGameUI::Init();
    
    AddScriptParam(SCRIPT_PARAM::PROGRESS_BAR, this, L"Ratio", false, 0.1f); // 이거 멤버변수만 바꾸는 중 -> 실질적으로 SetRenderScale 처리를 해야 제대로 반영이 됨
    
    // 왼쪽에서 오른쪽으로 차오르는 효과를 위해 피벗을 왼쪽 중앙(0, 0.5)으로 설정
    GetOwner()->GetRenderCom()->SetRenderPivot(Vec2(0.f, 0.5f));
}

void CProgressBar::Begin()
{
    CGameUI::Begin();
}

void CProgressBar::Tick()
{
    CGameUI::Tick();
}

void CProgressBar::SetRatio(float _fRatio)
{
    m_fRatio = _fRatio;
    m_fRatio = clamp(m_fRatio, 0.f, 1.f);
    GetOwner()->GetRenderCom()->SetRenderScale(Vec2(m_fRatio, 1.f));
}
