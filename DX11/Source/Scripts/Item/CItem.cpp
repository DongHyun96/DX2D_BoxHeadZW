#include "pch.h"
#include "CItem.h"

#include <algorithm>

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/06.Component/03.Collider2D/CCollider2D.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

#include "Source/Scripts/CharacterScript/CharacterStat/PlayerStat/CPlayerStat.h"
#include "Source/Scripts/CharacterScript/PlayerScript/InvenScript/CEquipmentScript.h"
#include "Source/Scripts/CharacterScript/PlayerScript/InvenScript/CInvenScript.h"
#include "Source/Scripts/UIScript/InGameUIManager/CIngameUIManager.h"

CItem::CItem()
    : CScript(SCRIPT_TYPE::ITEM)
    , m_LifeTime(10.f)
    , m_MaxLifeTime(10.f)
{
}

CItem::~CItem()
{
}

void CItem::AfterLevelBegin()
{
    CScript::AfterLevelBegin();
    
    // GM에 Owner PoolComponent 등록
    if (!GM->GetItemPooler())
    {
        // ObjectPooling 처리된 객체의 OwnerPooler에 한해서만 등록 처리를 진행시킴 (Testing 환경에서 Pooling 되지 않은 오브젝트가 있을 수 있음)
        if (CPoolComponent* PoolComponent = GetOwner()->GetOwnerPoolComponent())
            GM->SetItemPooler(PoolComponent);
    }
}

void CItem::Begin()
{
    ADD_DYNAMIC_BEGIN_OVERLAP(CItem::OnColliderBeginOverlap);
    m_DynamicMtrl = GetOwner()->GetRenderCom()->CreateDynamicMaterial();
}

void CItem::Tick()
{
    m_LifeTime -= DT;
    
    // 여기서 서서히 Render TintColor alpha 0으로
    float fAlpha = 1.f;
    if (m_MaxLifeTime > 0.001f) fAlpha = m_LifeTime / m_MaxLifeTime;
    fAlpha = clamp(fAlpha, 0.f, 1.f);

    Vec4 vColor = DEF_COLOR_WHITE;
    if (m_DynamicMtrl) vColor = m_DynamicMtrl->GetScalar<Vec4>(VEC4_0);
        
    vColor.w = fAlpha;
    
    if (m_DynamicMtrl) m_DynamicMtrl->SetScalar(VEC4_0, vColor);
    
    if (m_LifeTime < 0.f)
    {
        GetOwner()->SetActive(false);
        m_LifeTime = m_MaxLifeTime;
    }
}

