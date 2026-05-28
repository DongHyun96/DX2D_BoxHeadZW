#include "pch.h"
#include "CProgressBar.h"
#include "Source/ScriptMgr.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"

CProgressBar::CProgressBar()
    : CGameUI(PROGRESSBAR)
    , m_Ratio(1.f)
{
}

CProgressBar::~CProgressBar()
{
}

void CProgressBar::Init()
{
    CGameUI::Init();
    
    AddScriptParam(SCRIPT_PARAM::PROGRESS_BAR, this, L"Ratio", false, 0.1f);
    
    GetOwner()->GetRenderCom()->CreateDynamicMaterial();
    
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

float CProgressBar::SetRatio(float _Ratio)
{
    m_Ratio = _Ratio;
    m_Ratio = clamp(m_Ratio, 0.f, 1.f);
    GetOwner()->GetRenderCom()->SetRenderScale(Vec2(m_Ratio, 1.f));
    return m_Ratio;
}
