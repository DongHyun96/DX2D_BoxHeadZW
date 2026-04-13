#include "pch.h"
#include "CPlayerWeaponHandler.h"

#include "CPlayerScript.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/04.Asset/09.Prefab/APrefab.h"
#include "InvenScript/CEquipmentScript.h"
#include "InvenScript/CInvenScript.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/AirStrike/CAirStrike.h"
#include "Source/Scripts/ProjectileScript/CGrenade.h"
#include "Source/Scripts/UIScript/CCrossHair.h"
#include "Source/Scripts/UIScript/InGameUIManager/CIngameUIManager.h"

CPlayerWeaponHandler::CPlayerWeaponHandler()
    : CScript(SCRIPT_TYPE::PLAYERWEAPONHANDLER)
{
    // TODO : 추후 게임 불러오기 및 저장하기 까지 할꺼면 이 데이터 저장할 것
    m_mapCurrentMastery = 
    {
        { PLAYER_HANDSTATE::PISTOL,     WeaponMasteryData(WEAPON_MASTERY::BEGINNER, 0.f) },    
        { PLAYER_HANDSTATE::UZI,        WeaponMasteryData(WEAPON_MASTERY::BEGINNER, 0.f) },    
        { PLAYER_HANDSTATE::SHOTGUN,    WeaponMasteryData(WEAPON_MASTERY::BEGINNER, 0.f) },    
        { PLAYER_HANDSTATE::MINIGUN,    WeaponMasteryData(WEAPON_MASTERY::BEGINNER, 0.f) },    
        { PLAYER_HANDSTATE::ROCKET,     WeaponMasteryData(WEAPON_MASTERY::BEGINNER, 0.f) },    
    };
}

CPlayerWeaponHandler::~CPlayerWeaponHandler()
{
    // 여기서 관련된 애들 저장하기
}

void CPlayerWeaponHandler::Init()
{
    for (pair<const PLAYER_HANDSTATE, MuzzleOffsets>& Pair : m_mapEachMuzzleOffsets)
    {
        UINT i{};
        
        wstring HandDesc{};

        switch (Pair.first)
        {
        case PLAYER_HANDSTATE::PISTOL:      HandDesc = L"PISTOL ";  break;
        case PLAYER_HANDSTATE::UZI:         HandDesc = L"UZI ";     break;
        case PLAYER_HANDSTATE::SHOTGUN:     HandDesc = L"SHOTGUN "; break;
        case PLAYER_HANDSTATE::MINIGUN:     HandDesc = L"MINIGUN "; break;
        case PLAYER_HANDSTATE::ROCKET:      HandDesc = L"ROCKET ";  break;
        case PLAYER_HANDSTATE::UNARMED: case PLAYER_HANDSTATE::END: continue;
        }
        
        for (Vec2& vec : Pair.second)
        {
            wstring DirDesc{};
            switch (i)
            {
            case 0: DirDesc = L"Right";        break;
            case 1: DirDesc = L"RightUp";      break;
            case 2: DirDesc = L"Up";           break;
            case 3: DirDesc = L"UpLeft";       break;
            case 4: DirDesc = L"Left";         break;
            case 5: DirDesc = L"LeftDown";     break;
            case 6: DirDesc = L"Down";         break;
            case 7: DirDesc = L"DownRight";    break;
            }
            
            
            AddScriptParam(SCRIPT_PARAM::VEC2, vec, HandDesc + DirDesc, false, 0.5f);
            ++i;
        }
    }
}

void CPlayerWeaponHandler::Begin()
{
    m_PlayerMainScript = GetOwner()->GetScriptComponent<CPlayerScript>().Get();
    m_EquipmentScript = GetOwner()->GetScriptComponent<CEquipmentScript>().Get();

    m_AirStrikePrefab = FIND_ASSET(APrefab, L"Prefab\\AirStrikePrefab.pref");
}

