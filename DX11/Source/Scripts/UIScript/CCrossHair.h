#pragma once

class CCrossHair : public CScript
{
private:

    float m_AnimFPS{};
    
public:
    
    CCrossHair();
    virtual ~CCrossHair() override;
    CLONE(CCrossHair);
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
