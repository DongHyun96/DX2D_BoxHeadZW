#pragma once

class CRocketProjectile : public CScript
{
private:

    float   m_Speed{}; // ScriptParam Edited
    Vec3    m_Direction{};
    
private: // Life Time 관련 TODO : 이거 쓰지 말고, 실질적인 MapBoundary 잡히면 해당 Boundary 넘어갔을 때 Active false 처리 해줄 것 

    static const float s_MAX_LIFETIME; 
    float m_LifeTime{}; // 
    
    float m_Damage{};
    
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
