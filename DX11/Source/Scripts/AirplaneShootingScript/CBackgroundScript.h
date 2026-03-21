#pragma once
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/Script/CScript.h"

class CBackgroundScript : public CScript
{
private:
    
    GameObject* m_BackGrounds[2]{};
    
    const float m_BackgroundSpeed = 600.f;
    
public:
    
    CBackgroundScript();
    CBackgroundScript(const CBackgroundScript& _Origin);
    
    
    virtual ~CBackgroundScript() override;

    CLONE(CBackgroundScript)
    
public:
    
    void Tick() override;
    
public:
    
    void AddBackGroundObject(GameObject* _BackGroundObj, UINT slot);

public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
};
