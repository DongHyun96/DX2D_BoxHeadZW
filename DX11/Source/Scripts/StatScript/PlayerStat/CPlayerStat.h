#pragma once
#include "Source/Scripts/StatScript/CStatScript.h"

class CPlayerStat : public CStatScript
{
private:

    const float m_BoostMax = 100.f;
    float m_Boost{};
    
public:
    
    CPlayerStat();
    virtual ~CPlayerStat() override;
    CLONE(CPlayerStat)
    
public:
    
    void Tick() override;
    
public:

    bool ApplyBoost(float _BoostAmount);
    
public:

    // TODO : 나중에 전체 저장 및 게임 불러오기 처리로 할거면 제대로 구현할 것
    void SaveToLevelFile(FILE* _File) override {};
    void LoadFromLevelFile(FILE* _File) override {};
    
};
