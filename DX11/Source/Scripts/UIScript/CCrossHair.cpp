#include "pch.h"
#include "CCrossHair.h"

#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "InGameUIManager/CIngameUIManager.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CCrossHair::CCrossHair()
    : CScript(CROSSHAIR)
{
}

CCrossHair::~CCrossHair()
{
    ShowCursor(TRUE);
}

void CCrossHair::Init()
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_AnimFPS, L"Animation FPS");
}

void CCrossHair::Begin()
{
    CScript::Begin();
    ShowCursor(FALSE);
    FlipbookRender()->Play(L"CrossHair", 0, m_AnimFPS, -1);
    
    GM->GetIngameUIManager()->SetCrossHair(this);
}

void CCrossHair::Tick()
{
    const Vec2 vMousePos = KeyMgr::GetInst()->GetMouseUIPos();
    Transform()->SetRelativePosXY(vMousePos);
}

void CCrossHair::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_AnimFPS, sizeof(float), 1, _File);
}

void CCrossHair::LoadFromLevelFile(FILE* _File)
{
    fread(&m_AnimFPS, sizeof(float), 1, _File);
}
