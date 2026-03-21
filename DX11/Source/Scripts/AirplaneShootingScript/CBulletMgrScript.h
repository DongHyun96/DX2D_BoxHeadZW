#pragma once
#include "CBulletScript.h"
#include "GameEngine/05.GameObject/GameObject.h"


class CBulletMgrScript : public CScript
{
public:
    
    static const UINT BULLET_POOL_COUNT;
    
private:
    
    list<pair <Ptr<GameObject>, Ptr<CBulletScript>> > m_BulletPool{};
    
public:
    
    CBulletMgrScript();
    CBulletMgrScript(const CBulletMgrScript& _Origin);
    
    virtual ~CBulletMgrScript() override;

    CLONE(CBulletMgrScript)
    
public:
    
    void AddBulletToPool(const Ptr<GameObject>& _Bullet);

public:
    
    void Tick() override;
    
    bool FireBullet(const Vec3& _StartPosition, const Vec3& _FireDirection = Vec3(0.f, 1.f, 0.f), float _Speed = 5.f);
    
public:
    
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}
        
};
