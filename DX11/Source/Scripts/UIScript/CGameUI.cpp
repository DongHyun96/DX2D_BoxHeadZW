#include "pch.h"
#include "CGameUI.h"
#include "GameEngine/05.GameObject/GameObject.h"

CGameUI::CGameUI(int _ScriptType)
    : CScript(_ScriptType)
{
}

CGameUI::~CGameUI()
{
}

void CGameUI::Begin()
{
    // UILayer (31) 설정
    GetOwner()->SetLayerIdx(31);
}

void CGameUI::Tick()
{
}


void CGameUI::SaveToLevelFile(FILE* _File)
{
}

void CGameUI::LoadFromLevelFile(FILE* _File)
{
}
