#include "pch.h"
#include "GameManager.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/AStar/AStarPathFinder.h"
#include "Source/Scripts/ExplosionDome/CExplosion.h"
#include "Source/Scripts/ExplosionDome/CExplosionDome.h"
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
    
    CStructure::ClearInstalledInfo();
    AStarPathFinder::Init();
}

void GameManager::OnLevelPlayToStop()
{
    m_PlayerObject = nullptr;
    m_MainPlayerScript = nullptr;
    
    m_mapEnemyPoolers.clear();
    m_mapFlipbookEffectPoolers.clear();
    
    m_RocketProjectilePooler = nullptr;
    m_GrenadePooler = nullptr;
    m_BackgroundCellManager = nullptr;
    
    CStructure::ClearInstalledInfo();
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

CPoolComponent* GameManager::GetEnemyPooler(ENEMY_TYPE _EnemyType) const
{
    if (!m_mapEnemyPoolers.contains(_EnemyType)) return nullptr;
    return m_mapEnemyPoolers.at(_EnemyType);
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

void GameManager::SpawnExplosion
(
    const Vec3& _SpawnPos,
    float       _ExplosionSizeFactor,
    float       _FPS,
    float       _DamageAmount,
    CScript*    _SpawnedBy,
    bool        _UseSpawnExplosionCollisionForDamaging,
    bool        _PlayExplosionSound,
    const Vec2& _UpwardVelocity
)
{
    GameObject* Object = m_mapFlipbookEffectPoolers[FLIPBOOK_EFFECT_POOLER_TYPE::EXPLOSION_EFFECT_POOLER]->SpawnObject();
    if (Object)
    {
        Object->Transform()->SetRelativePosX(_SpawnPos.x);
        Object->Transform()->SetRelativePosY(_SpawnPos.y);
        Object->FlipbookRender()->Play(0, _FPS, 1);
        
        if (Ptr<CExplosion> Explosion = Object->GetScriptComponent<CExplosion>())
        {
            Explosion->ClearAlreadyDamaged();
            Explosion->SetDamage(_DamageAmount);
            Explosion->SetExplosionSize(_ExplosionSizeFactor);
            Explosion->SetSpawnedBy(_SpawnedBy);
            Explosion->SetUseCollisionForDamaging(_UseSpawnExplosionCollisionForDamaging);
            Explosion->SetUpwardVelocity(_UpwardVelocity);
        }

        if (_PlayExplosionSound)
        {
            Ptr<ASound> Sound = FIND_ASSET(ASound, L"Sound\\Explosion1.mp3");
            Sound->Play(1, 0.5f, true);
        }
    }
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
}
