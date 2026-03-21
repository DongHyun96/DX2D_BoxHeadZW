#pragma once

#include "GameEngine/06.Component/Script/CScript.h"
#include "AirplaneShootingScript/CBulletMgrScript.h"
#include "GameEngine/04.Asset/09.Prefab/APrefab.h"

class CPlayerScript : public CScript
{
private:
    
    Ptr<GameObject> m_Target{};
    Ptr<ATexture> m_Tex{};
    Ptr<APrefab> m_BulletPrefab{};

    float m_Speed = 500.f;
    
    Vec3 m_RotDest{};
    
    Vec3 m_CurDir{};
    Vec3 m_DirDest{};
    
private:
    
    Ptr<CBulletMgrScript> m_BulletMgrScript{};

    float m_CurAirplaneSpriteFrameX = 6.f;
    
private:
    
    class CLight2D* m_SpotLight{};

public:
    
    CPlayerScript();
    virtual ~CPlayerScript() override;
    
public:

    virtual void Init() override;
    virtual void Begin() override;
    virtual void Tick() override;
    
    CLONE(CPlayerScript)
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public: 
    
    void SetBulletMgrScript(const Ptr<CBulletMgrScript>& _BulletMgrScript) { this->m_BulletMgrScript = _BulletMgrScript; }
    
    SET(Ptr<GameObject>, Target)

    CLight2D* GetSpotLight() const { return m_SpotLight; }
    void SetSpotLight(CLight2D* _Light2D) { m_SpotLight = _Light2D; }

private:
    
    void Move();
    
    /// <summary>
    /// Currently not applicable 
    /// </summary>
    void Shoot();
    
    void SpawnBullet();
    
    void HandleLight();
    void HandleRayCast();
    
};
