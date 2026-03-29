#pragma once

/// <summary>
/// Owned by Player object (탄알, 배낭, 힐 및 부스트 아이템, 
/// </summary>
class CInvenScript : public CScript
{
private:
    
public:

    CInvenScript();
    virtual ~CInvenScript() override;
    
    CLONE(CInvenScript)
    
    
public:
    
    void Tick() override;
    
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {};
    virtual void LoadFromLevelFile(FILE* _File) override {};
};
