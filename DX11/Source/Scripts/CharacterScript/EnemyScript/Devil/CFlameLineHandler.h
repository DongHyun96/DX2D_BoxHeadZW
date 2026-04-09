#pragma once

/// <summary>
/// Devil에 붙어있는 자식 오브젝트여야 함
/// </summary>
class CFlameLineHandler : public CScript
{
private:

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
