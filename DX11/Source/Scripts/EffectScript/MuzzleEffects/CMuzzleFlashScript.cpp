#include "pch.h"
#include "CMuzzleFlashScript.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerWeaponHandler.h"
#include "Source/Scripts/Structure/CTurret.h"

const float CMuzzleFlashScript::s_OffsetFactorFromMuzzle = 1.2f;

CMuzzleFlashScript::CMuzzleFlashScript()
    : CFlipbookEffectScript(SCRIPT_TYPE::MUZZLEFLASHSCRIPT, FLIPBOOK_EFFECT_POOLER_TYPE::MUZZLE_FLASH_POOLER, L"Flash")
{
}

CMuzzleFlashScript::~CMuzzleFlashScript()
{
}

void CMuzzleFlashScript::Begin()
{
    CFlipbookEffectScript::Begin();
    
    GetOwner()->AddDeactivateDelegate(bind(&CMuzzleFlashScript::OnDeactivate, this, placeholders::_1));
}

void CMuzzleFlashScript::Tick()
{
    CFlipbookEffectScript::Tick();

    if (m_bIsPlayerWeaponMuzzle)
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
        return;
    }
    
    // Target Muzzle인 경우 (Owner가 TurretScript를 들고있음)
    // GetOwner의 GetParent가 Turret 게임 오브젝트
    if (CTurret* Turret = GetOwner()->GetParent()->GetScriptComponent<CTurret>().Get())
    {
        const Vec3 PinPointWorldPos = Turret->FlipbookRender()->GetCurrentSpritePinPointToWorldPos();
        Transform()->SetRelativePosFromWorldPos(PinPointWorldPos);
        Transform()->SetRelativeRotZ(Turret->GetCurrentFacedAngle());
    }
}

void CMuzzleFlashScript::OnDeactivate(const Ptr<GameObject>& _Owner)
{
    m_bIsPlayerWeaponMuzzle = true;
}
