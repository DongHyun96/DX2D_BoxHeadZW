#pragma once
#include "CStructure.h"
#include "TurretAttackStrategy.h"

enum class TURRET_STATE
{
    IDLE,
    ROTATE,
    ATTACK,
    END
};

enum class TURRET_TYPE
{
    MG,
    MORTAR,
    ROCKET
};

class CTurret : public CStructure
{
    
    friend class Turret_MGAttackStrategy;
    friend class Turret_MortarAttackStrategy;
    friend class Turret_RocketAttackStrategy;
    
private:

    TURRET_TYPE m_TurretType{};
    
private:
    
    TURRET_STATE m_CurrentTurretState{};
    
private:
    
    set<GameObject*> m_setAttackRangeEnteredEnemies{}; // 반경 안에 들어온 Enemy들 저장
    GameObject* m_TargetEnemyObject{};

private:

    // Flipbook stop과 stop 사이의 간격
    float m_RotateSpriteInterval = 0.2f;
    float m_RotateSpriteTimeChecker{};
    
private: // Attack 관련

    float m_AttackIntervalTotalTime = 0.5f;
    float m_AttackIntervalTimeChecker{};

private:
    
    static const map<TURRET_TYPE, Ptr<TurretAttackStrategy>> m_mapAttackStrategies;
    Ptr<TurretAttackStrategy> m_AttackStrategy{};
    
public:
    CTurret();
    CTurret(const CTurret& _Origin);
    virtual ~CTurret() override;
    CLONE(CTurret)

public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;

private: // 자식 GameObject의 AttackCollide callback 관련

    void AttackColliderBeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    void AttackColliderEndOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);

private:
    
    /// <summary>
    /// Rotate 도는 모션 끝난 뒤 호출 -> 아직 Target이 유효한지 아닌지에 따라 Idle 또는 Attack state로 전환 처리 
    /// </summary>
    void OnRotateAnimEnd();

    /// <summary>
    /// 실질적인 공격 처리
    /// </summary>
    /// <param name="_Target"></param>
    void Attack(GameObject* _Target);
    
private:
    
    /// <summary>
    /// TargetCharacter와의 방향 맞추기 처리   
    /// </summary>
    /// <returns> 해당 방향으로 맞추어 졌다면 return true </returns>
    bool HandleRotateToTarget();
    
public:
    
    /// <summary>
    /// 현재 재생 중인 Sprite가 바라보는 Angle return 
    /// </summary>
    float GetCurrentFacedAngle();
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};
