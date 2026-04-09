#include "pch.h"
#include "CFlameLineHandler.h"

#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

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
    static float Timer{};
    static bool Fired{};

    if (KEY_TAP(KEY::MRB))
        GM->SpawnFirePillar(Vec3(0.f, 0.f, 0.f), 0.f);
}

void CFlameLineHandler::MakeFlameLine(const Vec2& _Direction, float _Length, float _Damage)
{
    
}
