#include "pch.h"
#include "CAirStrike.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/CCamMoveScript.h"

ASound* CAirStrike::s_AirStrikeSound{};
bool CAirStrike::s_IsAirStrikeSpawned{};

const float CAirStrike::EFFECT_SPAWN_INTERVAL = 0.125f; // Effect 스폰 Interval


CAirStrike::CAirStrike()
    : CScript(SCRIPT_TYPE::AIRSTRIKE)
{
}

CAirStrike::~CAirStrike()
{
    s_IsAirStrikeSpawned = false;
}

void CAirStrike::Init()
{
    AddScriptParam(SCRIPT_PARAM::SOUND, &s_AirStrikeSound, L"AirStirkeSound");
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
        CamMove->SetOrthoScaleLerpData(0.35f, 5.f);
    }
}

void CAirStrike::Tick()
{
    // 한 3.5초 정도 기다린 뒤, Effect 스폰 및 Attack Collision 키우기
    m_Timer += DT;
    
    if (m_Timer < 3.5f) return;
    GetOwner()->SetVisible(false);

    // Attack 반경 점점 늘려가면서, 가장자리에 Explosion Effect 스폰 처리
    Ptr<CCamMoveScript> CamMove = RenderMgr::GetInst()->GetPOVCam()->GetOwner()->GetScriptComponent<CCamMoveScript>();
    if (CamMove) CamMove->SetOrthoScaleLerpData(2.5f, 1.5f);
        
    static const float RAD_SPEED = 750.f;
    
    float Radius = ColliderCircle()->GetRadius();
    Radius += DT * RAD_SPEED;
    ColliderCircle()->SetRadius(Radius);
    
    DebugUtil::AddDebugLog(to_string(Radius), DEF_COLOR_CYAN, 0.f);

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

            // Damage 처리를 False로 둠
            GM->SpawnExplosion({x, y, y}, 1.25f, 1500.f, 0.f, nullptr, false, false, {GetRandom(-0.05f, 0.05f), GetRandom(0.2f, 0.4f)});
        }
        
        // 터지는 소리 한번만 재생
        Ptr<ASound> Sound = FIND_ASSET(ASound, L"Sound\\Explosion1.mp3");
        Sound->Play(1, 0.5f, true);
    }
    
    // TODO : 카메라 Effect
    
    // if (m_Timer < 8.f) return;
    if (m_Timer < 5.5f) return;
    
    // AirStrike 종료 지점
    s_IsAirStrikeSpawned = false;
    if (CamMove)
    {
        CamMove->SetOrthoScaleLerpData(1.f, 5.f);
        CamMove->SetAirStriker(nullptr);
    }
    Destroy();
}

void CAirStrike::BeginOverlap(CCollider2D* _OwnerCollider, CCollider2D* _OtherCollider)
{
    // TODO : 실질적인 Damage 처리
}
