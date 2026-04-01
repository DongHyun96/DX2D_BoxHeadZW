#include "pch.h"
#include "CRocketSmoke.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CRocketSmoke::CRocketSmoke()
    : CScript(SCRIPT_TYPE::ROCKETSMOKE)
{
}

CRocketSmoke::~CRocketSmoke()
{
}

void CRocketSmoke::Begin()
{
    FlipbookRender()->SetCurrentCategory(L"Smoke");
    FlipbookRender()->AddNotifyFlipbookEndEvent(L"Smoke", 0, bind(&CRocketSmoke::OnSmokeAnimationEnd, this));
    /*Ptr<AMaterial> Mtrl = FlipbookRender()->CreateDynamicMaterial();
    Mtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASKED);*/
}

void CRocketSmoke::AfterLevelBegin()
{
    // GM에 Owner PoolComponent 등록
    if (!GM->GetEffectPooler(EFFECT_POOLER_TYPE::ROCKET_SMOKE_POOLER))
    {
        // ObjectPooling 처리된 객체의 OwnerPooler에 한해서만 등록 처리를 진행시킴 (Testing 환경에서 Pooling 되지 않은 오브젝트가 있을 수 있음)
        if (CPoolComponent* PoolComponent = GetOwner()->GetOwnerPoolComponent())
            GM->AddEffectPooler(EFFECT_POOLER_TYPE::ROCKET_SMOKE_POOLER, PoolComponent);
    }
}

void CRocketSmoke::Tick()
{
}
