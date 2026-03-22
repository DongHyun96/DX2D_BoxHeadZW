#pragma once

#include "GameEngine/06.Component/Script/CScript.h"
#include "AirplaneShootingScript/CBulletMgrScript.h"
#include "GameEngine/04.Asset/09.Prefab/APrefab.h"

class CPlayerScript : public CScript
{
private:
    
    float m_MoveSpeedBase = 500.f;
    
    Vec3 m_Direction = Vec3(0.f, -1.f, 0.f); // 첫 Direction은 앞을 바라보는 것으로(Down 방향)
    
public:
    
    CPlayerScript();
    virtual ~CPlayerScript() override;
    CLONE(CPlayerScript)
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
private:
    
    void Move();
    
    /// <summary>
    /// For Testing
    /// </summary>
    void HandleRayCast();
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
    
};
