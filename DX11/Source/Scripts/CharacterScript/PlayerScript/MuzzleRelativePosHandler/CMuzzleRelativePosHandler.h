#pragma once

class CMuzzleRelativePosHandler : public CScript
{
private:
    
public:
    
    CMuzzleRelativePosHandler();
    virtual ~CMuzzleRelativePosHandler() override;
    CLONE(CMuzzleRelativePosHandler);
    
public:
    
    void Tick() override;
    
public:
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
};
