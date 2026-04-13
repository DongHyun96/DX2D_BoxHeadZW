#include "pch.h"
#include "GameManager.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/AStar/AStarPathFinder.h"
#include "Source/Scripts/ExplosionDome/CExplosion.h"
#include "Source/Scripts/ExplosionDome/CExplosionDome.h"
#include "Source/Scripts/ExplosionDome/CFirePillarHandler.h"
#include "Source/Scripts/ProjectileScript/CGrenade.h"
#include "Source/Scripts/ProjectileScript/CRocketProjectile.h"
#include "Source/Scripts/Structure/CStructure.h"

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

void GameManager::OnLevelBegin()
{
    // Level에 들어있는 Layer들의 Begin 전에 먼저 호출 처리됨
    
    m_PlayerObject      = nullptr;
    m_MainPlayerScript  = nullptr;
    
    m_mapFlipbookEffectPoolers.clear();
    
    m_RocketProjectilePooler = nullptr;
    m_GrenadePooler          = nullptr;
    m_BackgroundCellManager  = nullptr;
    m_EnemySpawnHandler      = nullptr;
    m_FirePillarHandler      = nullptr;
    m_FirstSpawnLocManager   = nullptr;
    m_ItemPooler             = nullptr;
    
    CStructure::ClearInstalledInfo();
    AStarPathFinder::Init();
}

void GameManager::OnLevelPlayToStop()
{
    m_PlayerObject      = nullptr;
    m_MainPlayerScript  = nullptr;
    
    m_mapFlipbookEffectPoolers.clear();

    m_RocketProjectilePooler = nullptr;
    m_GrenadePooler          = nullptr;
    m_BackgroundCellManager  = nullptr;
    m_EnemySpawnHandler      = nullptr;
    m_FirePillarHandler      = nullptr;
    m_FirstSpawnLocManager   = nullptr;
    m_ItemPooler             = nullptr;
    
    CStructure::ClearInstalledInfo();
    
    m_bHasGameStart = false;
}

void GameManager::OnLevelChanged(ALevel* _PrevLevel, ALevel* _NextLevel)
{
    // TODO : 특정 Level에서 다른 Level 로 넘어갈 때 처리할 것 처리하기
    CStructure::ClearInstalledInfo();
    AStarPathFinder::Init();
}

CPoolComponent* GameManager::GetFlipbookEffectPooler(FLIPBOOK_EFFECT_POOLER_TYPE _PoolerType) const
{
    if (!m_mapFlipbookEffectPoolers.contains(_PoolerType)) return nullptr;
    return m_mapFlipbookEffectPoolers.at(_PoolerType);
}

void GameManager::SpawnRocketSmoke(const Vec3& _SpawnPos)
{
    GameObject* Object = m_mapFlipbookEffectPoolers[FLIPBOOK_EFFECT_POOLER_TYPE::ROCKET_SMOKE_POOLER]->SpawnObject();
    if (Object)
    {
        Object->Transform()->SetRelativePosX(_SpawnPos.x);
        Object->Transform()->SetRelativePosY(_SpawnPos.y);
        Object->FlipbookRender()->Play(0, 20.f, 1);
    }
}

void GameManager::SpawnExplosionDome(const Vec3& _SpawnPos, float _ExplosionSizeFactor, float _FPS, float _DamageAmount, CScript* _SpawnedBy)
{
    GameObject* Object = m_mapFlipbookEffectPoolers[FLIPBOOK_EFFECT_POOLER_TYPE::EXPLOSION_DOME_EFFECT_POOLER]->SpawnObject();
    if (Object)
    {
        Object->Transform()->SetRelativePosX(_SpawnPos.x);
        Object->Transform()->SetRelativePosY(_SpawnPos.y);
        Object->FlipbookRender()->Play(0, _FPS, 1);
        
        if (Ptr<CExplosionDome> ExplosionDome = Object->GetScriptComponent<CExplosionDome>())
        {
            ExplosionDome->ClearAlreadyDamaged();
            ExplosionDome->SetDamage(_DamageAmount);
            ExplosionDome->SetExplosionSize(_ExplosionSizeFactor);
            ExplosionDome->SetSpawnedBy(_SpawnedBy);
        }
        
        Ptr<ASound> Sound = FIND_ASSET(ASound, L"Sound\\Explosion1.mp3");
        Sound->Play(1, 0.5f, true);
    }
}

