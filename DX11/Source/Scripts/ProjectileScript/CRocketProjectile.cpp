#include "pch.h"
#include "CRocketProjectile.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/CharacterScript/CCharacterScript.h"
#include "Source/Scripts/StatScript/CStatScript.h"
#include "Source/Scripts/Structure/CBarrel.h"

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
    
    // Handle Boundary -> 넘어가면 사라지게끔 처리
    CBackgroundTile* BackgroundCellManager = GM->GetBackgroundCellManager();
    if (Pos.x + Transform()->GetRelativeScaleX() < -BackgroundCellManager->GetWorldSizeHalf() ||
        Pos.x - Transform()->GetRelativeScaleX()> BackgroundCellManager->GetWorldSizeHalf() ||
        Pos.y + Transform()->GetRelativeScaleY() < -BackgroundCellManager->GetWorldSizeHalf() ||
        Pos.y - Transform()->GetRelativeScaleY() > BackgroundCellManager->GetWorldSizeHalf())
    {
        DebugUtil::AddDebugLog("OutOfBound");
        GetOwner()->SetActive(false);
        return;
    }
    
    // Handle Spawning Rocket smoke
    m_SmokeSpawnTime += DT;
    if (m_SmokeSpawnTime >= s_SMOKE_SPAWN_INTERVAL)
    {
        m_SmokeSpawnTime -= s_SMOKE_SPAWN_INTERVAL;
        GM->SpawnRocketSmoke(Transform()->GetRelativePos());
    }
    
}

void CRocketProjectile::BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    GetOwner()->SetActive(false);
    
    // Effect Spawn 처리하기
    GM->SpawnExplosionDome(Transform()->GetWorldPos(), GetRandom(1.f, 1.2f));
    
    if (Ptr<CStatScript> Stat = _OtherCollider->GetOwner()->GetScriptComponent<CStatScript>())
    {
        // 여기서 Damage를 주면 안됨 / Barrel의 경우에만 연쇄폭발 첫 Barrel을 여기서 터트림
        if (_OtherCollider->GetOwner()->GetScriptComponent<CBarrel>())
            Stat->TakeDamage(m_Damage, ToVec2(Transform()->GetWorldPos()));
    }
}

void CRocketProjectile::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_Speed, sizeof(float), 1, _File);
}

void CRocketProjectile::LoadFromLevelFile(FILE* _File)
{
    fread(&m_Speed, sizeof(float), 1, _File);
}
