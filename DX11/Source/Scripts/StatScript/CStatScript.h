#pragma once

/// <summary>
/// Stat Script - Character의 Stat이 될 수도 있고, 설치물의 Stat이 될 수도 있는 부모 클래스
/// </summary>
class CStatScript : public CScript
{
private:

    float   m_HPMax{};
    float   m_HP{};
    
public:
    
    CStatScript();
    
protected:
    
    CStatScript(enum SCRIPT_TYPE _ScriptType);

public:
    
    virtual ~CStatScript() override;
    CLONE(CStatScript)

public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;

public:
    
    /// <summary>
    /// 스폰 시, 초기화 처리해야할 목록들
    /// </summary>
    virtual void InitSpawn();
    
public:
    
    /// <summary>
    /// Damage 입히기 시도
    /// </summary>
    /// <param name="_DamageAmount"> : Damage 총량 </param>
    /// <param name="_DamageCauser"> : Damage를 준 Causer Object</param>
    /// <returns> : 제대로 Damage 처리가 되었다면 return true </returns>
    virtual bool TakeDamage(float _DamageAmount, GameObject* _DamageCauser);
    
    virtual bool ApplyHeal(float _HealAmount);

public:
    
    float GetHP() const { return m_HP; }
    bool IsFullHP() const { return m_HP >= m_HPMax; }
    bool IsDead() const { return m_HP <= 0.f; }
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
