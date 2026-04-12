#pragma once
#include "GameEngine/06.Component/Script/CScript.h"

class CGameUI : public CScript
{
protected:

public:
    CGameUI();
    CGameUI(int _ScriptType);
    CGameUI(const CGameUI& _Origin);
    virtual ~CGameUI() override;
    virtual CGameUI* Clone() const override = 0;

public:
    virtual void Begin() override;
    virtual void Tick() override;

public:

public:

    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
