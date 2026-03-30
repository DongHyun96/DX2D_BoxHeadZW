#include "pch.h"
#include "CCharacterAnimHandler.h"

#include "Source/ScriptMgr.h"

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
