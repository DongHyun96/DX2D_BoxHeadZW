#include "pch.h"
#include "CCamMoveScript.h"

#include "AirStrike/CAirStrike.h"
#include "BackgroundTile/CBackgroundTile.h"
#include "CharacterScript/PlayerScript/CPlayerScript.h"
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

void CCamMoveScript::Begin()
{
    GetOwner()->SetIgnoreGlobalTimeScale(true); // 슬로모션 영향 x 
}

void CCamMoveScript::Tick()
{
    // CheckTogglingTargetMode();
    // Move();
    
    // 일단은 Player를 무조건 따라가도록 처리
    GameObject* PlayerObject = GM->GetPlayerObject(); 
    const Vec3 PlayerPos = PlayerObject->Transform()->GetRelativePos();
    // Vec3 CamPos = {PlayerPos.x, PlayerPos.y, CAMERA2D_POS_Z};
    m_CamLerpDestPos = ToVec3(GM->GetPlayerObject()->Transform()->GetWorldPos2D(), CAMERA2D_POS_Z);
    
    const Vec2 ToMousePosDir = PlayerObject->GetScriptComponent<CPlayerScript>()->GetPlayerToMousePos();
    const float Length = ToMousePosDir.Length();

    // 바라보는 방향으로 Lerp Dest 조정
    m_CamLerpDestPos += ToVec3(ToMousePosDir.Normalized()) * MappingToNewRange(Length, 0.f, RESOL_DIAG_HALF_LENGTH, 0.f, 250.f);
    
    HandleCameraEffect();
    HandleBoundary();
    
    Vec3 FinalPos = Vec3::Lerp(Transform()->GetRelativePos(), m_CamLerpDestPos, m_CamLerpAlphaSpeed * DT);
    Transform()->SetRelativePos(FinalPos);
    
    float NewOrthoScale = Lerp(Camera()->GetOrthoScale(), m_OrthoScaleDest, m_OrthoScaleLerpAlphaSpeed * DT);
    Camera()->SetOrthoScale(NewOrthoScale);
}

void CCamMoveScript::HandleCameraEffect()
{
    if (!m_AirStriker) return;
    
    m_CamLerpDestPos = ToVec3(m_AirStriker->Transform()->GetRelativePosXY(), CAMERA2D_POS_Z);
}

void CCamMoveScript::HandleBoundary()
{
    CBackgroundTile* BackgroundCellMgr = GM->GetBackgroundCellManager();
    
    // Boundary 넘어가지 않도록 조정
    const float BoundaryX = RESOL_HALF_X * m_OrthoScaleDest;
    const float BoundaryY = RESOL_HALF_Y * m_OrthoScaleDest;
    
    if (m_CamLerpDestPos.x - BoundaryX < -BackgroundCellMgr->GetWorldSizeHalf())
        m_CamLerpDestPos.x = -BackgroundCellMgr->GetWorldSizeHalf() + BoundaryX;
    else if (m_CamLerpDestPos.x + BoundaryX > BackgroundCellMgr->GetWorldSizeHalf())
        m_CamLerpDestPos.x = BackgroundCellMgr->GetWorldSizeHalf() - BoundaryX;
    
    if (m_CamLerpDestPos.y - BoundaryY < -BackgroundCellMgr->GetWorldSizeHalf())
        m_CamLerpDestPos.y = -BackgroundCellMgr->GetWorldSizeHalf() + BoundaryY;
    else if (m_CamLerpDestPos.y + BoundaryY > BackgroundCellMgr->GetWorldSizeHalf())
        m_CamLerpDestPos.y = BackgroundCellMgr->GetWorldSizeHalf() - BoundaryY;
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
    
    if (KEY_PRESSED(KEY::MRB))
    {
        Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
        
        vRot.y += vMouseDir.x * DT * XM_2PI * 3.f;
        vRot.x += vMouseDir.y * DT * XM_2PI * 3.f;
    }
    
    
    Transform()->SetRelativePos(vPos);
    Transform()->SetRelativeRot(vRot);
}
