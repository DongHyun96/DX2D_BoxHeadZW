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
    virtual void Tick() override;
    
public:
    
    void SetRatio(float _fRatio) { m_fRatio = _fRatio; }
    float GetRatio() const { return m_fRatio; }

};
