#include "pch.h"
#include "CText.h"
#include "Source/ScriptMgr.h"
#include "GameEngine/03.Manager/10.FontMgr/FontMgr.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"
#include "GameEngine/03.Manager/06.RenderMgr/RenderMgr.h"
#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"

CText::CText()
    : CGameUI(TEXT)
{
}

CText::CText(const CText& _Origin)
    : CGameUI(_Origin)
    , m_strText(_Origin.m_strText)
    , m_FontStyle(_Origin.m_FontStyle)
    , m_fFontSize(_Origin.m_fFontSize)
    , m_Color(_Origin.m_Color)
{
}

CText::~CText()
{
}

void CText::Init()
{
    CGameUI::Init();
    
    AddScriptParam(SCRIPT_PARAM::WSTRING, &m_strText, L"Text");
    AddScriptParam(SCRIPT_PARAM::WSTRING, &m_FontStyle, L"FontStyle");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fFontSize, L"FontSize");
    AddScriptParam(SCRIPT_PARAM::COLOR, &m_Color, L"Color");
    
    GetOwner()->SetIsTextObject(true);
}

void CText::Tick()
{
    CGameUI::Tick();
}

void CText::Render()
{
    const Vec2 vWorldPos = Transform()->GetWorldPos2D();
    const Vec2 vResol    = Device::GetInst()->GetRenderResolution();

    // UI 카메라가 고정(0,0)되어 있고 회전/스케일이 없으므로 직접 계산
    // 월드 좌표(중앙 0,0) -> 화면 좌표(좌상단 0,0)
    // ScreenX = WorldX + (Width / 2)
    // ScreenY = (Height / 2) - WorldY
    Vec2 vScreenPos{};
    vScreenPos.x = vWorldPos.x + (vResol.x * 0.5f);
    vScreenPos.y = (vResol.y * 0.5f) - vWorldPos.y;
    
    // FontMgr를 사용하여 화면에 텍스트 출력
    FontMgr::GetInst()->DrawFont(m_strText.c_str(), m_FontStyle.c_str(), vScreenPos.x, vScreenPos.y, m_fFontSize, m_Color);
}

void CText::SaveToLevelFile(FILE* _File)
{
    CGameUI::SaveToLevelFile(_File);
    
    SaveWString(_File, m_strText);
    SaveWString(_File, m_FontStyle);
    fwrite(&m_fFontSize, sizeof(float), 1, _File);
    fwrite(&m_Color, sizeof(Vec4), 1, _File);
}

void CText::LoadFromLevelFile(FILE* _File)
{
    CGameUI::LoadFromLevelFile(_File);
    
    m_strText = LoadWString(_File);
    m_FontStyle = LoadWString(_File);
    fread(&m_fFontSize, sizeof(float), 1, _File);
    fread(&m_Color, sizeof(Vec4), 1, _File);
}
