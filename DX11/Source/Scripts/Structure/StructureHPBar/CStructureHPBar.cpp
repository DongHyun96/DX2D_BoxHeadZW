#include "pch.h"
#include "CStructureHPBar.h"
#include "Source/Scripts/UIScript/CProgressBar.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CStructureHPBar::CStructureHPBar()
    : CScript(SCRIPT_TYPE::STRUCTUREHPBAR)
{
}

CStructureHPBar::~CStructureHPBar()
{
}

void CStructureHPBar::Begin()
{
    for (const Ptr<GameObject>& Child : GetOwner()->GetChildren())
    {
        if (Child->GetName() == L"ProgressBarDefault")
        {
            m_MainProgressBarGameObject = Child.Get();
            m_MainProgressBar           = m_MainProgressBarGameObject->GetScriptComponent<CProgressBar>().Get();
            m_MainBarMtrl               = m_MainProgressBarGameObject->GetRenderCom()->GetMaterial().Get();
            m_MainBarTintAlphaOrigin    = m_MainBarMtrl->GetTintColorAlpha();
        }
        else
        {
            m_BackgroundGameObject      = Child.Get();
            m_BackgroundBarMtrl         = m_BackgroundGameObject->GetRenderCom()->GetMaterial().Get();
            m_BackgroundTintAlphaOrigin = m_BackgroundBarMtrl->GetTintColorAlpha();
        }
    }

    m_PlayerTransform    = GM->GetPlayerObject()->Transform().Get();
    m_StructureTransform = GetOwner()->GetParent()->Transform().Get();
}

void CStructureHPBar::Tick()
{
    // Player로부터 200 반경까지는 보이게끔 처리
    static const float MAX_VISIBLE_RANGE_SQRT = 200.f * 200.f;
    const float DistanceSqrt = Vec2::DistanceSquared(m_PlayerTransform->GetRelativePosXY(), m_StructureTransform->GetRelativePosXY());

    const float MainBarAlpha    = MappingToNewRangeClamped(DistanceSqrt, 0.f, MAX_VISIBLE_RANGE_SQRT, m_MainBarTintAlphaOrigin, 0.f);
    const float BGAlpha         = MappingToNewRangeClamped(DistanceSqrt, 0.f, MAX_VISIBLE_RANGE_SQRT, m_BackgroundTintAlphaOrigin, 0.f);

    m_MainBarMtrl->SetTintColorAlpha(MainBarAlpha);
    m_BackgroundBarMtrl->SetTintColorAlpha(BGAlpha);
}

bool CStructureHPBar::UpdateHPBar(float _HP, float _MaxHP)
{
    if (_MaxHP <= 0.f) return false;
    const float Ratio = m_MainProgressBar->SetRatio(_HP / _MaxHP);

    // Color 조정
    const Vec3 CurrentColor = Vec3::Lerp({1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, Ratio);
    m_MainBarMtrl->SetTintColor(CurrentColor);    
    return true;
}

void CStructureHPBar::UpdateHPBar(float _Ratio)
{
    m_MainProgressBar->SetRatio(_Ratio);
}

void CStructureHPBar::SaveToLevelFile(FILE* _File)
{
}

void CStructureHPBar::LoadFromLevelFile(FILE* _File)
{
}
