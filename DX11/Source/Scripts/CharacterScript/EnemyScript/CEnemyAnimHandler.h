#pragma once

class CEnemyAnimHandler : public CScript
{
private:
    
    int m_AnimFPSTemp = 12;
    
private:

    class CEnemyScript* m_MainEnemyScript{};
    EDIRECTION m_AnimDirection = EDIRECTION::DOWN;
    
private:
    
    ENEMY_MAINSTATE m_PrevMainState = ENEMY_MAINSTATE::END;
    EDIRECTION m_PrevAnimDirection = EDIRECTION::END;
    
public:

    CEnemyAnimHandler();
    virtual ~CEnemyAnimHandler() override;
    CLONE(CEnemyAnimHandler)
    
public:

    virtual void Init() override;    
    virtual void Begin() override;
    virtual void Tick() override;

private:
    
    void UpdateAnimDirection();
    void UpdateAnimTransition();

    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
