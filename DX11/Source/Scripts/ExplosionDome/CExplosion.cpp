#include "pch.h"
#include "CExplosion.h"

#include "Source/ScriptMgr.h"

const Vec2 CExplosion::s_ScaleSizeBase = { 200.f, 200.f };

CExplosion::CExplosion()
    : CExplosionDome(SCRIPT_TYPE::EXPLOSION)
{
}

CExplosion::~CExplosion()
{
}

void CExplosion::SetExplosionSize(float _Factor)
{
    Transform()->SetRelativeScaleXY(s_ScaleSizeBase * _Factor);    
}
