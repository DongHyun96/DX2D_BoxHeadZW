#pragma once

class CGrenade : public CScript
{
private:
    
    static const float s_Gravity;
    static const float s_BounceDamping; // 바닥에 튕길 때, 잃는 에너지 ( 0 ~ 1)
    static const float s_GroundFriction; // 바닥에 튕길 때 x, y 축으로 감속되는 비율
    
private:

    Vec3 m_PrevLogicalPos{};
    Vec3 m_LogicalPos{}; // z를 높이값으로 사용
    Vec3 m_Velocity{};
    
    int m_BounceCount = 2; // 남은 튕김 횟수

private:

    // 터질 때, ExplosionDome의 Damage Amount
    float m_DamageAmount{};
    
public:
    CGrenade();
    virtual ~CGrenade() override;
    CLONE(CGrenade)

public:

    virtual void Begin() override;
    virtual void AfterLevelBegin() override;
    virtual void Tick() override;

public:

    void SetLogicalPos(const Vec3& _LogicalPos) { m_LogicalPos = _LogicalPos; }
    
    void SetDamageAmount(float _Damage) { m_DamageAmount = _Damage; }

    /// <summary>
    /// z값은 실질적인 가상의 높이 값이 된다
    /// </summary>
    /// <param name="_Velocity"></param>
    void SetFireVelocity(const Vec3& _Velocity) { m_Velocity = _Velocity; }

    /// <summary>
    /// 지면이나 벽면에 몇번 튀기고 터질지 체크
    /// </summary>
    void SetBounceTotalCount(UINT _Count) { m_BounceCount = _Count; }
    
private:
    
    void HandleOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
