#pragma once

/// <summary>
/// Owned by Player object
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
