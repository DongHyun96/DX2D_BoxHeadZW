#pragma once

class TurretAttackStrategy : public Entity
{
public:
    
    TurretAttackStrategy() = default;
    virtual ~TurretAttackStrategy() override = default;
    virtual TurretAttackStrategy* Clone() const = 0;
    
public:

    virtual void WaitAttack() {}
    
    /// <summary>
    /// Target에게 Attack 시도 
    /// </summary>
    /// <param name="_Turret"> : 공격시도하려는 Turret </param>
    /// <param name="_Target"> : 공격 대상 Enemy </param>
    /// <returns> : 공격이 정상적으로 끝났으면 return true / 공격이 아직 끝나지 않았으면 return true(점사 처리) </returns>
    virtual bool UseAttackStrategy(class CTurret* _Turret, GameObject* _Target) = 0;
    
};

class Turret_MGAttackStrategy : public TurretAttackStrategy
{
private:

    static float s_BurstInterval;
    float        m_BurstFireTimer = s_BurstInterval; // 바로 사격 가능하도록 처리
    int          m_FireCount{}; // 3발 사격이 끝났는지 체크하기 위함
    
public:
    
    Turret_MGAttackStrategy() = default;
    virtual ~Turret_MGAttackStrategy() override = default;
    CLONE(Turret_MGAttackStrategy)

private:

    virtual void WaitAttack() override;
    
    /// <summary>
    /// 3발 점사로 사격을 진행함
    /// </summary>
    virtual bool UseAttackStrategy(CTurret* _Turret, GameObject* _Target) override;
    
};

class Turret_MortarAttackStrategy : public TurretAttackStrategy
{
private:

public:
    Turret_MortarAttackStrategy() = default;
    virtual ~Turret_MortarAttackStrategy() override = default;
    CLONE(Turret_MortarAttackStrategy)
private:
    
    virtual bool UseAttackStrategy(CTurret* _Turret, GameObject* _Target) override;
};

class Turret_RocketAttackStrategy : public TurretAttackStrategy
{
private:

public:
    Turret_RocketAttackStrategy() = default;
    virtual ~Turret_RocketAttackStrategy() override = default;
    CLONE(Turret_RocketAttackStrategy)
    
private:
    
    virtual bool UseAttackStrategy(CTurret* __Turret, GameObject* _Target) override;
    
};


