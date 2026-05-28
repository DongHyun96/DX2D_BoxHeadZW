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

CText::~CText()
{
}

void CText::Init()
{
    CGameUI::Init();

    AddScriptParam(SCRIPT_PARAM::WSTRING,       &m_strText,     L"Text");
    AddScriptParam(SCRIPT_PARAM::FONT_STYLE,    &m_FontStyle,   L"FontStyle");
    AddScriptParam(SCRIPT_PARAM::FLOAT,         &m_fFontSize,   L"FontSize", false);
    AddScriptParam(SCRIPT_PARAM::COLOR,         &m_Color,       L"Color");
    AddScriptParam(SCRIPT_PARAM::FONT_ALIGN,    &m_Align,       L"FontAlign");
    AddScriptParam(SCRIPT_PARAM::FONT_COORD_MODE, &m_CoordMode, L"CoordMode");
    
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
    Vec2 ScreenPos{};

    switch (m_CoordMode)
    {
    case TEXT_COORD_MODE::LEGACY_SCREEN_CENTER:
        ScreenPos.x = vWorldPos.x + (vResol.x * 0.5f);
        ScreenPos.y = (vResol.y * 0.5f) - vWorldPos.y;
        break;
    case TEXT_COORD_MODE::CAMERA_PROJECTED:
    {
        CCamera* UICam = RenderMgr::GetInst()->GetUICamera();
        if (!UICam) return;
        
        UICam->WorldToScreen(Transform()->GetWorldPos(), ScreenPos);
    }
        break;
    }
    
    // FontMgr를 사용하여 화면에 텍스트 출력
    if (m_FontStyle == L"None" || m_FontStyle.empty())
    {
        // 폰트 스타일이 없으면 기본 렌더링 (기존 DrawFont도 정렬 파라미터를 추가하거나 위 오버로딩을 사용)
        FontMgr::GetInst()->DrawFont(m_strText.c_str(), L"Arial", ScreenPos.x, ScreenPos.y, m_fFontSize, m_Color, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, m_Align);
    }
    else
    {
        // 커스텀 폰트 렌더링에 정렬 옵션 전달
        FontMgr::GetInst()->DrawFont(m_strText.c_str(), m_FontStyle.c_str(), ScreenPos.x, ScreenPos.y, m_fFontSize, m_Color, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, m_Align);
    }
}

void CText::SaveToLevelFile(FILE* _File)
{
    CGameUI::SaveToLevelFile(_File);
    
    SaveWString(_File, m_strText);
    SaveWString(_File, m_FontStyle);
    fwrite(&m_fFontSize, sizeof(float), 1, _File);
    fwrite(&m_Color, sizeof(Vec4), 1, _File);
    fwrite(&m_Align, sizeof(FONT_ALIGN), 1, _File);
    fwrite(&m_CoordMode, sizeof(TEXT_COORD_MODE), 1, _File);
}

void CText::LoadFromLevelFile(FILE* _File)
{
    CGameUI::LoadFromLevelFile(_File);
    
    m_strText = LoadWString(_File);
    m_FontStyle = LoadWString(_File);
    fread(&m_fFontSize, sizeof(float), 1, _File);
    fread(&m_Color, sizeof(Vec4), 1, _File);
    fread(&m_Align, sizeof(FONT_ALIGN), 1, _File);
    fread(&m_CoordMode, sizeof(TEXT_COORD_MODE), 1, _File);
}
