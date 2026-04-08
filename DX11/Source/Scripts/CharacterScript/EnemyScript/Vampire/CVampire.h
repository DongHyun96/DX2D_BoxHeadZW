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
    
private:
    
    float m_TeleportStartTimeLimit{};
    
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

    /// <summary>
    /// 주기적으로 Player의 근처에 Teleport할 수 있는 위치에 순간이동한다
    /// 순간이동 처리를 한 뒤에는 Walk 상태(이미 되어있긴한데)로 되돌리고, TargetObject도 재설정 처리를 위해 nullptr로 비워두기 
    /// </summary>
    void HandleTeleport();
};
