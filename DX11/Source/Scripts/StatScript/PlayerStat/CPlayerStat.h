#pragma once
#include "Source/Scripts/StatScript/CStatScript.h"

class CPlayerStat : public CStatScript
{
private:

    const float m_BoostMax = 100.f;
    float m_Boost{};
    
public:
    
    CPlayerStat();
    virtual ~CPlayerStat() override;
    CLONE(CPlayerStat)
    
public:

    virtual void Begin() override;
    virtual void Tick() override;
    
public:

    virtual bool TakeDamage(float _DamageAmount, const Vec2& _DamageSourcePos) override;
    virtual bool ApplyBoost(float _BoostAmount);
    
public:

    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
