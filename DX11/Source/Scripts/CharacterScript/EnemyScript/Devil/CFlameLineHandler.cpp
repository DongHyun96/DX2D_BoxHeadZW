#include "pch.h"
#include "CFlameLineHandler.h"

#include "Source/ScriptMgr.h"

CFlameLineHandler::CFlameLineHandler()
    : CScript(SCRIPT_TYPE::FLAMELINEHANDLER)
{
}

CFlameLineHandler::~CFlameLineHandler()
{
}

void CFlameLineHandler::Begin()
{
    m_AttackCollider = GetCollider2D();
}

void CFlameLineHandler::Tick()
{
}

void CFlameLineHandler::MakeFlameLine(const Vec2& _Direction, float _Length, float _Damage)
{
    
}
