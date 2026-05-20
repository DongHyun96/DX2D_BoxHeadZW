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
    // TODO : 만약에 필요하다하면, GM의 OnGameStart Delegate 구독 박아두고 호출받아서 처리를 할 것(당장에는 Preview 오브젝트 Level에서 삭제해둠)
    // if (GM->GetIsGameStart()) Destroy();
}

void CAirStrikePreview::SaveToLevelFile(FILE* _File)
{
}

void CAirStrikePreview::LoadFromLevelFile(FILE* _File)
{
}
