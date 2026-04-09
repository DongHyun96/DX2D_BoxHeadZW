#include "pch.h"
#include "CExplosion.h"

#include <algorithm>

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/ScriptMgr.h"

const Vec2 CExplosion::s_ScaleSizeBase = { 200.f, 200.f };

CExplosion::CExplosion()
    : CExplosionDome(SCRIPT_TYPE::EXPLOSION)
{
}

CExplosion::~CExplosion()
{
}

void CExplosion::Begin()
{
    CExplosionDome::Begin();
    GetOwner()->AddActivateDelegate(bind(&CExplosion::OnActivateOwnerObject, this, placeholders::_1));
    GetOwner()->SetIgnoreGlobalTimeScale(true); // Unaffected by global time scale.
}

void CExplosion::Tick()
{
    CExplosionDome::Tick();
    m_Age += DT;

    TickDamagePulse();
    TickSecondaryBursts();

    // Pseudo noise for a flickering explosion feel.
    const float noise1 = sinf(m_Age * m_NoiseFreq1 + m_NoisePhase1);
    const float noise2 = cosf(m_Age * m_NoiseFreq2 + m_NoisePhase2);
    const float pseudoNoise = (noise1 + noise2) * 0.5f;

    // Render offset (upward drift + horizontal sway).
    Vec2 renderOffset = GetRenderCom()->GetRenderOffset();
    renderOffset += m_UpwardVelocity * DT;
    renderOffset.x += pseudoNoise * 0.015f * DT;
    GetRenderCom()->SetRenderOffset(renderOffset);

    // Render scale (lifetime shrink + flicker).
    const float lifeRatio = m_Age / m_MaxLifeTime;
    float baseScale = 1.0f - (lifeRatio * lifeRatio);
    baseScale = max(baseScale, 0.f);

    const float flickerScale = 1.0f + (pseudoNoise * 0.15f);
    const float finalScaleX = baseScale * flickerScale;
    const float finalScaleY = baseScale * flickerScale * 1.2f;
    GetRenderCom()->SetRenderScale({ finalScaleX, finalScaleY });
}

void CExplosion::SetExplosionSize(float _Factor)
{
    Transform()->SetRelativeScaleXY(s_ScaleSizeBase * _Factor);
}

void CExplosion::ConfigureDamagePulse(float _DelaySec, float _DurationSec, int _StartSpriteIdx)
{
    m_DamagePulseDelay = max(0.f, _DelaySec);
    m_DamagePulseDuration = max(0.001f, _DurationSec);
    m_DamagePulseSpriteIdx = _StartSpriteIdx;
}

void CExplosion::SetSecondaryBurst
(
    int _Count,
    float _Radius,
    float _MinDelay,
    float _MaxDelay,
    float _DamageScale,
    float _SizeScale,
    bool _PlaySound
)
{
    m_SecondaryBurst.Count       = max(0, _Count);
    m_SecondaryBurst.Radius      = max(0.f, _Radius);
    m_SecondaryBurst.MinDelay    = max(0.f, _MinDelay);
    m_SecondaryBurst.MaxDelay    = max(m_SecondaryBurst.MinDelay, _MaxDelay);
    m_SecondaryBurst.DamageScale = max(0.f, _DamageScale);
    m_SecondaryBurst.SizeScale   = max(0.05f, _SizeScale);
    m_SecondaryBurst.bPlaySound  = _PlaySound;
}

void CExplosion::TickDamagePulse()
{
    if (!m_DamagePulseArmed || !m_UseCollisionForDamaging)
        return;

    if (!m_DamagePulseStarted)
    {
        bool shouldStart = (m_Age >= m_DamagePulseDelay);

        if (!shouldStart && m_DamagePulseSpriteIdx >= 0 && FlipbookRender())
            shouldStart = (FlipbookRender()->GetCurAnimatingSpriteIdx() >= m_DamagePulseSpriteIdx);

        if (shouldStart)
        {
            GetCollider2D()->SetActive(true);
            m_DamagePulseStarted = true;
            m_DamagePulseStartAge = m_Age;
        }
        return;
    }

    if (m_Age - m_DamagePulseStartAge >= m_DamagePulseDuration)
    {
        GetCollider2D()->SetActive(false);
        m_DamagePulseArmed = false;
    }
}

