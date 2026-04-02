#pragma once

class CRocketProjectile : public CScript
{
private:

    float   m_Speed{}; // ScriptParam Edited
    Vec3    m_Direction{};
    float   m_Damage{};
    
private:
    
    static const float s_SMOKE_SPAWN_INTERVAL;
    float m_SmokeSpawnTime = s_SMOKE_SPAWN_INTERVAL;
    
public:

    CRocketProjectile();
    virtual ~CRocketProjectile() override;
    CLONE(CRocketProjectile)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void AfterLevelBegin() override;
    virtual void Tick() override;

public:
    
    void SetDirection(const Vec2& _Direction) { m_Direction = ToVec3(_Direction); }
    void SetDamage(float _Damage) { m_Damage = _Damage; }
    
private:
    
    void BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
