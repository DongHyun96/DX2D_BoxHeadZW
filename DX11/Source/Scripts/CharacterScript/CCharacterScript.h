#pragma once

class CCharacterScript : public CScript
{
protected:
    
    float m_MoveSpeedBase       = 300.f; // TODO : const 변수로 바꿀 것
    float m_MoveSpeedFactor     = 1.f;
    
    Vec3 m_Velocity{};
    
protected:
    
    EDIRECTION m_CurrentFacedDirection{};

public:
    
    CCharacterScript(enum SCRIPT_TYPE _ScriptType);
    virtual ~CCharacterScript() override;
    
public:
    
    virtual void Tick() override;

private:
    
    virtual void Move() = 0;
    virtual void UpdateCurrentFacedDirection() = 0;

public:
    
    virtual void SaveToLevelFile(FILE* _File) override {};
    virtual void LoadFromLevelFile(FILE* _File) override {};
    
public:
    
    const Vec3& GetVelocity() const { return m_Velocity; }
    EDIRECTION GetCurrentFacedDirection() const { return m_CurrentFacedDirection; }
    
};