void CPlayerWeaponHandler::Tick()
{
    if (m_PlayerMainScript->GetMainState() == PLAYER_MAINSTATE::DIE) return;
    
    TickSwapWeapon();
    TickFireWeapon();
    TickFireGrenade();
    TickDeployAirStrike();

    // TODO : 여기 지우기 & 버프 처리 시, Weapon에 실질적으로 setting을 해주어야 함(지금 무기를 바꿀때만 처리가 되는 중)
    if (KEY_TAP(KEY::MOUSE_X1))
    {
        if (Ptr<CWeaponScript> Weapon = m_EquipmentScript->GetEquippedWeapon(m_PlayerMainScript->GetHandState()))
        {
            int StateIdx = static_cast<int>(m_mapCurrentMastery[m_PlayerMainScript->GetHandState()].CurrentMasteryState);
            if (--StateIdx >= 0)
            {
                m_mapCurrentMastery[m_PlayerMainScript->GetHandState()].CurrentMasteryState = static_cast<WEAPON_MASTERY>(StateIdx);
                SetHandState(m_PlayerMainScript->GetHandState());   
            }
        }
    }
    
    if (KEY_TAP(KEY::MOUSE_X2))
    {
        if (Ptr<CWeaponScript> Weapon = m_EquipmentScript->GetEquippedWeapon(m_PlayerMainScript->GetHandState()))
        {
            int StateIdx = static_cast<int>(m_mapCurrentMastery[m_PlayerMainScript->GetHandState()].CurrentMasteryState);
            if (++StateIdx <= 2)
            {
                m_mapCurrentMastery[m_PlayerMainScript->GetHandState()].CurrentMasteryState = static_cast<WEAPON_MASTERY>(StateIdx);
                SetHandState(m_PlayerMainScript->GetHandState());   
            }
        }
    }
    
    // TODO : 여기 지우기 (Muzzle 위치 디버깅)   
    const PLAYER_HANDSTATE CurrentHandState = m_PlayerMainScript->GetHandState();
    const int DirectionIndex = static_cast<int>(m_PlayerMainScript->GetCurrentFacedDirection());
    const Vec3 MuzzleWorldPos = Transform()->GetWorldPos() + m_mapEachMuzzleOffsets[CurrentHandState][DirectionIndex];
    DrawDebugCircle(MuzzleWorldPos, 5.f, DEF_COLOR_RED, 0.f);
}

void CPlayerWeaponHandler::TickSwapWeapon()
{
    // 사격 중이라면 무기 교환 불가
    if (m_LastTickFired) return;

    // KEY_TAP(KEY::E)     ? PLAYER_HANDSTATE::UNARMED :
    static PLAYER_HANDSTATE PrevGunState{};
    if (KEY_TAP(KEY::E))
    {
        if (m_HandState == PLAYER_HANDSTATE::UNARMED)
        {
            SetHandState(PrevGunState);
        }
        else
        {
            PrevGunState = m_HandState;
            SetHandState(PLAYER_HANDSTATE::UNARMED);
        }
        return;   
    }
    
    // Numbering으로 무기 전환
    PLAYER_HANDSTATE NextHandState =    KEY_TAP(KEY::NUM_1) ? PLAYER_HANDSTATE::PISTOL : 
                                        KEY_TAP(KEY::NUM_2) ? PLAYER_HANDSTATE::UZI :
                                        KEY_TAP(KEY::NUM_3) ? PLAYER_HANDSTATE::SHOTGUN :
                                        KEY_TAP(KEY::NUM_4) ? PLAYER_HANDSTATE::MINIGUN :
                                        KEY_TAP(KEY::NUM_5) ? PLAYER_HANDSTATE::ROCKET : PLAYER_HANDSTATE::END;
                                        

    
    if (m_HandState != PLAYER_HANDSTATE::UNARMED)
    {
        // 마우스 휠로 무기 전환 (PISTOL ~ ROCKET 범위) (설치물 상태에서는 x)
        int wheel = KeyMgr::GetInst()->GetMouseWheel();
        if (NextHandState == PLAYER_HANDSTATE::END && wheel != 0)
        {
            if (m_HandState == PLAYER_HANDSTATE::UNARMED)
            {
                // UNARMED 상태에서 휠을 돌리면 PISTOL로 진입
                NextHandState = PLAYER_HANDSTATE::PISTOL;
            }
            else
            {
                int iCur = static_cast<int>(m_HandState);
                int iStart = static_cast<int>(PLAYER_HANDSTATE::PISTOL);
                int iEnd = static_cast<int>(PLAYER_HANDSTATE::ROCKET);

                if (wheel > 0) // Wheel Up -> 다음 무기
                {
                    iCur++;
                    if (iCur > iEnd) iCur = iStart;
                }
                else // Wheel Down -> 이전 무기
                {
                    iCur--;
                    if (iCur < iStart) iCur = iEnd;
                }
                NextHandState = static_cast<PLAYER_HANDSTATE>(iCur);
            }
        }
    }
    
    if (NextHandState == PLAYER_HANDSTATE::END) return; // 아무 무기 Swap 시도도 이루어지지 않음
    
    SetHandState(NextHandState);
}

void CPlayerWeaponHandler::TickFireWeapon()
{
    if (KEY_RELEASED(KEY::MLB))
    {
        Ptr<CWeaponScript> Weapon = m_EquipmentScript->GetEquippedWeapon(m_PlayerMainScript->GetHandState());
        if (m_LastTickFired && Weapon)
        {
            Weapon->OnFireReleased();
        }
        m_LastTickFired = false;
        return;
    }
    
    if (!KEY_PRESSED(KEY::MLB)) return;
    
    const PLAYER_HANDSTATE CurrentHandState = m_PlayerMainScript->GetHandState();

    const int DirectionIndex = static_cast<int>(m_PlayerMainScript->GetCurrentFacedDirection());
    const Vec2 MuzzleWorldPos = ToVec2(Transform()->GetWorldPos()) + m_mapEachMuzzleOffsets[CurrentHandState][DirectionIndex];
    
    if (Ptr<CWeaponScript> Weapon = m_EquipmentScript->GetEquippedWeapon(m_PlayerMainScript->GetHandState()))
    {
        const Vec2 MousePos = ToVec2(KeyMgr::GetInst()->GetMouseWorldPos());
        if (Weapon->Fire(MuzzleWorldPos, MousePos - MuzzleWorldPos))
            m_PlayerMainScript->GetOwner()->GetScriptComponent<CInvenScript>()->ReduceCurrentAmmoCount(CurrentHandState);
        m_LastTickFired = true;
    }
}

