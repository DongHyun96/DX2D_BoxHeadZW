#include "pch.h"
#include "TurretAttackStrategy.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/08.CollisionMgr/CollisionMgr.h"
#include "GameEngine/04.Asset/10.Sound/ASound.h"

float Turret_MGAttackStrategy::s_BurstInterval = 0.1f;

void Turret_MGAttackStrategy::WaitAttack()
{
    // 사격 요청이 들어왔을 때 첫 발 바로 사격 가능하도록 처리
    m_BurstFireTimer = s_BurstInterval;
    m_FireCount = 0;
}

bool Turret_MGAttackStrategy::UseAttackStrategy(GameObject* _Target)
{
    m_BurstFireTimer += DT;
    
    if (m_BurstFireTimer < s_BurstInterval) return false;
    m_BurstFireTimer = 0.f;
    
    // 점사 대기 시간 끝, 사격 처리
    // 사격 소리가 3가지가 있음
    const int randomSuffix = GetRandom(1, 3);
    wstring SoundKey = L"Sound\\MGShot" + to_wstring(randomSuffix) + L".wav";
    Ptr<ASound> ShootingSound = FIND_ASSET(ASound, SoundKey);
    ShootingSound->Play(1, 0.75f, true);
    
    // Target을 향하는 방향으로 Ray Casting
    Ray2D Ray{};
    //Ray.Origin      = _MuzzleWorldPos; // TODO : 총구 위치도 알긴 알아야 함 ㅅㅂ
    //Ray.Direction   = ToVec3(_FireDirection.Normalized()); 
    Ray.MaxDistance = RESOL_DIAG_LENGTH;
    
    
    // TODO : 총구 화염 및 effect 처리할 것
    

    // 3회 사격이 끝나면 이번 턴 공격 종료
    return ++m_FireCount >= 3;
}

bool Turret_MortarAttackStrategy::UseAttackStrategy(GameObject* _Target)
{
    return true;
}

bool Turret_RocketAttackStrategy::UseAttackStrategy(GameObject* _Target)
{
    return true;
}
