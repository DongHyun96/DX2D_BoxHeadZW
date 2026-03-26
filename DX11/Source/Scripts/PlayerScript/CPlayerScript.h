#pragma once

#include "GameEngine/06.Component/Script/CScript.h"

/// <summary>
/// Basic Movement 및 MainState Holder 담당
/// </summary>
class CPlayerScript : public CScript
{
private:
    
    PLAYER_HANDSTATE m_HandState{}; // 현재 손에 들고 있는 무기 종류 State
    
private:
    float m_MoveSpeedBase       = 300.f;
    float m_MoveSpeedFactor     = 1.f;
    
    Vec3 m_Velocity{};
    
public:
    
    CPlayerScript();
    virtual ~CPlayerScript() override;
    CLONE(CPlayerScript)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
private:
    
    void Move();
    
private:
    
    /// <summary>
    /// For Testing
    /// </summary>
    void HandleRayCast();
    
public:
    
    const Vec3& GetVelocity() const { return m_Velocity; }
    
    void SetHandState(PLAYER_HANDSTATE _HandState) { m_HandState = _HandState; }
    PLAYER_HANDSTATE GetHandState() const { return m_HandState; }
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
};
