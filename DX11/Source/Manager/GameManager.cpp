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
