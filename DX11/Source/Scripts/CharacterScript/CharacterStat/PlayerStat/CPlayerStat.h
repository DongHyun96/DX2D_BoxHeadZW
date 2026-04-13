#pragma once
#include "Source/Scripts/CharacterScript/CharacterStat/CCharacterStat.h"
#include <vector>

class CPlayerStat : public CCharacterStat
{
private:
    
    bool m_bDebugInvincible = true;
    
private:

    const float m_BoostMax = 100.f;
    float m_Boost{};
    
    // 무적 관련
    bool m_IsInvincible = false;
    float m_InvincibleTimer = 0.f;
    const float m_InvincibleDuration = 3.f;
    
    std::vector<float> m_HitHistory; // 최근 1초 내의 피격 기록 (Timestamp)
    float m_FlickerTimer = 0.f;

public:
    
    CPlayerStat();
    virtual ~CPlayerStat() override;
    CLONE(CPlayerStat)
    
public:

    virtual void Begin() override;
    virtual void Tick() override;
    
public:

    virtual bool TakeDamage(float _DamageAmount, GameObject* _DamageCauser) override;
    virtual bool ApplyHeal(float _HealAmount) override;
    virtual bool ApplyBoost(float _BoostAmount);

    void ToggleDebugInvincible() { m_bDebugInvincible = !m_bDebugInvincible; }
    bool IsDebugInvincible() const { return m_bDebugInvincible; }
    
public:
    
    bool GetIsInvincible() const { return m_IsInvincible; }
    
public:

    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
private:

    void UpdateInvincibility();
    
};
