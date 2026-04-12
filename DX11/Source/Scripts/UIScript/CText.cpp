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
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fFontSize, L"FontSize");
    AddScriptParam(SCRIPT_PARAM::COLOR, &m_Color, L"Color");
    
    if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
        RenderMgr::GetInst()->RegisterGameText(this);
}

void CText::Tick()
{
    CGameUI::Tick();
    Render();    
}

void CText::Render()
{
    Vec2 vWorldPos = Transform()->GetWorldPos2D();
    
    if (nullptr != GetRenderCom())
    {
        vWorldPos += GetRenderCom()->GetRenderOffset();
    }
    
    // UI 카메라를 사용하여 월드 좌표를 화면 좌표로 변환
    Ptr<CCamera> pUICam = RenderMgr::GetInst()->GetUICamera();
    Vec2 vScreenPos = vWorldPos;

    if (pUICam != nullptr)
    {
        Matrix matView = pUICam->GetViewMat();
        Matrix matProj = pUICam->GetProjMat();
        
        Vec3 vPos = Vec3(vWorldPos.x, vWorldPos.y, 0.f);
        vPos = Vec3::Transform(vPos, matView);
        vPos = Vec3::Transform(vPos, matProj);
        
        // vPos는 이제 NDC 좌표 (-1 ~ 1)
        // 화면 해상도에 맞춰 픽셀 좌표(0 ~ Width, 0 ~ Height)로 변환
        Vec2 vResol = Device::GetInst()->GetRenderResolution();
        vScreenPos.x = (vPos.x + 1.f) * 0.5f * vResol.x;
        vScreenPos.y = (1.f - vPos.y) * 0.5f * vResol.y;
    }
    
    // FontMgr를 사용하여 화면에 텍스트 출력
    FontMgr::GetInst()->DrawFont(m_strText.c_str(), vScreenPos.x, vScreenPos.y, m_fFontSize, m_Color);
}

void CText::SaveToLevelFile(FILE* _File)
{
    CGameUI::SaveToLevelFile(_File);
    
    SaveWString(_File, m_strText);
    fwrite(&m_fFontSize, sizeof(float), 1, _File);
    fwrite(&m_Color, sizeof(Vec4), 1, _File);
}

void CText::LoadFromLevelFile(FILE* _File)
{
    CGameUI::LoadFromLevelFile(_File);
    
    m_strText = LoadWString(_File);
    fread(&m_fFontSize, sizeof(float), 1, _File);
    fread(&m_Color, sizeof(Vec4), 1, _File);
}
