#pragma once
#include "Source/Scripts/CharacterScript/EnemyScript/CEnemyScript.h"

class CRunner : public CEnemyScript
{
private:

    // Runner의 경우, 달릴 때 && 공격 시 16방면을 바라볼 수 있어서 해당 Direction 기록 처리를 한다
    SIXTEEN_DIRECTION m_CurrentFacedSixteenDirection{};
    
public:
    CRunner();
    virtual ~CRunner() override;
    CLONE(CRunner)
    
public:
    
    virtual void Begin() override;
    virtual void Tick() override;
};
