#pragma once

class CPlayerSwapWeapon : public CScript
{
private:
    
    class CPlayerScript* m_PlayerMainScript{};
    
public:
    
    CPlayerSwapWeapon();
    virtual ~CPlayerSwapWeapon() override;
    
    CLONE(CPlayerSwapWeapon)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    void SaveToLevelFile(FILE* _File) override {};
    void LoadFromLevelFile(FILE* _File) override {};
};
