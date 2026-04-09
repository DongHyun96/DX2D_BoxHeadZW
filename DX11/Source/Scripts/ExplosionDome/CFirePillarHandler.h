#pragma once

class CFirePillarHandler : public CScript
{
private:
    struct FPendingFirePillar
    {
        FirePillarSpawnDesc Desc{};
        int SpawnedStepCount{};
        float TimeUntilNextSpawn{};
    };
    
private:
    
    vector<FPendingFirePillar> m_vecPendingFirePillars{};
    
public:
    
    CFirePillarHandler();
    virtual ~CFirePillarHandler() override;
    CLONE(CFirePillarHandler);
    
public:
    
    virtual void Begin() override;
    void Tick() override;
    
    
public:
    
    void SpawnFirePillar(const FirePillarSpawnDesc& _Desc);
    void SpawnFirePillar(const Vec3& _SpawnPos, float _DamageAmount = 75.f, CScript* _SpawnedBy = nullptr);
    
private:
    
    void SpawnStepExplosion(const FirePillarSpawnDesc& _Desc, int _StepIndex) const;
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
