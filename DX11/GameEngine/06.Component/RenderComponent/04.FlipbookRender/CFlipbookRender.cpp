#include "pch.h"
#include "CFlipbookRender.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"

CFlipbookRender::CFlipbookRender()
    : CRenderComponent(COMPONENT_TYPE::FLIPBOOK_RENDER)
{
}
 
CFlipbookRender::~CFlipbookRender()
{
}

void CFlipbookRender::CreateMaterial()
{
    wstring MeshName    = L"RectMesh";
    wstring MtrlName    = L"FlipbookMtrl";
    wstring ShaderName  = L"FlipbookShader";
    
    // RectMesh 설정
    SetMesh(AssetMgr::GetInst()->Find<AMesh>(MeshName));
    
    // 빌보드 전용 재질 생성
    Ptr<AMaterial> pMtrl = AssetMgr::GetInst()->Find<AMaterial>(MtrlName);
    
    // 찾는 재질이 없으면 생성한다
    if (!pMtrl)
    {
        PrintIrrelevantDebugLog();
        
        pMtrl = new AMaterial;
        pMtrl->SetName(MtrlName);
        AssetMgr::GetInst()->AddAsset(pMtrl->GetName(), pMtrl.Get()); // 쉐이더를 찾아서 재질에 새팅해준다.
        
        // 찾는 쉐이더가 없으면 만들어서 에셋매니저에 등록해준다.
        Ptr<AGraphicShader> pShader = AssetMgr::GetInst()->Find<AGraphicShader>(ShaderName);
        pMtrl->SetShader(pShader);
        pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_MASKED);
        pMtrl->SetIsProvidedByEngine(true);
    }

    SetMaterial(pMtrl);
}

void CFlipbookRender::FinalTick()
{
   if (CheckFinish()) return;
    
    const float fLimit = 1.f / m_FPS;
    
    m_AccTime += DT;
    
    if (m_AccTime > fLimit)
    {
        m_AccTime -= fLimit;
        ++m_CurSprite;

        if (m_CurSprite >= m_vecFlipbook[m_CurFlipbook]->GetSpriteCount())
        {
            m_Finish = true;
            --m_CurSprite;
        }
    }    
}

void CFlipbookRender::Render()
{
    if (!GetMaterial() || !GetMesh()) return;
    if (m_vecFlipbook.empty()) return;
    if (m_CurFlipbook < 0 || m_CurFlipbook >= m_vecFlipbook.size()) return;
    
    
    Ptr<AFlipbook> CurSelectedFlipbook = m_vecFlipbook[m_CurFlipbook];
    if (!CurSelectedFlipbook) return;
    
    Ptr<ASprite> pCurSprite = CurSelectedFlipbook->GetSprite(m_CurSprite);
    if (!pCurSprite) return;
    
    
    GetMaterial()->SetTexture(TEX_0, pCurSprite->GetAtlas());
    GetMaterial()->SetScalar(VEC2_0, pCurSprite->GetLeftTopUV());
    GetMaterial()->SetScalar(VEC2_1, pCurSprite->GetSliceUV());

    // BackgroundUV가 제대로 설정되어있지 않다면, Background 처리 x
    if (pCurSprite->GetBackgroundUV() == Vec2::Zero)
        GetMaterial()->SetScalar(VEC2_2, pCurSprite->GetSliceUV());
    else GetMaterial()->SetScalar(VEC2_2, pCurSprite->GetBackgroundUV());
    
    GetMaterial()->SetScalar(VEC2_3, pCurSprite->GetOffsetUV());
    
    GetMaterial()->Binding();
    GetMesh()->Render();
    
    GetMaterial()->Clear();
}

bool CFlipbookRender::CheckFinish()
{
    if (!m_Finish) return false;
    
    if (m_RepeatCount > 0)
    {
        m_CurSprite = 0;
        m_Finish = false;
        --m_RepeatCount;
        return false;
    }
    
    if (m_RepeatCount == -1)
    {
        m_CurSprite = 0;
        m_Finish = false;
        return false;
    }
    return true;
    
}

