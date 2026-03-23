#include "pch.h"
#include "CPlayerSwapWeapon.h"

#include "CPlayerScript.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"

CPlayerSwapWeapon::CPlayerSwapWeapon()
    : CScript(SCRIPT_TYPE::PLAYERSWAPWEAPON)
{
}

CPlayerSwapWeapon::~CPlayerSwapWeapon()
{
}

void CPlayerSwapWeapon::Init()
{
}

void CPlayerSwapWeapon::Begin()
{
    m_PlayerMainScript = GetOwner()->GetScriptComponent<CPlayerScript>().Get();
}

void CPlayerSwapWeapon::Tick()
{
    if (KEY_TAP(KEY::NUM_1)) m_PlayerMainScript->SetHandState(PLAYER_HANDSTATE::PISTOL);
    if (KEY_TAP(KEY::NUM_2)) m_PlayerMainScript->SetHandState(PLAYER_HANDSTATE::UZI);
    if (KEY_TAP(KEY::NUM_3)) m_PlayerMainScript->SetHandState(PLAYER_HANDSTATE::SHOTGUN);
    if (KEY_TAP(KEY::NUM_4)) m_PlayerMainScript->SetHandState(PLAYER_HANDSTATE::MINIGUN);
    if (KEY_TAP(KEY::NUM_5)) m_PlayerMainScript->SetHandState(PLAYER_HANDSTATE::ROCKET);
    if (KEY_TAP(KEY::NUM_6)) m_PlayerMainScript->SetHandState(PLAYER_HANDSTATE::UNARMED);
}