void CExplosion::TickSecondaryBursts()
{
    if (m_SecondaryBurst.Count <= 0)
        return;

    while (m_SecondaryBurstSpawnedCount < static_cast<int>(m_SecondarySpawnTimings.size()) &&
        m_Age >= m_SecondarySpawnTimings[m_SecondaryBurstSpawnedCount])
    {
        SpawnSecondaryBurst(m_SecondaryBurstSpawnedCount);
        ++m_SecondaryBurstSpawnedCount;
    }
}

void CExplosion::SpawnSecondaryBurst(int _BurstIndex) const
{
    if (!GM || m_SecondaryBurst.Count <= 0)
        return;

    const float angleStep = XM_2PI / static_cast<float>(m_SecondaryBurst.Count);
    const float angle = angleStep * _BurstIndex + GetRandom(-0.2f, 0.2f);

    Vec3 spawnPos = Transform()->GetWorldPos();
    spawnPos.x += cosf(angle) * m_SecondaryBurst.Radius;
    spawnPos.y += sinf(angle) * m_SecondaryBurst.Radius;
    spawnPos.z = spawnPos.y;

    const float mySizeFactor        = Transform()->GetRelativeScaleX() / s_ScaleSizeBase.x;
    const float secondarySizeFactor = max(0.05f, mySizeFactor * m_SecondaryBurst.SizeScale);
    const float secondaryDamage     = max(0.f, GetDamageAmount() * m_SecondaryBurst.DamageScale);

    ExplosionSpawnDesc Desc{};
    Desc.SpawnPos                   = spawnPos;
    Desc.ExplosionSizeFactor        = secondarySizeFactor;
    Desc.FPS                        = 1200.f;
    Desc.DamageAmount               = secondaryDamage;
    Desc.SpawnedBy                  = GetSpawnedBy();
    Desc.UseCollisionForDamaging    = true;
    Desc.PlayExplosionSound         = m_SecondaryBurst.bPlaySound;
    Desc.UpwardVelocity             = Vec2(GetRandom(-0.05f, 0.05f), GetRandom(0.2f, 0.45f));
    GM->SpawnExplosion(Desc);
}

void CExplosion::OnActivateOwnerObject(const Ptr<GameObject>& _OwnerObject)
{
    (void)_OwnerObject;

    GetRenderCom()->SetRenderOffset({ 0.f, 0.f });
    GetRenderCom()->SetRenderScale({ 1.f, 1.f });
    GetCollider2D()->SetActive(false);

    m_Age = 0.f;
    m_NoisePhase1 = GetRandom(0.f, XM_2PI);
    m_NoisePhase2 = GetRandom(0.f, XM_2PI);
    m_NoiseFreq1 = GetRandom(24.f, 36.f);
    m_NoiseFreq2 = GetRandom(40.f, 54.f);

    m_DamagePulseArmed = m_UseCollisionForDamaging;
    m_DamagePulseStarted = false;
    m_DamagePulseStartAge = 0.f;

    m_SecondaryBurstSpawnedCount = 0;
    m_SecondarySpawnTimings.clear();
    if (m_SecondaryBurst.Count > 0)
    {
        m_SecondarySpawnTimings.reserve(m_SecondaryBurst.Count);
        for (int i = 0; i < m_SecondaryBurst.Count; ++i)
            m_SecondarySpawnTimings.push_back(GetRandom(m_SecondaryBurst.MinDelay, m_SecondaryBurst.MaxDelay));

        sort(m_SecondarySpawnTimings.begin(), m_SecondarySpawnTimings.end());
    }
}
