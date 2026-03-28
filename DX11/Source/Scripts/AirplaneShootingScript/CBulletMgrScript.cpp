#include "pch.h"
#include "CBulletMgrScript.h"

#include "CBulletScript.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "Source/ScriptMgr.h"

const UINT CBulletMgrScript::BULLET_POOL_COUNT = 100;

CBulletMgrScript::CBulletMgrScript()
    : CScript(static_cast<int>(SCRIPT_TYPE::BULLETMGRSCRIPT))
{
    
}

CBulletMgrScript::CBulletMgrScript(const CBulletMgrScript& _Origin)
    : CScript(_Origin)
{
}

CBulletMgrScript::~CBulletMgrScript()
{
}

void CBulletMgrScript::AddBulletToPool(const Ptr<GameObject>& _Bullet)
{
    _Bullet->SetActive(false);
    Ptr<CBulletScript> BulletScript = _Bullet->GetScriptComponent<CBulletScript>();
    m_BulletPool.push_back( {_Bullet, BulletScript} );
}

void CBulletMgrScript::Tick()
{
}

bool CBulletMgrScript::FireBullet(const Vec3& _StartPosition, const Vec3& _FireDirection, float _Speed)
{
    for (pair<Ptr<GameObject>, Ptr<CBulletScript>>& bullet : m_BulletPool)
    {
        if (bullet.first->GetActive()) continue;

        bullet.second->Fire(_StartPosition, _FireDirection, _Speed);
        return true;
    }
    
    return false;
}
