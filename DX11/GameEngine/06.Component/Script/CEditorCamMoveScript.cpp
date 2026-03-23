#include "pch.h"
#include "CEditorCamMoveScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"
#include "Source/ScriptMgr.h"

CEditorCamMoveScript::CEditorCamMoveScript()
    : CScript(-1)
{
}

CEditorCamMoveScript::~CEditorCamMoveScript()
{
}

void CEditorCamMoveScript::Init()
{
    
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_MoveSpeed, L"CamSpeed", false);
}

void CEditorCamMoveScript::Tick()
{
    float Speed = m_MoveSpeed * Camera()->GetOrthoScale();
    if (KEY_PRESSED(KEY::LSHIFT))
        Speed = 14000.f;
    
    if (KEY_PRESSED(KEY::MRB))
    {
        Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
        Vec3 Direction = Vec3(-vMouseDir.x, vMouseDir.y, 0.f);
        
        m_TransformPosDest += Direction * Speed * E_DT;
    }
    
    Vec3 Pos = Transform()->GetRelativePos();
    Pos = Vec3::Lerp(Pos, m_TransformPosDest, E_DT * 15.f);
    Transform()->SetRelativePos(Pos);

    MouseWheelTick();
    
    
    // if (Camera()->GetType() == PROJ_TYPE::ORTHOGRAPHIC) MoveOrthographic();
    // else MovePerspective();
}

void CEditorCamMoveScript::MouseWheelTick()
{
    if (!KEY_PRESSED(KEY::CTRL)) return;

    float OrthoScale = Camera()->GetOrthoScale();
    
    if (KeyMgr::GetInst()->GetMouseWheel() == 1)
        OrthoScale *= 0.9f;
    if (KeyMgr::GetInst()->GetMouseWheel() == -1)
        OrthoScale *= 1.1f;
    
    Camera()->SetOrthoScale(OrthoScale);
}

void CEditorCamMoveScript::MoveOrthographic()
{
    Vec3 vPos = Transform()->GetRelativePos();

    Vec3 vDir{};
    
    if (KEY_PRESSED(KEY::W)) vDir.y += 1.f;
    if (KEY_PRESSED(KEY::S)) vDir.y -= 1.f;
    if (KEY_PRESSED(KEY::A)) vDir.x -= 1.f;
    if (KEY_PRESSED(KEY::D)) vDir.x += 1.f;
    
    float Speed = m_Speed;
    if (KEY_PRESSED(KEY::LSHIFT))
        Speed = 1000.f;
    
    vPos += vDir * E_DT * Speed;
    Transform()->SetRelativePos(vPos);
}

void CEditorCamMoveScript::MovePerspective()
{
    Vec3 vPos = Transform()->GetRelativePos();
    Vec3 vRot = Transform()->GetRelativeRot();

    Vec3 vFront = Transform()->GetDir(DIR::FRONT);
    Vec3 vRight = Transform()->GetDir(DIR::RIGHT);
    Vec3 vUp = Transform()->GetDir(DIR::UP);
    
    if (1 == KeyMgr::GetInst()->GetMouseWheel()) vPos += vFront * 20.f;
    if (-1 == KeyMgr::GetInst()->GetMouseWheel()) vPos -= vFront * 20.f;

    float Speed = m_Speed;
    if (KEY_PRESSED(KEY::LSHIFT))
        Speed = 1000.f;
    
    if (KEY_PRESSED(KEY::W)) vPos += vFront * E_DT * Speed;
    if (KEY_PRESSED(KEY::S)) vPos -= vFront * E_DT * Speed;
    if (KEY_PRESSED(KEY::A)) vPos -= vRight * E_DT * Speed;
    if (KEY_PRESSED(KEY::D)) vPos += vRight * E_DT * Speed;
        
    if (KEY_PRESSED(KEY::Q)) vPos += vUp * E_DT * Speed;
    if (KEY_PRESSED(KEY::E)) vPos -= vUp * E_DT * Speed;
    
    if (KEY_PRESSED(KEY::MRB))
    {
        Vec2 vMouseDir = KeyMgr::GetInst()->GetMouseDir();
        
        vRot.y += vMouseDir.x * E_DT * XM_2PI * 3.f;
        vRot.x += vMouseDir.y * E_DT * XM_2PI * 3.f;
    }
    
    
    Transform()->SetRelativePos(vPos);
    Transform()->SetRelativeRot(vRot);
}
