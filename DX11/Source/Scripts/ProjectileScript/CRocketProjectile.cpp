#include "pch.h"
#include "CRocketProjectile.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

const float CRocketProjectile::s_MAX_LIFETIME = 5.f;
// const float CRocketProjectile::s_SMOKE_SPAWN_INTERVAL = 0.5f;
const float CRocketProjectile::s_SMOKE_SPAWN_INTERVAL = 0.1f;

CRocketProjectile::CRocketProjectile()
    : CScript(SCRIPT_TYPE::ROCKETPROJECTILE)
{
}

CRocketProjectile::~CRocketProjectile()
{
}

void CRocketProjectile::Init()
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_Speed, L"ProjectileSpeed");
}

void CRocketProjectile::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CRocketProjectile::BeginOverlap);
    FlipbookRender()->SetCurrentCategory(L"RocketProjectile");
}

void CRocketProjectile::AfterLevelBegin()
{
    // GM에 Owner PoolComponent 등록
    if (!GM->GetRocketProjectilePooler())
    {
        // ObjectPooling 처리된 객체의 OwnerPooler에 한해서만 등록 처리를 진행시킴 (Testing 환경에서 Pooling 되지 않은 오브젝트가 있을 수 있음)
        if (CPoolComponent* PoolComponent = GetOwner()->GetOwnerPoolComponent())
            GM->SetRocketProjectilePooler(PoolComponent);
    }
}

void CRocketProjectile::Tick()
{
    // Move 처리
    Vec3 Pos = Transform()->GetRelativePos();
    Pos += m_Direction * m_Speed * DT;
    Transform()->SetRelativePos(Pos);
    
    // Handle Boundary -> 실질적인 Map Boundary 잡히면 잡아줄 것 (일단은 LifeTime으로 처리할 것)
    m_LifeTime += DT;
    
    if (m_LifeTime > s_MAX_LIFETIME) // End of life
    {
        m_LifeTime = 0.f;
        GetOwner()->SetActive(false);
        return;
    }
    
    // Handle Spawning Rocket smoke
    m_SmokeSpawnTime += DT;
    if (m_SmokeSpawnTime >= s_SMOKE_SPAWN_INTERVAL)
    {
        m_SmokeSpawnTime -= s_SMOKE_SPAWN_INTERVAL;
        GameObject* Object = GM->GetEffectPooler(EFFECT_POOLER_TYPE::ROCKET_SMOKE_POOLER)->SpawnObject();
        if (Object)
        {
            Object->Transform()->SetRelativePosX(Transform()->GetRelativePosX());
            Object->Transform()->SetRelativePosY(Transform()->GetRelativePosY());
            Object->FlipbookRender()->Play(0, 20.f, 1);
        }
    }
    
}

void CRocketProjectile::BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // TODO : LifeTime 제외시키기
    m_LifeTime = 0.f;
    GetOwner()->SetActive(false);
}

void CRocketProjectile::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_Speed, sizeof(float), 1, _File);
}

void CRocketProjectile::LoadFromLevelFile(FILE* _File)
{
    fread(&m_Speed, sizeof(float), 1, _File);
}
