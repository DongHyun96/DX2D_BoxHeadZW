#pragma once

/// <summary>
/// Enemy AI의 각 Detection Area detecting에 따른 State 전환 및 기본 처리 담당
/// Devil을 뺀 모든 Enemy에 대한 기본 Perception 처리 담당
/// & 추가로 클래스명이 PerceptionHandler이긴 한데, AttackCollider, AttackAreaDetectionCollider, StraightThroughCollider 모두 이 클래스에서 담당처리
/// </summary>
class CPerceptionHandler : public CScript
{
private:

    // 실질적인 피격 처리 Collider를 들고 있는 게임 오브젝트
    GameObject* m_AttackColliderObject{};

private:

    set<GameObject*> m_setStraightThroughDetectionEnteredObjects{}; // 현재 StraightThrough 영역에 들어와 있는 오브젝트들
    set<GameObject*> m_setAttackAreaEnteredObjects{};               // 현재 Attack 반경에 들어와 있는 오브젝트들

private:
    
    class CEnemyScript* m_MainEnemyScript{};
    
public:
    CPerceptionHandler();
    virtual ~CPerceptionHandler() override;
    CLONE(CPerceptionHandler);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

private:
    
    /// <summary>
    /// 가장 외곽의 Collider Overlap handling
    /// </summary>
    void OnStraightThroughColliderBeginOverlap(CCollider2D* _StraightThroughCollider, CCollider2D* _OtherCollider);
    void OnStraightThroughColliderEndOverlap(CCollider2D* _StraightThroughCollider, CCollider2D* _OtherCollider);

    /// <summary>
    /// Attack 반경 Collider overlap handling
    /// </summary>
    void OnAttackAreaColliderBeginOverlap(CCollider2D* _AttackAreaCollider, CCollider2D* _OtherCollider);
    void OnAttackAreaColliderEndOverlap(CCollider2D* _AttackAreaCollider, CCollider2D* _OtherCollider);
    
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
};
