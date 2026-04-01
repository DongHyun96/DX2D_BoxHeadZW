#include "pch.h"
#include "CMuzzleFlashScript.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerWeaponHandler.h"

const float CMuzzleFlashScript::s_OffsetFactorFromMuzzle = 1.2f;

CMuzzleFlashScript::CMuzzleFlashScript()
    : CFlipbookEffectScript(SCRIPT_TYPE::MUZZLEFLASHSCRIPT, FLIPBOOK_EFFECT_POOLER_TYPE::MUZZLE_FLASH_POOLER, L"Flash")
{
}

CMuzzleFlashScript::~CMuzzleFlashScript()
{
}

void CMuzzleFlashScript::Tick()
{
    CFlipbookEffectScript::Tick();
    
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
