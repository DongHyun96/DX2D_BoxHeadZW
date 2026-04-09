#include "pch.h"
#include "CFirePillarHandler.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/ScriptMgr.h"

CFirePillarHandler::CFirePillarHandler()
    : CScript(SCRIPT_TYPE::FIREPILLARHANDLER)
{
}

CFirePillarHandler::~CFirePillarHandler()
{
}

void CFirePillarHandler::Begin()
{
    m_vecPendingFirePillars.clear();
    GM->SetFirePillarHandler(this);
}

void CFirePillarHandler::Tick()
{
    for (int i = static_cast<int>(m_vecPendingFirePillars.size()) - 1; i >= 0; --i)
    {
        FPendingFirePillar& Pending = m_vecPendingFirePillars[i];
        Pending.TimeUntilNextSpawn -= DT;

        while (Pending.SpawnedStepCount < Pending.Desc.StepCount && Pending.TimeUntilNextSpawn <= 0.f)
        {
            SpawnStepExplosion(Pending.Desc, Pending.SpawnedStepCount);
            ++Pending.SpawnedStepCount;
            Pending.TimeUntilNextSpawn += Pending.Desc.StepIntervalSec;
        }

        if (Pending.SpawnedStepCount >= Pending.Desc.StepCount)
            m_vecPendingFirePillars.erase(m_vecPendingFirePillars.begin() + i);
    }
}

void CFirePillarHandler::SpawnFirePillar(const FirePillarSpawnDesc& _Desc)
{
    if (!GM->GetFlipbookEffectPooler(FLIPBOOK_EFFECT_POOLER_TYPE::EXPLOSION_EFFECT_POOLER))
        return;

    FPendingFirePillar Pending{};
    Pending.Desc                        = _Desc;
    Pending.Desc.StepCount              = max(1, Pending.Desc.StepCount);
    Pending.Desc.StepIntervalSec        = max(0.001f, Pending.Desc.StepIntervalSec);
    Pending.Desc.StepHeight             = max(0.f, Pending.Desc.StepHeight);
    Pending.Desc.BaseExplosionSizeScale = max(0.05f, Pending.Desc.BaseExplosionSizeScale);
    Pending.Desc.TopExplosionSizeScale  = max(0.05f, Pending.Desc.TopExplosionSizeScale);
    Pending.Desc.HorizontalJitter       = max(0.f, Pending.Desc.HorizontalJitter);
    Pending.Desc.VerticalJitter         = max(0.f, Pending.Desc.VerticalJitter);
    Pending.TimeUntilNextSpawn          = 0.f;
    m_vecPendingFirePillars.push_back(Pending);
}

void CFirePillarHandler::SpawnFirePillar(const Vec3& _SpawnPos, float _DamageAmount, CScript* _SpawnedBy)
{
    FirePillarSpawnDesc Desc{};
    Desc.SpawnPos                        = _SpawnPos;
    Desc.StepCount                       = 7;
    Desc.StepIntervalSec                 = 0.025f;
    Desc.StepHeight                      = 24.f;
    Desc.BaseExplosionSizeScale          = 0.72f;
    Desc.TopExplosionSizeScale           = 0.34f;
    Desc.HorizontalJitter                = 2.0f;
    Desc.VerticalJitter                  = 2.0f;
    Desc.ExplosionTemplate.ExplosionSizeFactor = 0.9f;
    Desc.ExplosionTemplate.DamageAmount  = _DamageAmount;
    Desc.ExplosionTemplate.SpawnedBy     = _SpawnedBy;
    Desc.ExplosionTemplate.SecondaryBurstCount = 0;
    Desc.ExplosionTemplate.UpwardVelocity = Vec2(0.f, 1.0f);
    SpawnFirePillar(Desc);
}

void CFirePillarHandler::SpawnStepExplosion(const FirePillarSpawnDesc& _Desc, int _StepIndex) const
{
    if (!GM->GetFlipbookEffectPooler(FLIPBOOK_EFFECT_POOLER_TYPE::EXPLOSION_EFFECT_POOLER))
        return;
    
    ExplosionSpawnDesc Desc = _Desc.ExplosionTemplate;

    const float Progress = (_Desc.StepCount <= 1)
                               ? 0.f
                               : static_cast<float>(_StepIndex) / static_cast<float>(_Desc.StepCount - 1);
    const float StepScale = _Desc.BaseExplosionSizeScale + ((_Desc.TopExplosionSizeScale - _Desc.BaseExplosionSizeScale) * Progress);

    Desc.SpawnPos            = _Desc.SpawnPos;
    Desc.SpawnPos.x          += GetRandom(-_Desc.HorizontalJitter, _Desc.HorizontalJitter);
    Desc.SpawnPos.y          += (_Desc.StepHeight * static_cast<float>(_StepIndex));
    Desc.SpawnPos.y          += GetRandom(-_Desc.VerticalJitter, _Desc.VerticalJitter);
    Desc.SpawnPos.z          = Desc.SpawnPos.y;
    Desc.ExplosionSizeFactor *= StepScale;

    if (!_Desc.PlaySoundEachStep && _StepIndex > 0)
        Desc.PlayExplosionSound = false;

    GM->SpawnExplosion(Desc);
}
