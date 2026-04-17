#include "pch.h"
#include "CCamPerspectiveMove.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"

CCamPerspectiveMove::CCamPerspectiveMove()
    : CScript(SCRIPT_TYPE::CAMPERSPECTIVEMOVE)
{
}

CCamPerspectiveMove::~CCamPerspectiveMove()
{
}

void CCamPerspectiveMove::Tick()
{
    Vec3 vPos = Transform()->GetRelativePos();
    Vec3 vRot = Transform()->GetRelativeRot();

    Vec3 vFront = Transform()->GetDir(DIR::FRONT);
    Vec3 vRight = Transform()->GetDir(DIR::RIGHT);
    Vec3 vUp = Transform()->GetDir(DIR::UP);
    
    if (1 == KeyMgr::GetInst()->GetMouseWheel()) vPos += vFront * 20.f;
    if (-1 == KeyMgr::GetInst()->GetMouseWheel()) vPos -= vFront * 20.f;
    
    if (KEY_PRESSED(KEY::W)) vPos += vFront * DT * 200.f;
    if (KEY_PRESSED(KEY::S)) vPos -= vFront * DT * 200.f;
    if (KEY_PRESSED(KEY::A)) vPos -= vRight * DT * 200.f;
    if (KEY_PRESSED(KEY::D)) vPos += vRight * DT * 200.f;
        
    if (KEY_PRESSED(KEY::Q)) vPos += vUp * DT * 400.f;
    if (KEY_PRESSED(KEY::E)) vPos -= vUp * DT * 400.f;
    
    if (KEY_PRESSED(KEY::MRB))
    {
        Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
        
        vRot.y += vMouseDir.x * DT * XM_2PI * 3.f;
        vRot.x += vMouseDir.y * DT * XM_2PI * 3.f;
    }
    
    
    Transform()->SetRelativePos(vPos);
    Transform()->SetRelativeRot(vRot);
}
