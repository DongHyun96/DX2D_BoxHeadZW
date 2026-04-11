#include "pch.h"
#include "CMuzzleFlashScript.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerWeaponHandler.h"
#include "Source/Scripts/Structure/CTurret.h"

const float CMuzzleFlashScript::s_OffsetFactorFromMuzzle = 1.2f;

CMuzzleFlashScript::CMuzzleFlashScript()
    : CFlipbookEffectScript(SCRIPT_TYPE::MUZZLEFLASHSCRIPT, FLIPBOOK_EFFECT_POOLER_TYPE::PLAYER_MUZZLE_FLASH_POOLER, L"Flash")
{
}

CMuzzleFlashScript::~CMuzzleFlashScript()
{
}

void CMuzzleFlashScript::Init()
{
    CFlipbookEffectScript::Init();
    AddScriptParam(SCRIPT_PARAM::INT, &m_bIsPlayerWeaponMuzzle, L"IsPlayerWeaponMuzzle?");
}

void CMuzzleFlashScript::Begin()
{
    CFlipbookEffectScript::Begin();
    
    if (m_bIsPlayerWeaponMuzzle)
    {
        m_WeaponHandler = GM->GetPlayerObject()->GetScriptComponent<CPlayerWeaponHandler>().Get();
    }
}

void CMuzzleFlashScript::AfterLevelBegin()
{
    // 여기서 GM에 FlipbookEffectPooler type 종류로 PoolComponent를 등록 처리한다 -> 이거 하지 말고 개인이 들고 있도록 처리
    // CFlipbookEffectScript::AfterLevelBegin();
    
    // 자기 자식으로 들어가게끔 처리
    // 문제점 -> Turret의 경우, CreateObject 처리가 되기 때문에 여기로 호출 처리가 안됨
    
    // 구현부는 놔둬야, GM PoolComponent에 등록처리를 하지 않음 -> 안전처리로 그냥 놔둠
}



void CMuzzleFlashScript::Tick()
{
    CFlipbookEffectScript::Tick();

    // 자기 자신의 Relative 위치를 Player의 Anim 회전과 동일한 방향으로 업데이트 처리해주어야 한다.5
    // & 회전 또한 처리를 매 프레임 해주어야 함
    if (m_WeaponHandler) // Player Muzzle effect인 경우
    {
        const Vec2& CurrentOffset = m_WeaponHandler->GetCurrentMuzzleOffset() * s_OffsetFactorFromMuzzle;
        Transform()->SetRelativePosX(CurrentOffset.x);
        Transform()->SetRelativePosY(CurrentOffset.y);
        
        const EDIRECTION PlayerDirection = GM->GetMainPlayerScript()->GetCurrentFacedDirection();
        const float Angle = GetEightDirectionToAngle(PlayerDirection);
        Transform()->SetRelativeRotZ(Angle);
        return;
    }
        
    
    // Target Muzzle인 경우 (Owner가 TurretScript를 들고있음) -> Turret이 Destroy당했을 수 있음
    // GetOwner의 GetParent가 Turret 게임 오브젝트
    GameObject* pParent = GetOwner()->GetParent();
    if (pParent)
    {
        if (CTurret* Turret = pParent->GetScriptComponent<CTurret>().Get())
        {
            const Vec3 PinPointWorldPos = Turret->FlipbookRender()->GetCurrentSpritePinPointToWorldPos();
            Transform()->SetRelativePosFromWorldPos(PinPointWorldPos);
            Transform()->SetRelativeRotZ(Turret->GetCurrentFacedAngle());
        }
    }
}

void CMuzzleFlashScript::SaveToLevelFile(FILE* _File)
{
    CFlipbookEffectScript::SaveToLevelFile(_File);
    fwrite(&m_bIsPlayerWeaponMuzzle, sizeof(bool), 1, _File);
}

void CMuzzleFlashScript::LoadFromLevelFile(FILE* _File)
{
    CFlipbookEffectScript::LoadFromLevelFile(_File);
    fread(&m_bIsPlayerWeaponMuzzle, sizeof(bool), 1, _File);
    
}