void CFlipbookRender::Play(int _FlipbookIdx, float _FPS, int _RepeatCount)
{
    assert(_FPS > 0.f);
        
    if (_FlipbookIdx < 0 || _FlipbookIdx >= static_cast<int>(m_vecFlipbook.size()))
        return;
        
    m_CurFlipbook   = _FlipbookIdx;
    m_CurSprite     = 0;
    m_RepeatCount   = _RepeatCount;
    m_FPS           = _FPS;
    m_AccTime       = 0.f;
    m_Finish        = false;
}

void CFlipbookRender::SetFlipbook(int _Idx, const Ptr<AFlipbook>& _Flipbook)
{
    if (_Idx < 0) return;

    if (m_vecFlipbook.size() <= static_cast<size_t>(_Idx))
        m_vecFlipbook.resize(_Idx + 1);

    m_vecFlipbook[_Idx] = _Flipbook;
}

bool CFlipbookRender::RemoveFlipbook(int _Idx)
{
    if (_Idx < 0 || _Idx >= static_cast<int>(m_vecFlipbook.size()))
        return false;

    m_vecFlipbook.erase(m_vecFlipbook.begin() + _Idx);

    if (m_vecFlipbook.empty())
    {
        m_CurFlipbook = 0;
        m_CurSprite = 0;
        m_Finish = false;
        return true;
    }

    if (m_CurFlipbook == _Idx)
    {
        m_CurFlipbook = min(_Idx, static_cast<int>(m_vecFlipbook.size()) - 1);
        m_CurSprite = 0;
        m_Finish = false;
    }
    else if (m_CurFlipbook > _Idx)
        --m_CurFlipbook;

    return true;
}

bool CFlipbookRender::RemoveFlipbook(const Ptr<AFlipbook>& _Flipbook)
{
    for (int i = 0; i < m_vecFlipbook.size(); ++i)
    {
        if (m_vecFlipbook[i] == _Flipbook)
            return RemoveFlipbook(i);
    }
    return false;
}

bool CFlipbookRender::SwapFlipbook(int _A, int _B)
{
    if (_A < 0 || _A >= m_vecFlipbook.size() || _B < 0 || _B >= m_vecFlipbook.size()) return false;
    if (_A == _B) return true;

    swap(m_vecFlipbook[_A], m_vecFlipbook[_B]);

    if (m_CurFlipbook == _A)      m_CurFlipbook = _B;
    else if (m_CurFlipbook == _B) m_CurFlipbook = _A;

    return true;
}

void CFlipbookRender::SaveToLevelFile(FILE* _File)
{
    CRenderComponent::SaveToLevelFile(_File);
    
    const size_t FlipbookCount = m_vecFlipbook.size();
    fwrite(&FlipbookCount, sizeof(size_t), 1, _File);
    
    for (const Ptr<AFlipbook>& Flipbook : m_vecFlipbook)
        SaveAssetRef(_File, Flipbook.Get());

    fwrite(&m_CurFlipbook, sizeof(int), 1, _File);
    fwrite(&m_CurSprite, sizeof(int), 1, _File);
    fwrite(&m_FPS, sizeof(int), 1, _File);
    
}

void CFlipbookRender::LoadFromLevelFile(FILE* _File)
{
    CRenderComponent::LoadFromLevelFile(_File);

    size_t FlipbookCount{};
    fread(&FlipbookCount, sizeof(size_t), 1, _File);

    for (size_t i = 0; i < FlipbookCount; ++i)
        m_vecFlipbook.push_back(LoadAssetRef<AFlipbook>(_File));
    

    fread(&m_CurFlipbook, sizeof(int), 1, _File);
    fread(&m_CurSprite, sizeof(int), 1, _File);
    fread(&m_FPS, sizeof(int), 1, _File);
    
}
