#pragma once

#include "GameEngine/06.Component/Script/CScript.h"

class CPlayerScript : public CScript
{
private:
    
    const float m_MoveSpeedBase     = 500.f;
    float       m_SpeedFactor       = 1.f;
    
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
    void UpdateAnimDirection();
    
private:
    
    /// <summary>
    /// For Testing
    /// </summary>
    void HandleRayCast();
    
public:
    
    const Vec3& GetVelocity() const { return m_Velocity; }
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
};
