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

CPlayerScript::CPlayerScript()
    : CScript(static_cast<int>(SCRIPT_TYPE::PLAYERSCRIPT))
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Init()
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, L"PlayerSpeed", false);
    AddScriptParam(SCRIPT_PARAM::TEXTURE, &m_Tex, L"TextureExample");
    
    m_BulletPrefab = LOAD_ASSET(APrefab, L"Prefab\\Bullet.pref");
}

void CPlayerScript::Begin()
{
    if (GetOwner()->FlipbookRender())
        GetOwner()->FlipbookRender()->Play(0, 10.f, 5);
}

void CPlayerScript::Tick()
{
    Move();
    Shoot();
    HandleLight();
    HandleRayCast();

    // MeshRender()->GetMaterial()->SetScalar(INT_0, KEY_PRESSED(KEY::X) ? 1 : 0);
}

void CPlayerScript::SaveToLevelFile(FILE* _File)
{
}

void CPlayerScript::LoadFromLevelFile(FILE* _File)
{
}


void CPlayerScript::Move()
{
    /*Vec3 vPos       = Transform()->GetRelativePos();
    Vec3 vScale     = Transform()->GetRelativeScale();
    Vec3 vRotation  = Transform()->GetRelativeRot();

    Vec3 vUp = Transform()->GetDir(DIR::UP);
    Vec3 vDown = -vUp;

    if (KEY_PRESSED(KEY::UP))
        vPos += vUp * 500.f * DT;
    if (KEY_PRESSED(KEY::DOWN))
        vPos += vDown * 500.f * DT;

    if (KEY_PRESSED(KEY::RIGHT))
        vRotation.z -= XM_PI * DT;
    if (KEY_PRESSED(KEY::LEFT))
        vRotation.z += XM_PI * DT;	

    Transform()->SetRelativePos(vPos);
    Transform()->SetRelativeScale(vScale);
    Transform()->SetRelativeRot(vRotation);*/

    
    
    
    
    m_DirDest = Vec3(0.f, 0.f, 0.f);
    
    if (KEY_PRESSED(KEY::UP))       m_DirDest.y += 1.f;
    if (KEY_PRESSED(KEY::DOWN))     m_DirDest.y -= 1.f;
    if (KEY_PRESSED(KEY::LEFT))     m_DirDest.x -= 1.f;
    if (KEY_PRESSED(KEY::RIGHT))    m_DirDest.x += 1.f;
    
    m_CurDir = Vec3::Lerp(m_CurDir, m_DirDest, DT * 5.f);
    
    Transform()->SetRelativeRot(Vec3(0.f, 0.f, atan2f(m_CurDir.y, m_CurDir.x)));
    
    Vec3 newPos = Transform()->GetRelativePos() + m_CurDir * m_Speed * DT;
    Transform()->SetRelativePos(newPos);
    
    
    
    
    /*Vec3 Direction{};
    
    if (KEY_PRESSED(KEY::UP))       Direction.y += 1.f;
    if (KEY_PRESSED(KEY::DOWN))     Direction.y -= 1.f;
    if (KEY_PRESSED(KEY::LEFT))     Direction.x -= 1.f;
    if (KEY_PRESSED(KEY::RIGHT))    Direction.x += 1.f;
    
    Direction.Normalize();
    
    const Vec3 NewPos = Transform()->GetRelativePos() + Direction * m_Speed * DT;
    Transform()->SetRelativePos(NewPos);*/
    
}

