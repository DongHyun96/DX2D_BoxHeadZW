#pragma once

class CMuzzleSmokeScript : public CScript
{
private:
    
public:
    
    CMuzzleSmokeScript();
    virtual ~CMuzzleSmokeScript() override;
    CLONE(CMuzzleSmokeScript)

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
