#pragma once
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

class CVampire : public CEnemyScript
{
private:

    // 마지막 공격 이후, 자가치유 시작할 수 있는 시간
    static const float s_SelfHealStartTime;
    static const float s_HealPerSec; // 초당 자가치유량
    
private:
    
    float m_AfterLastDamagedTimer{}; // 마지막 피격 당한 이후 흐른 시간
    
    class CStatScript* m_Stat{};
    
public:
    CVampire();
    virtual ~CVampire() override;
    CLONE(CVampire)

public:
    
    virtual void Begin() override;
    virtual void Tick() override;
    
private:
    
    virtual void OnTakeDamage() override;
    
private:
    
    void HandleSelfHeal();
};