void CItem::OnColliderBeginOverlap(CCollider2D* _Owner, CCollider2D* _Other)
{
    GameObject*         pOtherObj        = _Other->GetOwner();
    CPlayerStat*        pPlayerStat      = pOtherObj->GetScriptComponent<CPlayerStat>().Get();
    
    // Player가 아닌 다른 물체와의 충돌이 일어남
    if (!pPlayerStat) return;
    
    CEquipmentScript*   pEquipmentScript = pOtherObj->GetScriptComponent<CEquipmentScript>().Get();
    CInvenScript*       pInven           = pOtherObj->GetScriptComponent<CInvenScript>().Get();
    
    if (!m_EarnedSound) m_EarnedSound = FIND_ASSET(ASound, L"Sound\\PickUpAmmo.wav");
    m_EarnedSound->Play(1, 0.5f, true);

    
    m_LifeTime = m_MaxLifeTime;    
    GetOwner()->SetActive(false);

    // 새로운 무기종류 해금 : 확률 1%
    if (CheckProbabilityPercent(1.f))
    {
        // 모두 해금했는지 체크
        const PLAYER_HANDSTATE NextWeaponToUnlock = pEquipmentScript->GetNextWeaponTypeToUnlock();
        if (NextWeaponToUnlock != PLAYER_HANDSTATE::END)
        {
            if (pEquipmentScript->EquipWeapon(NextWeaponToUnlock))
            {
                switch (NextWeaponToUnlock)
                {
                case PLAYER_HANDSTATE::SHOTGUN: GM->GetIngameUIManager()->AddGameLog(L"SHOTGUN UNLOCKED");  return;
                case PLAYER_HANDSTATE::MINIGUN: GM->GetIngameUIManager()->AddGameLog(L"MINIGUN UNLOCKED");  return;
                case PLAYER_HANDSTATE::ROCKET:  GM->GetIngameUIManager()->AddGameLog(L"ROCKET UNLOCKED");   return;
                default: return;
                }                
            }
        }
    }

    // AirStrike : 확률 5%
    if (CheckProbabilityPercent(5.f))
    {
        const int IncreaseAmount = GetRandom(1, 2);
        pInven->IncreaseCurrentAirStrikeCount(IncreaseAmount);
        return;
    }

    // 나머지 94% 확률을 동일한 확률로 쪼개어 ApplyHeal, Inven Structure 개수 늘리기, 탄알 수 늘리기, Grenade Count 늘리기
    switch (GetRandom(0, 3))
    {
    case 0: // 1. Heal
        pPlayerStat->ApplyHeal(GetRandom(30.f, 60.f));
        return;
    case 1: // 2. 구조물 하나 선택해서 늘리기(CInvenScript)
    {
        const PLAYER_STRUCTURE_TYPE type = static_cast<PLAYER_STRUCTURE_TYPE>(rand() % static_cast<int>(PLAYER_STRUCTURE_TYPE::END));

        // Turret 종류
        switch (type)
        {
        case PLAYER_STRUCTURE_TYPE::BARRICADE:
        case PLAYER_STRUCTURE_TYPE::BARREL:
            pInven->IncreaseCurrentStructureCount(type, GetRandom(10, 20));
            break;
        case PLAYER_STRUCTURE_TYPE::TURRET_MACHINE_GUN: 
        case PLAYER_STRUCTURE_TYPE::TURRET_MORTAR: 
        case PLAYER_STRUCTURE_TYPE::TURRET_ROCKET:
            pInven->IncreaseCurrentStructureCount(type, GetRandom(3, 10));
            break;
        case PLAYER_STRUCTURE_TYPE::END:
            break;
        }
    }
        return;
    case 2: // 3. 탄알 수 늘리기(CInvenScript)
    {
        CInvenScript* pInven = pOtherObj->GetScriptComponent<CInvenScript>().Get();
        if (pInven)
        {
            // PISTOL(1)은 무한이므로 제외하고 UZI(2) ~ ROCKET(5) 중 선택 (END인 경우, GrenadeCount 늘리는 것으로 처리)
            const PLAYER_HANDSTATE WeaponType = static_cast<PLAYER_HANDSTATE>(rand() % 5 + 2);

            static const map<PLAYER_HANDSTATE, pair<int, int>> RANDOM_AMMO_AMOUNT_RANGE = 
            {
                {PLAYER_HANDSTATE::UZI,     {30, 60}},
                {PLAYER_HANDSTATE::SHOTGUN, {20, 40}},
                {PLAYER_HANDSTATE::MINIGUN, {50, 200}},
                {PLAYER_HANDSTATE::ROCKET,  {20, 30}},
                {PLAYER_HANDSTATE::END,     {5, 15}}, // Grenade
            };

            const int _Min               = RANDOM_AMMO_AMOUNT_RANGE.at(WeaponType).first;
            const int _Max               = RANDOM_AMMO_AMOUNT_RANGE.at(WeaponType).second;
            const int IncreaseAmmoAmount = GetRandom(_Min, _Max);

            if (WeaponType != PLAYER_HANDSTATE::END)
                pInven->IncreaseCurrentAmmoCount(WeaponType, IncreaseAmmoAmount);
            else pInven->IncreaseCurrentGrenadeCount(IncreaseAmmoAmount);
        }
    }
    }
}
