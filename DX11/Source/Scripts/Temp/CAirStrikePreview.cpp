#include "pch.h"
#include "CAirStrikePreview.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CAirStrikePreview::CAirStrikePreview()
    : CScript(SCRIPT_TYPE::AIRSTRIKEPREVIEW)
{
}

CAirStrikePreview::~CAirStrikePreview()
{
}

void CAirStrikePreview::Begin()
{
    FlipbookRender()->Play(L"AirStrike", 0, 20.f, -1);
}

void CAirStrikePreview::Tick()
{
    if (GM->GetIsGameStart()) Destroy();
}

void CAirStrikePreview::SaveToLevelFile(FILE* _File)
{
}

void CAirStrikePreview::LoadFromLevelFile(FILE* _File)
{
}