void GameManager::SpawnExplosion(const ExplosionSpawnDesc& _Desc)
{
    GameObject* Object = m_mapFlipbookEffectPoolers[FLIPBOOK_EFFECT_POOLER_TYPE::EXPLOSION_EFFECT_POOLER]->SpawnObject();
    if (Object)
    {
        Object->Transform()->SetRelativePosX(_Desc.SpawnPos.x);
        Object->Transform()->SetRelativePosY(_Desc.SpawnPos.y);
        Object->FlipbookRender()->Play(0, _Desc.FPS, 1);
        
        if (Ptr<CExplosion> Explosion = Object->GetScriptComponent<CExplosion>())
        {
            Explosion->ClearAlreadyDamaged();
            Explosion->SetDamage(_Desc.DamageAmount);
            Explosion->SetExplosionSize(_Desc.ExplosionSizeFactor);
            Explosion->SetSpawnedBy(_Desc.SpawnedBy);
            Explosion->SetUseCollisionForDamaging(_Desc.UseCollisionForDamaging);
            Explosion->SetUpwardVelocity(_Desc.UpwardVelocity);
            Explosion->ConfigureDamagePulse(_Desc.DamagePulseDelaySec, _Desc.DamagePulseDurationSec, _Desc.DamagePulseSpriteIdx);
            Explosion->SetSecondaryBurst
            (
                _Desc.SecondaryBurstCount,
                _Desc.SecondaryBurstRadius,
                _Desc.SecondaryBurstMinDelaySec,
                _Desc.SecondaryBurstMaxDelaySec,
                _Desc.SecondaryBurstDamageScale,
                _Desc.SecondaryBurstSizeScale,
                _Desc.SecondaryBurstPlaySound
            );
            if (_Desc.DamageAmount <= 0.f) Explosion->GetCollider2D()->SetActive(false);
        }

        if (_Desc.PlayExplosionSound)
        {
            Ptr<ASound> Sound = FIND_ASSET(ASound, L"Sound\\Explosion1.mp3");
            Sound->Play(1, 0.5f, true);
        }
    }
}

bool GameManager::SpawnFirePillar(const FirePillarSpawnDesc& _Desc)
{
    if (!m_FirePillarHandler) return false;
    m_FirePillarHandler->SpawnFirePillar(_Desc);
    return true;
}

bool GameManager::SpawnFirePillar(const Vec3& _SpawnPos, float _DamageAmount, CScript* _SpawnedBy)
{
    if (!m_FirePillarHandler) return false;
    m_FirePillarHandler->SpawnFirePillar(_SpawnPos, _DamageAmount, _SpawnedBy);
    return true;
}

bool GameManager::SpawnRocketProjectile(const Vec3& _SpawnPos, const Vec2& _Direction, float _Damage)
{
    if (!m_RocketProjectilePooler) return false;
    
    GameObject* SpawnedRocketObj = m_RocketProjectilePooler->SpawnObject(_SpawnPos);
    if (!SpawnedRocketObj) return false; // Pool Count 부족
    
    Ptr<CRocketProjectile> ProjectileScript = SpawnedRocketObj->GetScriptComponent<CRocketProjectile>();
    ProjectileScript->SetDirection(_Direction.Normalized());
    ProjectileScript->SetDamage(_Damage);
    
    SpawnedRocketObj->FlipbookRender()->Play(0, 15.f, -1);
    
    // Sound Play
    Ptr<ASound> Sound = FIND_ASSET(ASound, L"Sound\\RocketShot.mp3");
    Sound->Play(1, 0.5f, true);
    
    return true;
}

bool GameManager::SpawnGrenade
(
    const Vec3&     _SpawnPos,
    const Vec2&     _Direction,
    float           _Damage,
    int             _BounceCount,
    float           _ThrowSpeedXY,
    float           _UpwardSpeed,
    bool            _SpawnSubGrenade,    
    bool            _IsSubGrenade    
)
{
    if (!m_GrenadePooler) return false;
    
    GameObject* SpawnedGrenade = m_GrenadePooler->SpawnObject(_SpawnPos);
    if (!SpawnedGrenade) return false;
    
    Ptr<CGrenade> Grenade = SpawnedGrenade->GetScriptComponent<CGrenade>();
            
    // 논리적 시작 위치 (렌더링 시작점)
    // z값은 0으로 시작 x -> Bounce Count 하나 까고 시작해버림
    Grenade->SetLogicalPos(ToVec3(ToVec2(_SpawnPos), 1.f));

            
    // 최종 속도(Velocity) 조합
    Vec3 FireVelocity;
    FireVelocity.x = _Direction.x * _ThrowSpeedXY;
    FireVelocity.y = _Direction.y * _UpwardSpeed;
    FireVelocity.z = _UpwardSpeed;

    Grenade->SetDamageAmount(_Damage);
    Grenade->SetFireVelocity(FireVelocity);
    Grenade->SetBounceTotalCount(_BounceCount);
    Grenade->SetSpawnSubGrenade(_SpawnSubGrenade);
    
    _IsSubGrenade ? Grenade->SetSubGrenadeScale() : Grenade->SetMainGrenadeScale();
    return true;
}
