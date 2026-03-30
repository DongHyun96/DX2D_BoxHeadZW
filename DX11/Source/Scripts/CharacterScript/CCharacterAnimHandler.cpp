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

void CCharacterAnimHandler::RewindPushedOut(const Vec2& _PushedOutFaceDirection)
{
    m_PrevAnimDirection = EDIRECTION::END; // 추후, Walk또는 Idle 상태로 전환 시, Transition 처리가 제대로 이루어지려면, 상태를 돌려놔야 함
    m_PushedOutSpriteIdxToShow = 0;

    switch (GetEightDirection(_PushedOutFaceDirection))
    {
    case EDIRECTION::UP:            m_PushedOutSpriteIdxToShow = GetRandom(0, 1) ? 3 : 4; break; // 얘네는 명확히 떨어지는 Sprite 방향이 없어서 Random하게 비슷한 방향 Sprite에서 뽑음
    case EDIRECTION::LEFT:          m_PushedOutSpriteIdxToShow = GetRandom(0, 1) ? 2 : 3; break;
    case EDIRECTION::DOWN:          m_PushedOutSpriteIdxToShow = GetRandom(0, 1) ? 1 : 2; break;
            
    case EDIRECTION::RIGHT:         m_PushedOutSpriteIdxToShow = 0; break;
    case EDIRECTION::RIGHT_UP:      m_PushedOutSpriteIdxToShow = 4; break;
    case EDIRECTION::UP_LEFT:       m_PushedOutSpriteIdxToShow = 3; break;
    case EDIRECTION::LEFT_DOWN:     m_PushedOutSpriteIdxToShow = 2; break;
    case EDIRECTION::DOWN_RIGHT:    m_PushedOutSpriteIdxToShow = 1; break;
    case EDIRECTION::END: break;
    }
}

UINT CCharacterAnimHandler::GetDieFlipbookIdxBase() const
{
    // 이전 상태였던 PushedOut 방향의 Idx를 참조해서 재생시킬 Die Flipbook Idx base를 구한다
    switch (m_PushedOutSpriteIdxToShow)
    {
    case 0: case 1: case 4: return 0;
    case 2:                 return 1;
    case 3:                 return 2;
    }

    assert(nullptr);
    return -1;
}
