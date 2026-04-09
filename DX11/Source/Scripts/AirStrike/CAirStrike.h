#pragma once
#include "Source/Scripts/CCamMoveScript.h"

class CAirStrike : public CScript
{
private:
    
    static class ASound* s_AirStrikeSound;
    static bool s_IsAirStrikeSpawned; // 현재 AirStrike가 Spawn 되었는지 체크 (하나씩만 스폰 가능하도록 조정)

private:
    
    float m_Timer{};
    
    static const float EFFECT_SPAWN_INTERVAL; // Effect 스폰 Interval
    float m_EffectTimer = EFFECT_SPAWN_INTERVAL; // 첫 Effect는 바로 Spawn 처리 되도록 설정

private:
    
    ExplosionSpawnDesc m_ExplosionSpawnDesc{};
    
public:
    CAirStrike();
    virtual ~CAirStrike() override;
    CLONE(CAirStrike)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;

private:
    
    void TickWaitAirStrike();
    void TickAirStriking(const Ptr<CCamMoveScript>& CamMove);
    void TickAirStrikeFinish(const Ptr<CCamMoveScript>& CamMove);
    
public:
    
    /// <summary>
    /// 맵에 AirStrike가 스폰되어 아직 살아있는지 체크 
    /// </summary>
    static bool HasAirStrikeSpawnedAlive() { return s_IsAirStrikeSpawned; }
    
private:
    /// <summary>
    /// Explosion Dome 또는 Explosion Effect가 Damage 주는 것을 관장하는 것이 아닌, AirStrike 객체가 직접 Collider 원을 키우면서 해당 원에 Overlap되는 Enemy를 직접 타격한다 
    /// </summary>
    void BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
};