/// <summary>
/// Currently not applicable 
/// </summary>
void CPlayerScript::Shoot()
{
    Vec3 vPos = Transform()->GetRelativePos();
    
    static const float Y_OFFSET = 0.2f;
    
    /*if (KEY_TAP(KEY::SPACE))
    {
        SpawnBullet();
        return;
    }
    
    static float fireTimer{};
    if (KEY_PRESSED(KEY::SPACE))
    {
        static const float FIRE_CYCLE = 0.15f; // 0.1초를 주기로 fire

        fireTimer += DT;
        if (fireTimer > FIRE_CYCLE)
        {
            SpawnBullet();
            fireTimer = 0.f;
        }
    }
    else fireTimer = 0.f;*/
    
    if (KEY_TAP(KEY::SPACE))
    {
        // 미사일 역할의 오브젝트 생성
        /*GameObject* pObject = new GameObject;
        pObject->SetIsActive(false);
        
        pObject->AddComponent(new CTransform);
        pObject->AddComponent(new CMeshRender);
        pObject->AddComponent(new CBulletScript);
        pObject->AddComponent(new CColliderRect);
        
        // pObject->GetScriptComponent<CBulletScript>()->SetTarget(m_Target);

        pObject->MeshRender()->SetMesh(AssetMgr::GetInst()->Find<AMesh>(L"RectMesh"));
        pObject->MeshRender()->SetMaterial(AssetMgr::GetInst()->Find<AMaterial>(L"BulletMaterial"));
        
        Vec3 vForward2D     = Transform()->GetDir(DIR::RIGHT);
        Vec3 FireStartPos   = Transform()->GetRelativePos() + vForward2D * 50.f;

        pObject->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 1.f));
        pObject->Transform()->SetRelativeRot(Vec3(0.f, 0.f, XM_PI * 0.5f));
        
        pObject->GetScriptComponent<CBulletScript>()->Fire(FireStartPos, vForward2D, 1000.f);

        CreateObject(pObject, 4);*/
        
        
        Vec3 vForward2D     = Transform()->GetDir(DIR::RIGHT);
        Vec3 FireStartPos   = Transform()->GetRelativePos() + vForward2D * 50.f;
        
        GameObject* BulletObject = Instantiate(m_BulletPrefab.Get(), 4, FireStartPos);
        BulletObject->GetScriptComponent<CBulletScript>()->Fire(FireStartPos, vForward2D, 1000.f);
    }
    
    // if (KEY_TAP(KEY::Z)) Destroy();
    
    if (KEY_TAP(KEY::SPACE))
    {
        // DrawDebugRect()
        // DrawDebugRect(Transform()->GetRelativePos(), Transform()->GetRelativeScale() * 2.f, Vec3(0.f, 0.f, 0.f), Vec4(1.f, 1.f, 0.f, 0.f), 1.f);
        DrawDebugRect(Transform()->GetWorldMatrix(), Vec4(1.f, 1.f, 0.f, 1.f), 1.f);
    }
}

void CPlayerScript::SpawnBullet()
{
    Vec3 FireStartPos = Transform()->GetRelativePos() + Transform()->GetDir(DIR::UP) * 50.f;
    m_BulletMgrScript->FireBullet(FireStartPos, Transform()->GetDir(DIR::UP), 1000.f);
}

void CPlayerScript::HandleLight()
{
    // m_SpotLight->SetLightColor(GetRandomColor());
    if (KEY_PRESSED(KEY::Z))
    {
        const float NewAngle =  max(m_SpotLight->GetAngle() - XM_PI * DT, 0.f);
        m_SpotLight->SetAngle(NewAngle);
    }
    
    if (KEY_PRESSED(KEY::X))
    {
        const float NewAngle =  min(m_SpotLight->GetAngle() + XM_PI * DT, XM_PI * 2.f);
        m_SpotLight->SetAngle(NewAngle);
    }
    
    if (KEY_PRESSED(KEY::C))
    {
        const float NewRad = max(m_SpotLight->GetRadius() - 500.f * DT, 5.f);
        m_SpotLight->SetRadius(NewRad);
    }
    
    if (KEY_PRESSED(KEY::V))
    {
        const float NewRad = min(m_SpotLight->GetRadius() + 500.f * DT, 5000.f);
        m_SpotLight->SetRadius(NewRad);
    }
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
