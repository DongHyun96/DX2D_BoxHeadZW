#include "pch.h"
#include "CAirStrike.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CCamMoveScript.h"
#include "Source/Scripts/StatScript/CStatScript.h"

ASound* CAirStrike::s_AirStrikeSound{};
bool CAirStrike::s_IsAirStrikeSpawned{};

const float CAirStrike::EFFECT_SPAWN_INTERVAL = 0.125f; // Effect 스폰 Interval


CAirStrike::CAirStrike()
    : CScript(SCRIPT_TYPE::AIRSTRIKE)
{
    m_ExplosionSpawnDesc.SpawnPos                   = Vec3();
    m_ExplosionSpawnDesc.ExplosionSizeFactor        = 1.25f;
    m_ExplosionSpawnDesc.FPS                        = 50.f;
    m_ExplosionSpawnDesc.DamageAmount               = 0.f;
    m_ExplosionSpawnDesc.SpawnedBy                  = nullptr;
    m_ExplosionSpawnDesc.UseCollisionForDamaging    = false;
    m_ExplosionSpawnDesc.PlayExplosionSound         = false;
    m_ExplosionSpawnDesc.UpwardVelocity             = Vec2();
    m_ExplosionSpawnDesc.DamagePulseDelaySec        = 0.06f;
    m_ExplosionSpawnDesc.DamagePulseDurationSec     = 0.04f;
    m_ExplosionSpawnDesc.DamagePulseSpriteIdx       = 2;
    m_ExplosionSpawnDesc.SecondaryBurstCount        = 0;
}

CAirStrike::~CAirStrike()
{
    s_IsAirStrikeSpawned = false;
}

void CAirStrike::Init()
{
    AddScriptParam(SCRIPT_PARAM::SOUND, &s_AirStrikeSound, L"AirStrikeSound");
}

void CAirStrike::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CAirStrike::BeginOverlap);

    ColliderCircle()->SetRadius(0.f); // 첫 Radius 반경

    // Sound 플레이
    if (!s_AirStrikeSound) s_AirStrikeSound = FIND_ASSET(ASound, L"Sound\\AirStrike.mp3").Get();
    s_AirStrikeSound->Play(1, 0.5f, true);
    
    // Flipbook 재생 (반짝이)
    FlipbookRender()->Play(L"AirStrike", 0, 20.f, -1);
    
    s_IsAirStrikeSpawned = true;
    
    // Cam effect
    Ptr<CCamMoveScript> CamMove = RenderMgr::GetInst()->GetPOVCam()->GetOwner()->GetScriptComponent<CCamMoveScript>();
    if (CamMove)
    {
        CamMove->SetAirStriker(this);
        CamMove->SetOrthoScaleLerpData(0.35f, 2.5f);
    }
    
    // TimeScale 영향 받지 않게끔 설정
    GetOwner()->SetIgnoreGlobalTimeScale(true);
}

void CAirStrike::Tick()
{
    // 한 3.5초 정도 기다린 뒤, Effect 스폰 및 Attack Collision 키우기
    m_Timer += DT;
    
    TickWaitAirStrike();
    
    if (m_Timer < 3.5f) return;
    
    Ptr<CCamMoveScript> CamMove = RenderMgr::GetInst()->GetPOVCam()->GetOwner()->GetScriptComponent<CCamMoveScript>();
    TickAirStriking(CamMove);
    
    // if (m_Timer < 8.f) return;
    if (m_Timer < 5.5f) return;
    
    // AirStrike 종료 지점
    TickAirStrikeFinish(CamMove);
}

void CAirStrike::TickWaitAirStrike()
{
    // Time Slow + Sound pitch 하강
    // Strike Sound는 예외처리 해둠
    const float TimeAndPitchScale = MappingToNewRange(m_Timer, 0.f, 3.5f, 1.f, 0.1f);
    AssetMgr::GetInst()->SetGlobalSoundPitch(TimeAndPitchScale);
    TimeMgr::GetInst()->SetTimeScale(TimeAndPitchScale);
}

void CAirStrike::TickAirStriking(const Ptr<CCamMoveScript>& CamMove)
{
    // Sound Pitch 및 TimeScale 설정
    const float SoundPitchScale = MappingToNewRange(m_Timer, 3.5f, 5.5f, 0.1f, 1.f);
    AssetMgr::GetInst()->SetGlobalSoundPitch(SoundPitchScale);
    TimeMgr::GetInst()->SetTimeScale(SoundPitchScale);
    
    GetOwner()->SetVisible(false);

    // Attack 반경 점점 늘려가면서, 가장자리에 Explosion Effect 스폰 처리
    if (CamMove) CamMove->SetOrthoScaleLerpData(2.5f, 1.5f);
        
    static const float RAD_SPEED = 750.f;
    
    float Radius = ColliderCircle()->GetRadius();
    Radius += DT * RAD_SPEED;
    ColliderCircle()->SetRadius(Radius);
    
    m_EffectTimer += DT;
    if (m_EffectTimer > EFFECT_SPAWN_INTERVAL)
    {
        m_EffectTimer -= EFFECT_SPAWN_INTERVAL;
        UINT EffectCount = MappingToNewRange(m_Timer, 3.5f, 5.5f, 10.f, 60.f);
        
        for (int i = 0 ; i < EffectCount; i++)
        {
            float Angle = XM_2PI / EffectCount;
            float x = Radius * cosf(i * Angle) + Transform()->GetWorldPos().x;
            float y = Radius * sinf(i * Angle) + Transform()->GetWorldPos().y;

            // Damage 처리를 False로 둠 + SecondaryBurstCount(2차 폭발) 0으로 고정
            
            m_ExplosionSpawnDesc.SpawnPos       = Vec3(x, y, y);
            m_ExplosionSpawnDesc.UpwardVelocity = Vec2(GetRandom(-0.05f, 0.05f), GetRandom(0.2f, 0.4f));
            GM->SpawnExplosion(m_ExplosionSpawnDesc);
        }
        
        // 터지는 소리 한번만 재생
        Ptr<ASound> Sound = FIND_ASSET(ASound, L"Sound\\Explosion1.mp3");
        Sound->Play(1, 0.5f, true);
    }
    
    
    // TODO : 카메라 Effect
}

void CAirStrike::TickAirStrikeFinish(const Ptr<CCamMoveScript>& CamMove)
{
    s_IsAirStrikeSpawned = false;
    if (CamMove)
    {
        CamMove->SetOrthoScaleLerpData(1.f, 5.f);
        CamMove->SetAirStriker(nullptr);
    }
    AssetMgr::GetInst()->SetGlobalSoundPitch(1.f);
    TimeMgr::GetInst()->SetTimeScale(1.f);
    
    Destroy();
}

void CAirStrike::BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // TODO : 실질적인 Damage 처리
    if (Ptr<CStatScript> Stat = _OtherCollider->GetOwner()->GetScriptComponent<CStatScript>())
        Stat->TakeDamage(5000.f, GetOwner());
}
