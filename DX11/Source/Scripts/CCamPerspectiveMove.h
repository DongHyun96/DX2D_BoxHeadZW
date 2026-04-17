#pragma once

class CCamPerspectiveMove : public CScript
{
private:


public:
    CCamPerspectiveMove();
    virtual ~CCamPerspectiveMove() override;
    CLONE(CCamPerspectiveMove);

public:
    
    void Tick() override;
    
public:
    
    void SaveToLevelFile(FILE* _File) override {}
    void LoadFromLevelFile(FILE* _File) override {}
};
