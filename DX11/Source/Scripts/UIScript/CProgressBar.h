#pragma once
#include "CGameUI.h"

class CProgressBar : public CGameUI
{
private:
    float m_Ratio;

public:
    CProgressBar();
    virtual ~CProgressBar() override;
    CLONE(CProgressBar)

public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    float SetRatio(float _Ratio);
    float GetRatio() const { return m_Ratio; }

};
