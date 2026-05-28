#pragma once

class CStructureHPBar : public CScript
{
private:

    GameObject*         m_MainProgressBarGameObject{};
    GameObject*         m_BackgroundGameObject{};
    
    class CProgressBar* m_MainProgressBar{};
    
private:

    AMaterial*          m_MainBarMtrl{};
    AMaterial*          m_BackgroundBarMtrl{};
    

    float               m_MainBarTintAlphaOrigin{};
    float               m_BackgroundTintAlphaOrigin{};

private:
    
    CTransform*         m_PlayerTransform{};
    CTransform*         m_StructureTransform{};
    
public:
    
    CStructureHPBar();
    virtual ~CStructureHPBar() override;
    
    CLONE(CStructureHPBar);
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

public:
    
    bool UpdateHPBar(float _HP, float _MaxHP);
    void UpdateHPBar(float _Ratio);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
};