void CPlayerWeaponHandler::TickFireGrenade()
{
    if (KEY_TAP(KEY::C))
        GM->SpawnGrenade(Transform()->GetWorldPos(), m_PlayerMainScript->GetPlayerToMousePos().Normalized(), 75.f, 3, 400.f, 300.f, true);
}

void CPlayerWeaponHandler::TickDeployAirStrike()
{
    if (KEY_TAP(KEY::Q))
    {
        if (CAirStrike::HasAirStrikeSpawnedAlive()) return;
        GameObject* AirStriker = m_AirStrikePrefab->InstantiateAndSpawnToCurLevel();
        AirStriker->Transform()->SetRelativePos(Transform()->GetWorldPos() - Vec3::UnitY * 30.f);
    }
}

void CPlayerWeaponHandler::SetHandState(PLAYER_HANDSTATE _HandState)
{
    m_HandState = _HandState;

    if (m_HandState == PLAYER_HANDSTATE::UNARMED)
    {
        GM->GetIngameUIManager()->GetCrossHair()->GetOwner()->SetActive(false);
    }
    
    // 해당 Slot에 무기가 존재한다면
    if (Ptr<CWeaponScript> Weapon = m_EquipmentScript->GetEquippedWeapon(m_HandState))
    {
        WEAPON_MASTERY NextWeaponMasteryState = m_mapCurrentMastery[m_HandState].CurrentMasteryState;
        const WeaponMasteryBuff& BuffData = EACH_WEAPON_MASTERY_BUFF.at(m_HandState).at(static_cast<int>(NextWeaponMasteryState));
        
        const float FireInterval = 60.f / BuffData.FireRPM;
        Weapon->SetFireIntervalTime(FireInterval);
        Weapon->SetDamageAmountPerRound(BuffData.DamageAmountPerRound);
        
        // UI 업데이트
        CInvenScript* Inven = GM->GetPlayerObject()->GetScriptComponent<CInvenScript>().Get();
        
        GM->GetIngameUIManager()->GetAmmoCountUIAreaRef().UpdateToGun(m_HandState, Inven->GetCurrentAmmoCount(_HandState));
        GM->GetIngameUIManager()->GetCrossHair()->GetOwner()->SetActive(true);
    }
}

const Vec2& CPlayerWeaponHandler::GetCurrentMuzzleOffset()
{
    const MuzzleOffsets& CurrentMuzzleOffsets = m_mapEachMuzzleOffsets[m_PlayerMainScript->GetHandState()];
    return CurrentMuzzleOffsets.at(static_cast<int>(m_PlayerMainScript->GetCurrentFacedDirection()));
}

void CPlayerWeaponHandler::SaveToLevelFile(FILE* _File)
{
    // m_mapEachMuzzleOffset 개수 저장
    UINT mapCount = m_mapEachMuzzleOffsets.size();
    fwrite(&mapCount, sizeof(UINT), 1, _File);

    for (const auto& Pair : m_mapEachMuzzleOffsets)
    {
        // PlayerHandState 정보 저장
        UINT HandStateInt = static_cast<UINT>(Pair.first);
        fwrite(&HandStateInt, sizeof(UINT), 1, _File);
        
        // 배열 정보 저장
        for (UINT i = 0; i < static_cast<UINT>(EDIRECTION::END); ++i)
            fwrite(&(Pair.second[i]), sizeof(Vec2), 1, _File);
    }
}

void CPlayerWeaponHandler::LoadFromLevelFile(FILE* _File)
{
    // m_mapEachMuzzleOffset 개수 불러오기
    UINT mapCount{};
    fread(&mapCount, sizeof(UINT), 1, _File);
    
    for (UINT i = 0; i < mapCount; ++i)
    {
        // PlayerHandState 정보 복원
        UINT HandStateInt{};
        fread(&HandStateInt, sizeof(UINT), 1, _File);
        PLAYER_HANDSTATE HandState = static_cast<PLAYER_HANDSTATE>(HandStateInt);
        
        MuzzleOffsets muzzleOffsets{};
        
        // 배열 정보 불러오기
        for (UINT i = 0; i < static_cast<UINT>(EDIRECTION::END); ++i)
            fread(&muzzleOffsets[i], sizeof(Vec2), 1, _File);

        // map에 정보 저장
        m_mapEachMuzzleOffsets[HandState] = muzzleOffsets;
    }
}
