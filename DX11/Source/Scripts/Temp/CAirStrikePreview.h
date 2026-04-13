#pragma once


class CAirStrikePreview : public CScript
{
public:
    CAirStrikePreview();
    virtual ~CAirStrikePreview() override;
    CLONE(CAirStrikePreview)

public:
    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    virtual  void SaveToLevelFile(FILE* _File) override;
    virtual  void LoadFromLevelFile(FILE* _File) override;
};