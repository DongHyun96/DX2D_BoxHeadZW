#pragma once

/// <summary>
/// Damage 처리할 것 (한 번 Damage를 준 오브젝트는 중복해서 Damage 처리를 하면 안됨)
/// </summary>
class CExplosionDome : public CScript
{
private:

    float m_DamageAmount{}; // 이 ExplosionDome의 Damage량
    
    // 이미 Damage를 입힌 CCollider의 경우, 넘어가야 함
    set<CCollider2D*> m_setAlreadyDamaged{};
    
public:

    CExplosionDome();
    virtual ~CExplosionDome() override;
    CLONE(CExplosionDome)
        
public:

    virtual  void Begin() override;
    virtual  void Tick() override;

public:
    
    void ClearAlreadyDamaged() { m_setAlreadyDamaged.clear(); }
    void SetDamage(float _DamageAmount) { m_DamageAmount = _DamageAmount; }

private:
    
    void BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
    
};
