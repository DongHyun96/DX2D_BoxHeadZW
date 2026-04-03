#include "pch.h"
#include "GameManager.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/Scripts/ExplosionDome/CExplosionDome.h"

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

void GameManager::OnLevelBegin()
{
}

void GameManager::OnLevelPlayToStop()
{
    // m_PlayerObject = nullptr;
    m_MainPlayerScript = nullptr;
    
    m_mapEnemyPoolers.clear();
    m_mapFlipbookEffectPoolers.clear();
    
    m_RocketProjectilePooler = nullptr;
    m_BackgroundCellManager = nullptr;
}

void GameManager::OnLevelChanged(ALevel* _PrevLevel, ALevel* _NextLevel)
{
    // TODO : 특정 Level에서 다른 Level 로 넘어갈 때 처리할 것 처리하기
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

void GameManager::SpawnRocketSmoke(Vec3 _SpawnPos)
{
    GameObject* Object = m_mapFlipbookEffectPoolers[FLIPBOOK_EFFECT_POOLER_TYPE::ROCKET_SMOKE_POOLER]->SpawnObject();
    if (Object)
    {
        Object->Transform()->SetRelativePosX(_SpawnPos.x);
        Object->Transform()->SetRelativePosY(_SpawnPos.y);
        Object->FlipbookRender()->Play(0, 20.f, 1);
    }
}

void GameManager::SpawnExplosionDome(Vec3 _SpawnPos, float _FPS, float _DamageAmount)
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
            
        }
        
        Ptr<ASound> Sound = FIND_ASSET(ASound, L"Sound\\Explosion1.mp3");
        Sound->Play(1, 0.5f, true);
    }
}
