#pragma once

#include <unordered_set>

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
    
    // 피격 반경 감지기 오브젝트
    GameObject* m_AttackRangeDetector{};
    
    // Straight 이동 감지기 오브젝트
    GameObject* m_MoveStraightDetector{};

    // 피격 판정이 이번 공격에 들어갔는지 체킹
    unordered_set<CCollider2D*> m_AlreadyDamaged{};

private:

    unordered_set<GameObject*> m_setStraightThroughDetectionEnteredObjects{}; // 현재 StraightThrough 영역에 들어와 있는 오브젝트들
    unordered_set<GameObject*> m_setAttackAreaEnteredObjects{};               // 현재 Attack 반경에 들어와 있는 오브젝트들

private:
    
    class CEnemyScript* m_MainEnemyScript{};
    
public:
    CPerceptionHandler();
    virtual ~CPerceptionHandler() override;
    CLONE(CPerceptionHandler);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

public:
    
    GameObject* GetFirstAttackAreaObject() const;
    GameObject* GetNearestStraightThroughDetectionEnteredObject() const;
    
    bool IsStraightThroughDetectionSetContainObject(GameObject* _Object) const { return m_setStraightThroughDetectionEnteredObjects.contains(_Object); }

    /// <summary>
    /// Spawn시의 초기값으로 되돌리기
    /// </summary>
    void InitSpawn();
    
    /// <summary>
    /// DamagingCollider 켜고 끄기
    /// </summary>
    /// <param name="_Enabled"></param>
    /// <param name="_RotationAngle"> : Spawn시킬 회전 방향 </param>
    void ToggleDamagingCollider(bool _Enabled, float _RotationAngle = 0.f);
    
    GameObject* GetDamageColliderObject() const { return m_AttackColliderObject; }

private:

    GameObject* ResolvePerceptionTarget(CCollider2D* _OtherCollider) const;
    
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

    /// <summary>
    /// Attack 피격 범위 Collider overlap handling 
    /// </summary>
    void OnAttackDamageColliderBeginOverlap(CCollider2D* _DamageCollider, CCollider2D* _OtherCollider);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
};
