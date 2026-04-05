#pragma once

class CAirStrike : public CScript
{
private:

public:
    CAirStrike();
    virtual ~CAirStrike() override;
    CLONE(CAirStrike)
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
};
