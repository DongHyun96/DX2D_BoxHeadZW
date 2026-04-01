#include "pch.h"
#include "CMuzzleFlashScript.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CMuzzleFlashScript::CMuzzleFlashScript()
    : CScript(SCRIPT_TYPE::MUZZLEFLASHSCRIPT)
{
}

CMuzzleFlashScript::~CMuzzleFlashScript()
{
}

void CMuzzleFlashScript::Begin()
{
    /*Ptr<AMaterial> DynamicMaterial = GetRenderCom()->CreateDynamicMaterial();
    DynamicMaterial->SetScalar(SCALAR_PARAM::VEC4_0, Vec4(1.f, 1.f, 1.f, 0.45f)); // 연기여서 알파를 좀 옅게 줌*/
    
    FlipbookRender()->AddNotifyFlipbookEndEvent(L"MuzzleFlash", 0, bind(&CMuzzleFlashScript::OnFlashAnimationEnd, this));
}

void CMuzzleFlashScript::AfterLevelBegin()
{
    // GM에 Owner PoolComponent 등록
    if (!GM->GetEffectPooler(EFFECT_POOLER_TYPE::MUZZLE_FLASH_POOLER))
    {
        // ObjectPooling 처리된 객체의 OwnerPooler에 한해서만 등록 처리를 진행시킴 (Testing 환경에서 Pooling 되지 않은 오브젝트가 있을 수 있음)
        if (CPoolComponent* PoolComponent = GetOwner()->GetOwnerPoolComponent())
        {
            GM->AddEffectPooler(EFFECT_POOLER_TYPE::MUZZLE_FLASH_POOLER, PoolComponent);
        }
    }
}

void CMuzzleFlashScript::Tick()
{
}
