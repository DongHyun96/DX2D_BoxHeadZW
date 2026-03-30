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
    : CCharacterScript(SCRIPT_TYPE::PLAYERSCRIPT)
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Init()
{

    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_MoveSpeedBase, L"PlayerSpeedBase", false);
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_MoveSpeedFactor, L"PlayerSpeedFactor", false);
    
    /*AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, L"PlayerSpeed", false);
    AddScriptParam(SCRIPT_PARAM::TEXTURE, &m_Tex, L"TextureExample");
    
    m_BulletPrefab = LOAD_ASSET(APrefab, L"Prefab\\Bullet.pref");*/
    // if (!APrefab::IsPrefabPrototypeObject(GetOwner()))
    GM->SetPlayerObject(GetOwner());
}

void CPlayerScript::Begin()
{
    if (GetOwner()->FlipbookRender())
        GetOwner()->FlipbookRender()->Stop(L"UnArmed", 6, 0);
    
    /*Collider2D()->AddDynamicBeginOverlap(this, static_cast<COLLISION_EVENT>(&CBulletScript::BeginOverlap));
    Collider2D()->AddDynamicOverlap     (this, static_cast<COLLISION_EVENT>(&CBulletScript::Overlap));
    Collider2D()->AddDynamicEndOverlap  (this, static_cast<COLLISION_EVENT>(&CBulletScript::EndOverlap));*/
}

void CPlayerScript::Tick()
{
    CCharacterScript::Tick();
    
    // HandleRayCast(); // TODO : Raycasting Test 라인 지우기
    // MeshRender()->GetMaterial()->SetScalar(INT_0, KEY_PRESSED(KEY::X) ? 1 : 0);
    
}

void CPlayerScript::Move()
{
    Transform()->SetPrevRelativePos(Transform()->GetRelativePos()); // 이동 처리 직전에 이전 PrevPos 저장(blocking 처리용)
    
    // Velocity 초기화
    m_Velocity = Vec3();
    
    Vec3 Direction{};
    
    if (KEY_PRESSED(KEY::A)) Direction.x -= 1.f; // Left
    if (KEY_PRESSED(KEY::D)) Direction.x += 1.f; // Right
    if (KEY_PRESSED(KEY::W)) Direction.y += 1.f; // Up
    if (KEY_PRESSED(KEY::S)) Direction.y -= 1.f; // Down

    // TODO : 이 라인 지우기 (testing 환경에서의 Fast Move 처리)
    m_MoveSpeedFactor = KEY_PRESSED(KEY::LSHIFT) ? 2.f : 1.f; 

    if (Direction.LengthSquared() == 0.f) return;
    Direction.Normalize();
    
    m_Velocity = Direction * m_MoveSpeedBase * m_MoveSpeedFactor;
    
    Vec3 Pos = Transform()->GetRelativePos() + m_Velocity * DT;
    Transform()->SetRelativePos(Pos);
}

void CPlayerScript::UpdateCurrentFacedDirection()
{
    const Vec2 MousePos     = ToVec2(KeyMgr::GetInst()->GetMouseWorldPos());
    const Vec2 PlayerPos2D  = ToVec2(Transform()->GetRelativePos());
    
    m_PlayerToMousePos      = MousePos - PlayerPos2D;
    m_CurrentFacedDirection = GetEightDirection(m_PlayerToMousePos);
    
    // 이 경우, 마우스포인터 좌표와 Player의 위치가 완전히 일치하는 상황 (거의 아예 안나올거다)
    // 따로 DOWN 방향으로 처리
    if (m_CurrentFacedDirection == EDIRECTION::END) m_CurrentFacedDirection = EDIRECTION::DOWN;
}

void CPlayerScript::HandleRayCast()
{
    if (KEY_PRESSED(KEY::MLB))
    {
        Ray2D Ray{};
        Ray.Origin = Transform()->GetWorldPos();
        const Vec2 MousePos = KeyMgr::GetInst()->GetMouseWorldPos2D();
        
        Ray.Direction = MousePos - Ray.Origin;
        Ray.MaxDistance = Ray.Direction.Length();
        Ray.Direction.Normalize();

        RayCastHit Hit{};

        Vec4 Color = Vec4(0.f, 1.f, 0.f, 1.f); 
        
        if (CollisionMgr::GetInst()->RayCast(Ray, {5, 2, 0}, &Hit))
        {
            DrawDebugCircle(ToVec3(Hit.Point), 10.f, Vec4(0.f, 1.f, 1.f, 1.f), 0.f);
            Color = Vec4(1.f, 0.f, 0.f, 1.f);
        }
        
        DrawDebugLine(Transform()->GetWorldPos(), Transform()->GetWorldPos() + Ray.Direction * Ray.MaxDistance, Color, 0.f);
    }
}

void CPlayerScript::SaveToLevelFile(FILE* _File)
{
    CCharacterScript::SaveToLevelFile(_File);
}

void CPlayerScript::LoadFromLevelFile(FILE* _File)
{
    CCharacterScript::LoadFromLevelFile(_File);
}
