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
    // 현재 Selected된 카테고리가 없을 경우
    if (!m_vecCurSelectedCategoryFlipbooks) return;
    
    const vector<Ptr<AFlipbook>>& vecCurSelectedCategoryFlipbooks = *m_vecCurSelectedCategoryFlipbooks;
    
    // 아무 Flipbook도 갖지 못한 카테고리 종류이거나, 현재 선택된 Flipbook 이 없을 때
    if (m_CurSelectedFlipbookIdx < 0 || vecCurSelectedCategoryFlipbooks.empty()) return; 
    
    if (CheckFinish()) return;

    const float fLimit = 1.f / m_FPS;

    m_AccTime += DT;

    if (m_AccTime > fLimit)
    {
        m_AccTime -= fLimit;
        ++m_CurAnimatingSpriteIdx;

        if (m_CurAnimatingSpriteIdx >= vecCurSelectedCategoryFlipbooks[m_CurSelectedFlipbookIdx]->GetSpriteCount())
        {
            m_Finish = true;
            --m_CurAnimatingSpriteIdx;
        }
    }    
}

void CFlipbookRender::Render()
{
    if (!GetMaterial() || !GetMesh()) return;

    // 현재 Selected된 카테고리가 없을 경우
    if (!m_vecCurSelectedCategoryFlipbooks) return;
    
    const vector<Ptr<AFlipbook>>& vecCurSelectedCategoryFlipbooks = *m_vecCurSelectedCategoryFlipbooks;
    
    if (vecCurSelectedCategoryFlipbooks.empty()) return;
    
    if (m_CurSelectedFlipbookIdx < 0 || m_CurSelectedFlipbookIdx >= vecCurSelectedCategoryFlipbooks.size()) return;
    
    
    Ptr<AFlipbook> CurSelectedFlipbook = vecCurSelectedCategoryFlipbooks[m_CurSelectedFlipbookIdx];
    if (!CurSelectedFlipbook) return;
    
    Ptr<ASprite> pCurSprite = CurSelectedFlipbook->GetSprite(m_CurAnimatingSpriteIdx);
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
        m_CurAnimatingSpriteIdx = 0;
        m_Finish = false;
        --m_RepeatCount;
        return false;
    }
    
    if (m_RepeatCount == -1)
    {
        m_CurAnimatingSpriteIdx = 0;
        m_Finish = false;
        return false;
    }
    return true;
    
}

bool CFlipbookRender::Play(const wstring& _Category, int _FlipbookIdx, float _FPS, int _RepeatCount)
{
    assert(_FPS > 0.f);

    if (!m_mapCategoryFlipbooks.contains(_Category)) return false;

    // 고른 카테고리로 데이터 세팅
    m_CurSelectedCategory = _Category;
    m_vecCurSelectedCategoryFlipbooks = &m_mapCategoryFlipbooks[_Category];
    
    const vector<Ptr<AFlipbook>>& vecCurSelectedCategoryFlipbooks = *m_vecCurSelectedCategoryFlipbooks;
    
    if (_FlipbookIdx < 0 || _FlipbookIdx >= vecCurSelectedCategoryFlipbooks.size())
        return false;
        
    m_CurSelectedFlipbookIdx    = _FlipbookIdx;
    m_CurAnimatingSpriteIdx     = 0;
    m_RepeatCount               = _RepeatCount;
    m_FPS                       = _FPS;
    m_AccTime                   = 0.f;
    m_Finish                    = false;
    
    return true;
}

bool CFlipbookRender::SetFlipbook(const wstring& _Category, int _Idx, const Ptr<AFlipbook>& _Flipbook)
{
    if (_Idx < 0) return false;
    if (!m_mapCategoryFlipbooks.contains(_Category)) return false; // 해당하는 카테고리가 없을 때
    
    vector<Ptr<AFlipbook>>& vecFlipbooks = m_mapCategoryFlipbooks[_Category];     

    if (vecFlipbooks.size() <= _Idx)
        vecFlipbooks.resize(_Idx + 1);

    vecFlipbooks[_Idx] = _Flipbook;
    
    return true;
}

bool CFlipbookRender::AddFlipbook(const wstring& _Category, const Ptr<AFlipbook>& _Flipbook)
{
    if (!_Flipbook) return false;

    // 새로이 들어온 Category의 경우, map 에 새로운 카테고리로 잡아준다
    if (!m_mapCategoryFlipbooks.contains(_Category))
        m_mapCategoryFlipbooks.insert(make_pair(_Category, vector<Ptr<AFlipbook>>()));

    m_mapCategoryFlipbooks[_Category].push_back(_Flipbook);
    return true;
}

UINT CFlipbookRender::GetCategoryFlipbookCount(const wstring& _Category)
{
    if (!m_mapCategoryFlipbooks.contains(_Category)) return 0;
    return m_mapCategoryFlipbooks[_Category].size();
}

Ptr<AFlipbook> CFlipbookRender::GetFlipbook(const wstring& _Category, int _Idx)
{
    if (!m_mapCategoryFlipbooks.contains(_Category)) return nullptr;
    
    const vector<Ptr<AFlipbook>>& TargetFlipbook = m_mapCategoryFlipbooks[_Category];
    
    return _Idx < 0 || _Idx >= TargetFlipbook.size() ? nullptr : TargetFlipbook[_Idx];
}

