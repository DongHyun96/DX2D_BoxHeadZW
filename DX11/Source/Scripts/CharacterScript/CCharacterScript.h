#pragma once

class CCharacterScript : public CScript
{
protected:
    
    float m_MoveSpeedBase       = 300.f;
    float m_MoveSpeedFactor     = 1.f;
    
    Vec3 m_Velocity{};

public:
    
    CCharacterScript();
    CCharacterScript(int _ScriptType);
    virtual ~CCharacterScript() override;
    
public:
    
    virtual void Tick() override;

private:
    
    virtual void Move() = 0;
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {};
    virtual void LoadFromLevelFile(FILE* _File) override {};
    
};
