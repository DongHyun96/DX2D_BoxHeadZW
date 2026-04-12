#pragma once

struct AmmoCountUIArea
{
    // class CText* 
};

class CIngameUIManager : public CScript
{
private:

public:
    CIngameUIManager();
    virtual ~CIngameUIManager() override;
    CLONE(CIngameUIManager);
    
public:
    
    void Tick() override;

public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
