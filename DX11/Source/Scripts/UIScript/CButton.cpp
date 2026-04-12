#include "pch.h"
#include "CButton.h"
#include "Source/ScriptMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/05.GameObject/GameObject.h"
#include "GameEngine/06.Component/01.Transform/CTransform.h"
#include "GameEngine/06.Component/RenderComponent/CRenderComponent.h"

CButton::CButton()
    : CGameUI(BUTTON)
    , m_bMouseOn(false)
    , m_bLBtnDown(false)
{
}

CButton::CButton(const CButton& _Origin)
    : CGameUI(_Origin)
    , m_IdleTex(_Origin.m_IdleTex)
    , m_HoverTex(_Origin.m_HoverTex)
    , m_ClickedTex(_Origin.m_ClickedTex)
    , m_CallBackFunc(nullptr)
    , m_bMouseOn(false)
    , m_bLBtnDown(false)
{
}

CButton::~CButton()
{
}

void CButton::Init()
{
    CGameUI::Init();
    
    AddScriptParam(SCRIPT_PARAM::TEXTURE, &m_IdleTex, L"IdleTexture");
    AddScriptParam(SCRIPT_PARAM::TEXTURE, &m_HoverTex, L"HoverTexture");
    AddScriptParam(SCRIPT_PARAM::TEXTURE, &m_ClickedTex, L"ClickedTexture");
}

void CButton::Begin()
{
    CGameUI::Begin();

    Ptr<CRenderComponent> pRenderCom = GetOwner()->GetRenderCom();
    if (pRenderCom != nullptr)
    {
        pRenderCom->CreateDynamicMaterial();
    }
}

void CButton::Tick()
{
    CGameUI::Tick();

    bool bPrevMouseOn = m_bMouseOn;
    m_bMouseOn = CheckMouseOn();

    if (!bPrevMouseOn && m_bMouseOn)
    {
        OnMouseOn();
    }
    else if (bPrevMouseOn && !m_bMouseOn)
    {
        OnMouseOut();
    }

    if (m_bMouseOn)
    {
        if (KEY_TAP(KEY::MLB))
        {
            m_bLBtnDown = true;
            OnLBtnDown();
        }

        if (KEY_RELEASED(KEY::MLB))
        {
            if (m_bLBtnDown)
            {
                OnLBtnClicked();
            }
            m_bLBtnDown = false;
            OnLBtnUp();
        }
    }
    else
    {
        if (KEY_RELEASED(KEY::MLB))
        {
            m_bLBtnDown = false;
        }
    }

    Ptr<CRenderComponent> pRenderCom = GetOwner()->GetRenderCom();
    if (!pRenderCom) return;
        

    Ptr<AMaterial> pMtrl = pRenderCom->GetMaterial();
    if (!pMtrl) return;

    if (m_bMouseOn)
    {
        if (m_bLBtnDown)
        {
            if (m_ClickedTex != nullptr)
                pMtrl->SetTexture(TEX_0, m_ClickedTex);
        }
        else
        {
            if (m_HoverTex != nullptr)
                pMtrl->SetTexture(TEX_0, m_HoverTex);
        }
    }
    else
    {
        if (m_IdleTex != nullptr)
            pMtrl->SetTexture(TEX_0, m_IdleTex);
    }
}

void CButton::OnMouseOn()
{
}

void CButton::OnMouseOut()
{
}

void CButton::OnLBtnDown()
{
}

void CButton::OnLBtnUp()
{
}

void CButton::OnLBtnClicked()
{
    if (m_CallBackFunc) m_CallBackFunc();
}

bool CButton::CheckMouseOn()
{
    Vec2 vMousePos = KeyMgr::GetInst()->GetMouseUIPos();

    Vec2 vWorldPos = Transform()->GetWorldPos2D() + GetRenderCom()->GetRenderOffset();
    Vec2 vWorldScale = Transform()->GetWorldScale2D() * GetRenderCom()->GetRenderScale();

    if (vMousePos.x > vWorldPos.x - vWorldScale.x * 0.5f &&
        vMousePos.x < vWorldPos.x + vWorldScale.x * 0.5f &&
        vMousePos.y > vWorldPos.y - vWorldScale.y * 0.5f &&
        vMousePos.y < vWorldPos.y + vWorldScale.y * 0.5f)
        return true;

    return false;
}

void CButton::SaveToLevelFile(FILE* _File)
{
    CGameUI::SaveToLevelFile(_File);
    SaveAssetRef(_File, m_IdleTex.Get());
    SaveAssetRef(_File, m_HoverTex.Get());
    SaveAssetRef(_File, m_ClickedTex.Get());
}

void CButton::LoadFromLevelFile(FILE* _File)
{
    CGameUI::LoadFromLevelFile(_File);
    m_IdleTex    = LoadAssetRef<ATexture>(_File);
    m_HoverTex   = LoadAssetRef<ATexture>(_File);
    m_ClickedTex = LoadAssetRef<ATexture>(_File);
}
