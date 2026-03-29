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
    
    HandleRayCast(); // TODO : Raycasting Test 라인 지우기
    // MeshRender()->GetMaterial()->SetScalar(INT_0, KEY_PRESSED(KEY::X) ? 1 : 0);
    
    // TODO : 이 라인 지우기 ForTesting
    if (KEY_TAP(KEY::MRB))
    {
        GameObject* gObject = GM->GetZombiePoolManager()->SpawnObject(Transform()->GetWorldPos() + Vec3::UnitX * 300.f);
        if (gObject) m_Temp.push_back(gObject);
    }
    if (KEY_TAP(KEY::MLB))
    {
        for (GameObject* gObject : m_Temp)
            gObject->SetActive(false);
        
        m_Temp.clear();
    }
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

void CPlayerScript::HandleRayCast()
{
    if (KEY_PRESSED(KEY::MLB))
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
