#pragma once
#include "CStructure.h"

enum class TURRET_STATE
{
    IDLE,
    ROTATE,
    ATTACK,
    END
};

class CTurret : public CStructure
{
private:
    
    TURRET_STATE m_CurrentTurretState{};
    TURRET_STATE m_PrevTurretState{}; // 이전 Tick에서의 TurretState
    
private:
    
    set<GameObject*> m_setAttackRangeEnteredEnemies{}; // 반경 안에 들어온 Enemy들 저장
    GameObject* m_TargetEnemyObject{};
    
public:
    CTurret();
    virtual ~CTurret() override;
    CLONE(CTurret)

public:
    
    virtual void Begin() override;
    virtual void Tick() override;    
    
private: // 자식 GameObject의 AttackCollide callback 관련

    void AttackColliderBeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    void AttackColliderEndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);    
};
