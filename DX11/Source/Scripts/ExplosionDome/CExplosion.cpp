#include "pch.h"
#include "CExplosion.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"

const Vec2 CExplosion::s_ScaleSizeBase = { 200.f, 200.f };

CExplosion::CExplosion()
    : CExplosionDome(SCRIPT_TYPE::EXPLOSION)
{
}

CExplosion::~CExplosion()
{
}

void CExplosion::Begin()
{
    CExplosionDome::Begin();
    GetOwner()->AddActivateDelegate(bind(&CExplosion::OnActivateOwnerObject, this, placeholders::_1));
}

void CExplosion::Tick()
{
    CExplosionDome::Tick();
    
    Vec2 RenderOffset = GetRenderCom()->GetRenderOffset();
    RenderOffset.y += m_UpwardSpeed * DT;
    GetRenderCom()->SetRenderOffset(RenderOffset);
    
    GetRenderCom()->SetRenderScale({GetRandom(0.8f, 1.1f), GetRandom(0.8f, 1.1f)});
    
}

void CExplosion::SetExplosionSize(float _Factor)
{
    Transform()->SetRelativeScaleXY(s_ScaleSizeBase * _Factor);    
}

void CExplosion::OnActivateOwnerObject(const Ptr<GameObject>& _OwnerObject)
{
    // DebugUtil::AddDebugLog("CExplosion::OnActivateOwnerObject");
    
    GetRenderCom()->SetRenderOffset({0.f, 0.f});
    GetRenderCom()->SetRenderScale({1.f, 1.f});
    
    m_UpwardSpeed = GetRandom(0.5f, 1.f);
    
    GetCollider2D()->SetActive(m_UseCollisionForDamaging);
}
