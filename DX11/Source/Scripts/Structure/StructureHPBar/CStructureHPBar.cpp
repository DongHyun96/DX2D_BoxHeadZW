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
            // m_MainBarMtrl               = m_MainProgressBarGameObject->GetRenderCom()->CreateDynamicMaterial().Get();
            m_MainBarMtrl               = m_MainProgressBarGameObject->GetRenderCom()->GetMaterial().Get();
            m_MainBarTintColorOrigin    = m_MainBarMtrl->GetScalar<Vec4>(VEC4_0);
        }
        else
        {
            m_BackgroundGameObject      = Child.Get();
            // m_BackgroundBarMtrl         = m_BackgroundGameObject->GetRenderCom()->CreateDynamicMaterial().Get();
            m_BackgroundBarMtrl         = m_BackgroundGameObject->GetRenderCom()->GetMaterial().Get();
            m_BackgroundTintColorOrigin = m_BackgroundBarMtrl->GetScalar<Vec4>(VEC4_0);
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

    const float MainBarAlpha    = MappingToNewRangeClamped(DistanceSqrt, 0.f, MAX_VISIBLE_RANGE_SQRT, m_MainBarTintColorOrigin.w, 0.f);
    const float BGAlpha         = MappingToNewRangeClamped(DistanceSqrt, 0.f, MAX_VISIBLE_RANGE_SQRT, m_BackgroundTintColorOrigin.w, 0.f);
    
    m_MainBarMtrl->SetScalar(SCALAR_PARAM::VEC4_0, Vec4(m_MainBarTintColorOrigin.x, m_MainBarTintColorOrigin.y, m_MainBarTintColorOrigin.z, MainBarAlpha));
    m_BackgroundBarMtrl->SetScalar(SCALAR_PARAM::VEC4_0, Vec4(m_BackgroundTintColorOrigin.x, m_BackgroundTintColorOrigin.y, m_BackgroundTintColorOrigin.z, BGAlpha));
}

bool CStructureHPBar::UpdateHPBar(float _HP, float _MaxHP)
{
    if (_MaxHP <= 0.f) return false;
    m_MainProgressBar->SetRatio(_HP / _MaxHP);
    

    // Color 조정
    m_MainBarMtrl->GetScalar<Vec4>(VEC4_0);
    
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
