#pragma once

class TurretAttackStrategy : public Entity
{
public:
    
    TurretAttackStrategy() = default;
    virtual ~TurretAttackStrategy() override = default;
    
public:
    
    virtual void UseAttackStrategy(GameObject* _Target) = 0;
    
};

class Turret_MGAttackStrategy : public TurretAttackStrategy
{
private:
    
public:
    
    Turret_MGAttackStrategy() = default;
    virtual ~Turret_MGAttackStrategy() override = default;

private:
    
    virtual void UseAttackStrategy(GameObject* _Target) override;
    
};

class Turret_MortarAttackStrategy : public TurretAttackStrategy
{
private:

public:
    Turret_MortarAttackStrategy() = default;
    virtual ~Turret_MortarAttackStrategy() override = default;
    
private:
    
    virtual void UseAttackStrategy(GameObject* _Target) override;
};

class Turret_RocketAttackStrategy : public TurretAttackStrategy
{
private:

public:
    Turret_RocketAttackStrategy() = default;
    virtual ~Turret_RocketAttackStrategy() override = default;
    
private:
    
    virtual void UseAttackStrategy(GameObject* _Target) override;
    
};


