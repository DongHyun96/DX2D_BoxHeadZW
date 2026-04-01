#include "pch.h"
#include "CMuzzleFlashScript.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerWeaponHandler.h"

const float CMuzzleFlashScript::s_OffsetFactorFromMuzzle = 1.2f;

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
    
    FlipbookRender()->SetCurrentCategory(L"Flash");
    FlipbookRender()->AddNotifyFlipbookEndEvent(L"Flash", 0, bind(&CMuzzleFlashScript::OnFlashAnimationEnd, this));
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
    // 자기 자신의 Relative 위치를 Player의 Anim 회전과 동일한 방향으로 업데이트 처리해주어야 한다.5
    // & 회전 또한 처리를 매 프레임 해주어야 함
    Ptr<CPlayerWeaponHandler> WeaponHandler = GM->GetPlayerObject()->GetScriptComponent<CPlayerWeaponHandler>().Get();    
    
    if (WeaponHandler)
    {
        const Vec2& CurrentOffset = WeaponHandler->GetCurrentMuzzleOffset() * s_OffsetFactorFromMuzzle;
        Transform()->SetRelativePosX(CurrentOffset.x);
        Transform()->SetRelativePosY(CurrentOffset.y);
        
        const EDIRECTION PlayerDirection = GM->GetMainPlayerScript()->GetCurrentFacedDirection();
        const float Angle = GetEightDirectionToAngle(PlayerDirection);
        Transform()->SetRelativeRotZ(Angle);
    }
}
