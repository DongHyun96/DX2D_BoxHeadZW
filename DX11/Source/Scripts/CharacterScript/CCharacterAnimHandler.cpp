#include "pch.h"
#include "CCharacterAnimHandler.h"

#include "CCharacterScript.h"
#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Scripts/StatScript/CStatScript.h"

CCharacterAnimHandler::CCharacterAnimHandler(enum SCRIPT_TYPE _ScriptType)
    : CScript(_ScriptType)
{
}

CCharacterAnimHandler::~CCharacterAnimHandler()
{
}

void CCharacterAnimHandler::Init()
{
}

void CCharacterAnimHandler::Begin()
{
}

void CCharacterAnimHandler::Tick()
{
    UpdateAnimTransition();
}
