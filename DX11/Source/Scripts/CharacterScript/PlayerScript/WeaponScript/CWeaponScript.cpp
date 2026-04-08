#include "pch.h"
#include "CWeaponScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"


vector<UINT> CWeaponScript::m_HitScanLayer{};

CWeaponScript::CWeaponScript(SCRIPT_TYPE _ScriptType)
    : CScript(_ScriptType)
{
}

CWeaponScript::~CWeaponScript()
{
}

void CWeaponScript::WeaponTick()
{
    m_TimeAfterLastFire = min(m_TimeAfterLastFire + DT, m_FireIntervalTime + 1.f);
}

bool CWeaponScript::AddHitScanLayer(int _Layer)
{
    if (_Layer < 0) return false;
    m_HitScanLayer.push_back(_Layer);
    return true;
}

void CWeaponScript::SpawnMuzzleEffects(const Vec2& _MuzzleWorldPos)
{
    SpawnMuzzleFlash(_MuzzleWorldPos);
    
    const EDIRECTION PlayerDirection = GM->GetMainPlayerScript()->GetCurrentFacedDirection();
    const float Angle = GetEightDirectionToAngle(PlayerDirection);
    SpawnMuzzleSmoke(_MuzzleWorldPos, Angle);
}

void CWeaponScript::SpawnMuzzleFlash(const Vec2& _MuzzleWorldPos)
{
    GameObject* SpawnedFlashEffect = GM->GetFlipbookEffectPooler(FLIPBOOK_EFFECT_POOLER_TYPE::PLAYER_MUZZLE_FLASH_POOLER)->SpawnObject(ToVec3(_MuzzleWorldPos)); // 두 번째 Play 시, 
    if (SpawnedFlashEffect)
    {
        const EDIRECTION PlayerDirection = GM->GetMainPlayerScript()->GetCurrentFacedDirection();
        const float Angle = GetEightDirectionToAngle(PlayerDirection);
        
        SpawnedFlashEffect->Transform()->SetRelativeRotZ(Angle);

        // Player Character를 따라가도록 처리를 해야한다 (이미 해당 Object가 AddChild 처리되었다면, 처리 2번 안당하게끔 처리함)
        // TODO : 이거 PlayerMuzzleFlashEffect 사전에 모두 자식으로 추가를 해놓기 (AddChild하면서 성능저하 이슈 있음)
        GM->GetPlayerObject()->AddChild(SpawnedFlashEffect);
        
        SpawnedFlashEffect->FlipbookRender()->Play(0, 15.f, 1);
    }
}

void CWeaponScript::SpawnMuzzleSmoke(const Vec2& _MuzzleWorldPos, float _RotAngle)
{
    GameObject* SpawnedFlashSmoke = GM->GetFlipbookEffectPooler(FLIPBOOK_EFFECT_POOLER_TYPE::MUZZLE_SMOKE_POOLER)->SpawnObject(ToVec3(_MuzzleWorldPos));
    if (SpawnedFlashSmoke)
    {
        SpawnedFlashSmoke->Transform()->SetRelativeRotZ(_RotAngle);
        SpawnedFlashSmoke->FlipbookRender()->Play(0, 15.f, 1);
    }
}

