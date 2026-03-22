#include "pch.h"
#include "CPlayerScript.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/08.CollisionMgr/CollisionMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"
#include "GameEngine/06.Component/03.Collider2D/CColliderRect.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CPlayerScript::CPlayerScript()
    : CScript(static_cast<int>(SCRIPT_TYPE::PLAYERSCRIPT))
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Init()
{
    
    /*AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, L"PlayerSpeed", false);
    AddScriptParam(SCRIPT_PARAM::TEXTURE, &m_Tex, L"TextureExample");
    
    m_BulletPrefab = LOAD_ASSET(APrefab, L"Prefab\\Bullet.pref");*/
    GM->SetPlayerObject(GetOwner());
}

void CPlayerScript::Begin()
{
    if (GetOwner()->FlipbookRender())
        GetOwner()->FlipbookRender()->Play(L"UnArmed", 0, 10.f, 2, true);
}

void CPlayerScript::Tick()
{
    Move();
    
    // MeshRender()->GetMaterial()->SetScalar(INT_0, KEY_PRESSED(KEY::X) ? 1 : 0);
}

void CPlayerScript::Move()
{
    Vec3 Direction{};
    
    if (KEY_PRESSED(KEY::D)) Direction.x += 1.f;
    if (KEY_PRESSED(KEY::W)) Direction.y += 1.f;
    if (KEY_PRESSED(KEY::A)) Direction.x -= 1.f;
    if (KEY_PRESSED(KEY::S)) Direction.y -= 1.f;

    if (Direction.LengthSquared() == 0.f) return;
    Direction.Normalize();
    
    m_Direction = Direction; // 마지막 Direction 방향 기록
    
    Vec3 Pos = Transform()->GetRelativePos() + m_Direction * m_MoveSpeedBase * DT;
    Transform()->SetRelativePos(Pos);
}

void CPlayerScript::HandleRayCast()
{
    if (KEY_PRESSED(KEY::LMB))
    {
        Ray2D Ray{};
        Ray.Origin = Transform()->GetWorldPos();
        const Vec3 MousePos = KeyMgr::GetInst()->GetMouseWorldPos();
        
        Ray.Dir = MousePos - Ray.Origin;
        Ray.MaxDistance = Ray.Dir.Length();
        Ray.Dir.Normalize();

        RayCastHit Hit{};

        Vec4 Color = Vec4(0.f, 1.f, 0.f, 1.f); 
        
        if (CollisionMgr::GetInst()->RayCast(Ray, {5, 2, 0}, &Hit))
        {
            DrawDebugCircle(Hit.Point, 10.f, Vec4(0.f, 1.f, 1.f, 1.f), 0.f);
            Color = Vec4(1.f, 0.f, 0.f, 1.f);
        }
        
        DrawDebugLine(Transform()->GetWorldPos(), Transform()->GetWorldPos() + Ray.Dir * Ray.MaxDistance, Color, 0.f);
    }
}
