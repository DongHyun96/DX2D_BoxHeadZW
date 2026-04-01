#pragma once

class CRocketSmoke : public CScript
{
private:
    
public:

    CRocketSmoke();
    virtual ~CRocketSmoke() override;
    CLONE(CRocketSmoke)

public:

    virtual void Begin() override;
    virtual void AfterLevelBegin() override;
    virtual void Tick() override;

private:
    
    void OnSmokeAnimationEnd() const { GetOwner()->SetActive(false); }
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
    
};
