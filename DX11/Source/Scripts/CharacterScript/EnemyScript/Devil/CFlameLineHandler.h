#pragma once

/// <summary>
/// Devil에 붙어있는 자식 오브젝트여야 함
/// </summary>
class CFlameLineHandler : public CScript
{
private:

    int m_PlayerDamagedCount{}; // 한 공격 내에서 공격 횟수가 3회 이상 넘어가지 않도록 처리를 한다 (3회여도 뒤지긴 함)
    
private:

    float m_Damage = 80.f;
    float m_AttackColliderScaleTimer{};
    
    float m_PillarIntervalTimer{};
    
    
public:
    
    CFlameLineHandler();
    virtual ~CFlameLineHandler() override;
    CLONE(CFlameLineHandler);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

public:


    void InitSpawn();
    
    // 공격 대기 시간으로 전환 시, 초기화할 변수 처리
    void InitWaitState();
    
public:
    
    /// <summary>
    /// Flame line 생성
    /// </summary>
    void MakeFlameLine(float _Angle, float _Damage);

private:
    
    void OnAttackColliderBeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
    
};
