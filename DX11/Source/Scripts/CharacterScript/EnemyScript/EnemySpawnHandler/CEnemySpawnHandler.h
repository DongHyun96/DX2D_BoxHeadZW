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
    
    /*void AddEnemyPooler(ENEMY_TYPE _EnemyType, CPoolComponent* _EnemyPooler) { m_mapEnemyPoolers[_EnemyType] = _EnemyPooler; }
    CPoolComponent* GetEnemyPooler(ENEMY_TYPE _EnemyType) const;*/

    /// <summary>
    /// 해당 EnemyType 해당 위치에 Spawn 시키기 (Cell에 맞추어 Spawn처리됨)
    /// </summary>
    /// <returns> 제대로 Spawn되었다면 Spawn된 Enemy GameObject 반환 </returns>
    GameObject* SpawnEnemy(ENEMY_TYPE _EnemyType, const Vec2& _SpawnPos);
    GameObject* SpawnEnemy(ENEMY_TYPE _EnemyType, const CellCoord& _CellCoord);
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
