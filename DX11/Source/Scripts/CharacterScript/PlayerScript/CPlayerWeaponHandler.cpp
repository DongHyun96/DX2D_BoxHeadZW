#include "pch.h"
#include "CPlayerWeaponHandler.h"

#include "CPlayerScript.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "InvenScript/CEquipmentScript.h"
#include "Source/ScriptMgr.h"

CPlayerWeaponHandler::CPlayerWeaponHandler()
    : CScript(SCRIPT_TYPE::PLAYERWEAPONHANDLER)
{
    // TODO : 추후 게임 불러오기 및 저장하기 까지 할꺼면 이 데이터 저장할 것
    m_mapCurrentMastery = 
    {
        { PLAYER_HANDSTATE::PISTOL,     WeaponMasteryData(WEAPON_MASTERY::MASTER, 0.f) },    
        { PLAYER_HANDSTATE::UZI,        WeaponMasteryData() },    
        { PLAYER_HANDSTATE::SHOTGUN,    WeaponMasteryData() },    
        { PLAYER_HANDSTATE::MINIGUN,    WeaponMasteryData() },    
        { PLAYER_HANDSTATE::ROCKET,     WeaponMasteryData() },    
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
}

void CPlayerWeaponHandler::Tick()
{
    TickSwapWeapon();
    TickFireWeapon();
    
    // TODO : 여기 지우기 (Muzzle 위치 디버깅)   
    const PLAYER_HANDSTATE CurrentHandState = m_PlayerMainScript->GetHandState();
    const int DirectionIndex = static_cast<int>(m_PlayerMainScript->GetCurrentFacedDirection());
    const Vec3 MuzzleWorldPos = Transform()->GetWorldPos() + m_mapEachMuzzleOffsets[CurrentHandState][DirectionIndex];
    DrawDebugCircle(MuzzleWorldPos, 5.f, DEF_COLOR_RED, 0.f);
}

void CPlayerWeaponHandler::TickSwapWeapon()
{
    if (KEY_TAP(KEY::TILDE))
    {
        m_PlayerMainScript->SetHandState(PLAYER_HANDSTATE::UNARMED);
        return;
    }
    
    PLAYER_HANDSTATE NextHandState = KEY_TAP(KEY::NUM_1) ? PLAYER_HANDSTATE::PISTOL :
                                     KEY_TAP(KEY::NUM_2) ? PLAYER_HANDSTATE::UZI :
                                     KEY_TAP(KEY::NUM_3) ? PLAYER_HANDSTATE::SHOTGUN :
                                     KEY_TAP(KEY::NUM_4) ? PLAYER_HANDSTATE::MINIGUN :
                                     KEY_TAP(KEY::NUM_5) ? PLAYER_HANDSTATE::ROCKET : PLAYER_HANDSTATE::END;
    
    if (NextHandState == PLAYER_HANDSTATE::END) return; // 아무 무기 Swap 시도도 이루어지지 않음

    // 해당 Slot에 무기가 존재한다면
    if (Ptr<CWeaponScript> Weapon = m_EquipmentScript->GetEquippedWeapon(NextHandState))
    {
        m_PlayerMainScript->SetHandState(NextHandState);

        WEAPON_MASTERY NextWeaponMasteryState = m_mapCurrentMastery[NextHandState].CurrentMasteryState;
        const WeaponMasteryBuff& BuffData = EACH_WEAPON_MASTERY_BUFF.at(NextHandState).at(static_cast<int>(NextWeaponMasteryState));
        
        const float FireInterval = 60.f / BuffData.FireRPM;
        Weapon->SetFireIntervalTime(FireInterval);
        Weapon->SetDamageAmountPerRound(BuffData.DamageAmountPerRound);
    }
}

void CPlayerWeaponHandler::TickFireWeapon()
{
    if (!KEY_PRESSED(KEY::MLB)) return;
    
    const PLAYER_HANDSTATE CurrentHandState = m_PlayerMainScript->GetHandState();

    const int DirectionIndex = static_cast<int>(m_PlayerMainScript->GetCurrentFacedDirection());
    const Vec2 MuzzleWorldPos = ToVec2(Transform()->GetWorldPos()) + m_mapEachMuzzleOffsets[CurrentHandState][DirectionIndex];
    
    if (Ptr<CWeaponScript> Weapon = m_EquipmentScript->GetEquippedWeapon(m_PlayerMainScript->GetHandState()))
        Weapon->Fire(MuzzleWorldPos, m_PlayerMainScript->GetPlayerToMousePos());
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
