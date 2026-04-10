#pragma once

#include "EAnimTransitionStrategy.h"
#include "Source/Scripts/CharacterScript/AnimHandler/CCharacterAnimHandler.h"

enum class ENEMY_ANIMTRANS_TYPE
{
    COMMON,
    RUNNER
};

class CEnemyAnimHandler : public CCharacterAnimHandler
{
    friend class CommonEnemyTransitionStrategy;
    friend class RunnerTransitionStrategy;
    
private:

    static map<ENEMY_ANIMTRANS_TYPE, Ptr<EAnimTransitionStrategy>> s_mapAnimTransitionStrategies;
    Ptr<EAnimTransitionStrategy> m_TransitionStrategy{};
    
private:

    class CEnemyScript* m_MainEnemyScript{};
    
private:
    
    ENEMY_MAINSTATE m_PrevMainState = ENEMY_MAINSTATE::END;
    
private:
    
    float m_AttackAnimFPS = 8.f;
    
public:

    CEnemyAnimHandler();
    virtual ~CEnemyAnimHandler() override;
    CLONE(CEnemyAnimHandler)
    
public:

    virtual void Init() override;    
    virtual void Begin() override;

private:
    
    virtual void UpdateAnimTransition() override;

public:
    
    ENEMY_MAINSTATE GetPrevMainState() const { return m_PrevMainState; }
    
    void SetAttackAnimFPS(float _FPS) { m_AttackAnimFPS = _FPS; }
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
