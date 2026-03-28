#pragma once

class CEquipmentScript : public CScript
{
    
public:

    CEquipmentScript();
    virtual ~CEquipmentScript() override;
    CLONE(CEquipmentScript)
    
public:
    
    void Tick() override;
    
public:
    
    void SaveToLevelFile(FILE* _File) override {};
    void LoadFromLevelFile(FILE* _File) override {};
};
