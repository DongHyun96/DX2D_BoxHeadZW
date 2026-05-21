#include "pch.h"
#include "CGameLogManager.h"

#include "GameEngine/05.GameObject/GameObjectRefHolder.h"
#include "Source/ScriptMgr.h"

CGameLogManager::CGameLogManager()
    : CScript(SCRIPT_TYPE::GAMELOGMANAGER)
{
}

CGameLogManager::~CGameLogManager()
{
}

void CGameLogManager::Init()
{
    CScript::Init();
    
    m_vecGameLogs.resize(4);
    
    // AddScriptParam(SCRIPT_PARAM::GAME_OBJ_REF_HOLDER, &(m_vecGameLogs[0]), L"GameRefHolderTest", false);
    AddScriptParam(SCRIPT_PARAM::VEC_GAME_OBJ_REF_HOLDER, &m_vecGameLogs, L"VecGameLogs", false);
}

void CGameLogManager::Tick()
{
}

void CGameLogManager::SaveToLevelFile(FILE* _File)
{
}

void CGameLogManager::LoadFromLevelFile(FILE* _File)
{
}
