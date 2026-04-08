#pragma once

class EAnimTransitionStrategy : public Entity
{
public:
    EAnimTransitionStrategy()                   = default;
    virtual ~EAnimTransitionStrategy() override = default;
    
public:
    
    virtual void UseWalkStateTransitionStrategy(class CEnemyAnimHandler* _AnimHandler) = 0;
    virtual void UseAttackStateTransitionStrategy(CEnemyAnimHandler* _AnimHandler) = 0;
    
};

// 얘는 사실 Clone할 필요 없긴 함
class CommonEnemyTransitionStrategy : public EAnimTransitionStrategy
{
private:

public:
    CommonEnemyTransitionStrategy() = default;
    virtual ~CommonEnemyTransitionStrategy() override = default;

public:
    
    virtual void UseWalkStateTransitionStrategy(CEnemyAnimHandler* _AnimHandler) override;
    virtual void UseAttackStateTransitionStrategy(CEnemyAnimHandler* _AnimHandler) override;
};

/// <summary>
/// 여기서 Clone 처리로 사용을 해야 함
/// </summary>
class RunnerTransitionStrategy : public EAnimTransitionStrategy
{
private:
    
    class CRunner*      m_Runner{};
    SIXTEEN_DIRECTION   m_PrevSixteenDirection{};

public:
    RunnerTransitionStrategy() = default;
    virtual ~RunnerTransitionStrategy() override = default;
    CLONE(RunnerTransitionStrategy)
    
    void SetRunner(CRunner* runner) { m_Runner = runner; }

public:
    
    virtual void UseWalkStateTransitionStrategy(CEnemyAnimHandler* _AnimHandler) override;
    virtual void UseAttackStateTransitionStrategy(CEnemyAnimHandler* _AnimHandler) override;
};