bool CFlipbookRender::RemoveFlipbook(const wstring& _Category, int _Idx)
{
    if (!m_mapCategoryFlipbooks.contains(_Category)) return false; // 해당하는 카테고리가 없을 때
    vector<Ptr<AFlipbook>>& vecFlipbooks = m_mapCategoryFlipbooks[_Category]; // Target flipbook vector
    
    if (_Idx < 0 || _Idx >= vecFlipbooks.size()) return false;
        
    vecFlipbooks.erase(vecFlipbooks.begin() + _Idx);

    if (vecFlipbooks.empty())
    {
        m_CurSelectedFlipbookIdx = 0;
        m_CurAnimatingSpriteIdx = 0;
        m_Finish = false;
        return true;
    }

    if (m_CurSelectedFlipbookIdx == _Idx)
    {
        m_CurSelectedFlipbookIdx = min(_Idx, vecFlipbooks.size() - 1);
        m_CurAnimatingSpriteIdx = 0;
        m_Finish = false;
    }
    else if (m_CurSelectedFlipbookIdx > _Idx)
        --m_CurSelectedFlipbookIdx;

    return true;
}

bool CFlipbookRender::RemoveFlipbook(const wstring& _Category, const Ptr<AFlipbook>& _Flipbook)
{
    if (!m_mapCategoryFlipbooks.contains(_Category)) return false; // 해당하는 카테고리가 없을 때
    vector<Ptr<AFlipbook>>& vecFlipbooks = m_mapCategoryFlipbooks[_Category]; // Target flipbook vector
    
    for (int i = 0; i < vecFlipbooks.size(); ++i)
    {
        if (vecFlipbooks[i] == _Flipbook) return RemoveFlipbook(_Category, i);
    }
    return false;
}

bool CFlipbookRender::SwapFlipbook(const wstring& _Category, int _A, int _B)
{
    if (!m_mapCategoryFlipbooks.contains(_Category)) return false; // 해당하는 카테고리가 없을 때
    vector<Ptr<AFlipbook>>& vecFlipbooks = m_mapCategoryFlipbooks[_Category]; // Target flipbook vector
    
    if (_A < 0 || _A >= vecFlipbooks.size() || _B < 0 || _B >= vecFlipbooks.size()) return false;
    if (_A == _B) return true;

    swap(vecFlipbooks[_A], vecFlipbooks[_B]);

    if (m_CurSelectedFlipbookIdx == _A)      m_CurSelectedFlipbookIdx = _B;
    else if (m_CurSelectedFlipbookIdx == _B) m_CurSelectedFlipbookIdx = _A;

    return true;
}

void CFlipbookRender::SaveToLevelFile(FILE* _File)
{
    CRenderComponent::SaveToLevelFile(_File);

    // 현재 선택된 카테고리 저장 
    SaveWString(_File, m_CurSelectedCategory);
    
    // 카테고리 갯수 저장
    const size_t CategoryCount = m_mapCategoryFlipbooks.size();
    fwrite(&CategoryCount, sizeof(size_t), 1, _File);

    for (const pair<const wstring, vector<Ptr<AFlipbook>>>& Pair : m_mapCategoryFlipbooks)
    {
        SaveWString(_File, Pair.first); // 카테고리 이름 저장
        
        // 해당 카테고리의 flipbook 개수 저장
        size_t FlipbookCount = Pair.second.size(); 
        fwrite(&FlipbookCount, sizeof(size_t), 1, _File);
        
        // Flipbook들 저장
        for (const Ptr<AFlipbook>& Flipbook : Pair.second)
            SaveAssetRef(_File, Flipbook.Get());    
    }

    fwrite(&m_CurSelectedFlipbookIdx, sizeof(int), 1, _File);
    fwrite(&m_CurAnimatingSpriteIdx, sizeof(int), 1, _File);
    fwrite(&m_FPS, sizeof(int), 1, _File);
}

void CFlipbookRender::LoadFromLevelFile(FILE* _File)
{
    CRenderComponent::LoadFromLevelFile(_File);
    
    // 현재 선택된 카테고리 불러오기
    m_CurSelectedCategory = LoadWString(_File);
    
    // 카테고리별, Flipbook들 불러오기
    size_t CategoryCount{};
    fread(&CategoryCount, sizeof(size_t), 1, _File);
    
    for (size_t i = 0; i < CategoryCount; ++i)
    {
        wstring CategoryName{};
        CategoryName = LoadWString(_File); // 카테고리 이름 불러오기
        m_mapCategoryFlipbooks.insert(make_pair(CategoryName, vector<Ptr<AFlipbook>>())); // 해당 카테고리 이름으로 새로운 vector data 넣기

        // 현재 카테고리의 총 Flipbook 개수 불러오기
        size_t FlipbookCount{};
        fread(&FlipbookCount, sizeof(size_t), 1, _File);
        
        // Flipbook들 불러오기
        for (size_t j = 0; j < FlipbookCount; ++j)
            m_mapCategoryFlipbooks[CategoryName].push_back(LoadAssetRef<AFlipbook>(_File));
    }

    fread(&m_CurSelectedFlipbookIdx, sizeof(int), 1, _File);
    fread(&m_CurAnimatingSpriteIdx, sizeof(int), 1, _File);
    fread(&m_FPS, sizeof(int), 1, _File);
    
}
