#pragma once

class CEnemySpawnHandler : public CScript
{
private:

    map<ENEMY_TYPE, CPoolComponent*> m_mapEnemyPoolers{};
    
public:
    
    CEnemySpawnHandler();
    virtual ~CEnemySpawnHandler() override;
    CLONE(CEnemySpawnHandler);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

public:
    
    /// <summary>
    /// 라운드 시작 시, 맵 밖에 스폰 처리  
    /// </summary>
    /// <param name="_EnemyType"></param>
    /// <param name="_SpawnLoc"> : 첫 스폰 지점 영역(해당 영역에서 랜덤한 위치에 스폰 처리할 것임)</param>
    /// <returns> : 제대로 Spawn되었다면 Spawn된 Enemy GameObject 반환 </returns>
    GameObject* SpawnEnemyOnFirstSpawnArea(ENEMY_TYPE _EnemyType, FIRST_SPAWN_LOC _SpawnLoc);
    
    /*void AddEnemyPooler(ENEMY_TYPE _EnemyType, CPoolComponent* _EnemyPooler) { m_mapEnemyPoolers[_EnemyType] = _EnemyPooler; }
    CPoolComponent* GetEnemyPooler(ENEMY_TYPE _EnemyType) const;*/

    /// <summary>
    /// 해당 EnemyType 해당 위치에 Spawn 시키기 (Cell에 맞추어 Spawn처리됨)
    /// </summary>
    /// <returns> 제대로 Spawn되었다면 Spawn된 Enemy GameObject 반환 </returns>
    GameObject* SpawnEnemyOnAvailableCell(ENEMY_TYPE _EnemyType, const Vec2& _SpawnPos);
    GameObject* SpawnEnemyOnAvailableCell(ENEMY_TYPE _EnemyType, const CellCoord& _CellCoord);
    
private:
    
    void TryInitSpawnedEnemy(GameObject* EnemyObject);
    void TryInitSpawnedEnemy(class CEnemyScript* _EnemyScript);
    
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
