#include "pch.h"
#include "CCamMoveScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CCamMoveScript::CCamMoveScript()
    : CScript(static_cast<int>(SCRIPT_TYPE::CAMMOVESCRIPT))
{
}

CCamMoveScript::~CCamMoveScript()
{
}

void CCamMoveScript::Tick()
{
    // CheckTogglingTargetMode();
    // Move();
    
    // 일단은 Player를 무조건 따라가도록 처리
    float z = Transform()->GetRelativePosZ();
    const Vec3 PlayerPos = GM->GetPlayerObject()->Transform()->GetRelativePos();
    const Vec3 CamPos = {PlayerPos.x, PlayerPos.y, z}; 
    Transform()->SetRelativePos(CamPos);
}

void CCamMoveScript::MoveOrthographic()
{
    // Target mode
    if (m_2DFollowTargetObject)
    {
        CTransform* TargetTransform = m_2DFollowTargetObject->Transform().Get();
        m_FollowDestPos = TargetTransform->GetWorldPos() - Vec3(0.f, 0.f, TargetTransform->GetRelativePosZ());
        
        if (m_UseLerpToFollow)
        {
            Vec3 CurPos = Transform()->GetRelativePos();
            CurPos = Vec3::Lerp(CurPos, m_FollowDestPos, DT * 5.f);
            Transform()->SetRelativePos(CurPos);
        }
        else Transform()->SetRelativePos(m_FollowDestPos);
        
        return;
    }
    
    // Free mode
    
    Vec3 vPos = Transform()->GetRelativePos();

    Vec3 vDir{};
    
    if (KEY_PRESSED(KEY::W)) vDir.y += 1.f;
    if (KEY_PRESSED(KEY::S)) vDir.y -= 1.f;
    if (KEY_PRESSED(KEY::A)) vDir.x -= 1.f;
    if (KEY_PRESSED(KEY::D)) vDir.x += 1.f;
    
    vPos += vDir * DT * 400.f;
    Transform()->SetRelativePos(vPos);
}

void CCamMoveScript::MovePerspective()
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
    
    if (KEY_PRESSED(KEY::RMB))
    {
        Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
        
        vRot.y += vMouseDir.x * DT * XM_2PI * 3.f;
        vRot.x += vMouseDir.y * DT * XM_2PI * 3.f;
    }
    
    
    Transform()->SetRelativePos(vPos);
    Transform()->SetRelativeRot(vRot);
}

void CCamMoveScript::Move()
{
    if (Camera()->GetType() == PROJ_TYPE::ORTHOGRAPHIC) MoveOrthographic();
    else MovePerspective();
    
   
    

    

}

/*void CCamMoveScript::CheckTogglingTargetMode()
{
    if (KEY_TAP(KEY::ESC))
    {
        // if (m_2DFollowTargetObject->IsDead())    
        m_2DFollowTargetObject = (m_2DFollowTargetObject) ? nullptr : LevelMgr::GetInst()->GetPlayerObject();
    }
}*/
