#pragma once

class CFlipbookEffectScript : public CScript
{
private:
    
    
public:
    
    CFlipbookEffectScript();
    virtual ~CFlipbookEffectScript() override;
    CLONE(CFlipbookEffectScript)
    
public:
    
    void Tick() override;
    

public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
    
};
