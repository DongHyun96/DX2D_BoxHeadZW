#include "pch.h"
#include "GameManager.h"

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
    m_mapEffectPoolers.clear();
    
    m_RocketProjectilePooler = nullptr;
}

void GameManager::OnLevelChanged(ALevel* _PrevLevel, ALevel* _NextLevel)
{
    // TODO : 특정 Level에서 다른 Level 로 넘어갈 때 처리할 것 처리하기
}

CPoolComponent* GameManager::GetEffectPooler(EFFECT_POOLER_TYPE _PoolerType) const
{
    if (!m_mapEffectPoolers.contains(_PoolerType)) return nullptr;
    return m_mapEffectPoolers.at(_PoolerType);
}

CPoolComponent* GameManager::GetEnemyPooler(ENEMY_TYPE _EnemyType) const
{
    if (!m_mapEnemyPoolers.contains(_EnemyType)) return nullptr;
    return m_mapEnemyPoolers.at(_EnemyType);
}
