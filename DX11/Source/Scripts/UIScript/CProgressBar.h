#pragma once
#include "CGameUI.h"

class CProgressBar : public CGameUI
{
private:
    float m_fRatio;

public:
    CProgressBar();
    CProgressBar(const CProgressBar& _Origin);
    virtual ~CProgressBar() override;
    CLONE(CProgressBar)

public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
public:
    
    void SetRatio(float _fRatio);
    float GetRatio() const { return m_fRatio; }

};
