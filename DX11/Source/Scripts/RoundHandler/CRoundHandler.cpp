#include "pch.h"
#include "CRoundHandler.h"

#include "Source/ScriptMgr.h"

CRoundHandler::CRoundHandler()
    : CScript(SCRIPT_TYPE::ROUNDHANDLER)
{
}

CRoundHandler::~CRoundHandler()
{
}

void CRoundHandler::Tick()
{
    HandleTransition();
}

void CRoundHandler::HandleTransition()
{
    switch (m_RoundState)
    {
    case ROUND_STATE::WAIT:
    {
        
    }
        break;
    case ROUND_STATE::ROUND_GOING:
        break;
    case ROUND_STATE::GAME_OVER:
        break;
    }
}

void CRoundHandler::SetRoundState(ROUND_STATE _RoundState)
{
    m_RoundState = _RoundState;
    
    
}

void CRoundHandler::SaveToLevelFile(FILE* _File)
{
}

void CRoundHandler::LoadFromLevelFile(FILE* _File)
{
}
