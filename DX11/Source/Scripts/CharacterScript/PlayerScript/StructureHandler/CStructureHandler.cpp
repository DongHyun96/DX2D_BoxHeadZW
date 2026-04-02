#include "pch.h"
#include "CStructureHandler.h"

#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"
#include "Source/Scripts/CharacterScript/PlayerScript/InvenScript/CInvenScript.h"

CStructureHandler::CStructureHandler()
    : CScript(SCRIPT_TYPE::STRUCTUREHANDLER)
{
}

CStructureHandler::~CStructureHandler()
{
}

void CStructureHandler::Begin()
{
    // Init Prefabs & Preview Objects
    
    
    m_MainPlayerScript  = GetOwner()->GetScriptComponent<CPlayerScript>().Get();
    m_InvenScript       = GetOwner()->GetScriptComponent<CInvenScript>().Get();
}

void CStructureHandler::Tick()
{
    if (m_MainPlayerScript->GetMainState() != PLAYER_MAINSTATE::IDLE)       return;
    if (m_MainPlayerScript->GetHandState() != PLAYER_HANDSTATE::UNARMED)    return;

    // TODO : 두 가지를 체크해야 함 -> Cell 위치가 Available한 Cell인지 & Preview Object와 Ovelapped 중인 물체가 있는지
    Vec2 MousePos = KeyMgr::GetInst()->GetMouseWorldPos2D();
    // GM->GetBackgroundCellManager()->IsCellAvailable(MousePos);
    
    
}
