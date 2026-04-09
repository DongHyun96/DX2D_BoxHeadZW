#pragma once
#include "CExplosionDome.h"
#include <vector>

class CExplosion : public CExplosionDome
{
private:
    
    static const Vec2 s_ScaleSizeBase;

    struct FSecondaryBurst
    {
        int Count           = 0;
        float Radius        = 90.f;
        float MinDelay      = 0.08f;
        float MaxDelay      = 0.20f;
        float DamageScale   = 0.6f;
        float SizeScale     = 0.55f;
        bool bPlaySound     = false;
    };
    
private:
    
    Vec2 m_UpwardVelocity{};
    bool m_UseCollisionForDamaging = true; // Collision 검사로 들어온 Character들에게 Damage를 줄건지
    float m_NoisePhase1{};
    float m_NoisePhase2{};
    float m_NoiseFreq1 = 30.f;
    float m_NoiseFreq2 = 46.f;
    bool m_DamagePulseArmed{};
    bool m_DamagePulseStarted{};
    float m_DamagePulseDelay = 0.06f;
    float m_DamagePulseDuration = 0.04f;
    float m_DamagePulseStartAge{};
    int m_DamagePulseSpriteIdx = 2;
    
    FSecondaryBurst m_SecondaryBurst{};
    vector<float> m_SecondarySpawnTimings{};
    int m_SecondaryBurstSpawnedCount{};
    
private:

    // 누적 시간 측정
    float m_Age{};
    const float m_MaxLifeTime = 3.f;

public:
    CExplosion();
    virtual ~CExplosion() override;
    CLONE(CExplosion)

public:
    
    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    virtual void SetExplosionSize(float _Factor) override;
    void ConfigureDamagePulse(float _DelaySec, float _DurationSec, int _StartSpriteIdx = -1);
    void SetSecondaryBurst
    (
        int _Count,
        float _Radius = 90.f,
        float _MinDelay = 0.08f,
        float _MaxDelay = 0.20f,
        float _DamageScale = 0.6f,
        float _SizeScale = 0.55f,
        bool _PlaySound = false
    );
    void SetUseCollisionForDamaging(bool _UseCollisionForDamaging) { m_UseCollisionForDamaging = _UseCollisionForDamaging; }
    void SetUpwardVelocity(const Vec2& _UpwardVelocity) { m_UpwardVelocity = _UpwardVelocity; }
    
private:
    
    void TickDamagePulse();
    void TickSecondaryBursts();
    void SpawnSecondaryBurst(int _BurstIndex) const;
    void OnActivateOwnerObject(const Ptr<GameObject>& _OwnerObject);
    
};
