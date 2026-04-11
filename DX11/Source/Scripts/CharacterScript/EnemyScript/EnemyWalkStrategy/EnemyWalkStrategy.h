#pragma once

class EnemyWalkStrategy : public Entity
{
private:

public:
    EnemyWalkStrategy() = default;
    virtual ~EnemyWalkStrategy() override = default;
    // virtual EnemyWalkStrategy* Clone() const = 0;

public:

    virtual void UseWalkStrategy(class CEnemyScript* _Enemy) = 0;

public:
    
    /// <summary>
    /// 전체 Target으로 삼을 수 있는 오브젝트를 탐색하여 찾기 
    /// </summary>
    static GameObject* FindNearestTargetFromAllObjects(CEnemyScript* _Enemy);
    
    static GameObject* GetRandomTargetFromAllObjects(CEnemyScript* _Enemy);
    
};


class EnemyWalkThroughCellPathStrategy : public EnemyWalkStrategy
{
private:
    
public:
    
    EnemyWalkThroughCellPathStrategy() = default;
    virtual ~EnemyWalkThroughCellPathStrategy() override  = default;
    
private:
    
    virtual void UseWalkStrategy(CEnemyScript* _Enemy) override;
    
};

class EnemyWalkStraightStrategy : public EnemyWalkStrategy
{
private:
    
public:
    
    EnemyWalkStraightStrategy() = default;
    virtual ~EnemyWalkStraightStrategy() override = default;
    
private:
    
    virtual void UseWalkStrategy(CEnemyScript* _Enemy) override;
    
};

/// <summary>
/// 첫 스폰 처리되고, Valid한 Cell까지 이동해야할 때, 이 WalkStrategy를 사용한다
/// </summary>
class EnemyFirstSpawnWalkStrategy : public EnemyWalkStrategy
{
private:
    
public:
    
    EnemyFirstSpawnWalkStrategy() = default;
    virtual ~EnemyFirstSpawnWalkStrategy() override = default;
    
private:

    virtual void UseWalkStrategy(CEnemyScript* _Enemy) override;
    
};