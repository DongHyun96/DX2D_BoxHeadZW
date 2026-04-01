#pragma once

class CMuzzleFlashScript : public CScript
{
private:
    
public:
    
    CMuzzleFlashScript();
    virtual ~CMuzzleFlashScript() override;
    CLONE(CMuzzleFlashScript)

public:

    virtual void Begin() override;
    virtual void AfterLevelBegin() override;
    virtual void Tick() override;

private:
    
    void OnFlashAnimationEnd() const { GetOwner()->SetActive(false); }
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
    
};
