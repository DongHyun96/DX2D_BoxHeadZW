#include "pch.h"
#include "CFlipbookRender.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"

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
        pMtrl->SetDomain(RENDER_DOMAIN::DOMAIN_TRANSPARENT);
        pMtrl->SetScalar(VEC4_0, DEF_COLOR_WHITE);
        pMtrl->SetIsProvidedByEngine(true);
    }

    SetMaterial(pMtrl);
}

void CFlipbookRender::FinalTick()
{
    CRenderComponent::FinalTick();
    
    // 현재 Selected된 카테고리가 없을 경우
    if (!m_vecCurSelectedCategoryFlipbooks) return;
    
    const vector<Ptr<AFlipbook>>& vecCurSelectedCategoryFlipbooks = *m_vecCurSelectedCategoryFlipbooks;
    
    // 아무 Flipbook도 갖지 못한 카테고리 종류이거나, 현재 선택된 Flipbook 이 없을 때
    if (m_CurSelectedFlipbookIdx < 0 || vecCurSelectedCategoryFlipbooks.empty()) return; 
    
    if (CheckFinish()) return;

    const float frameTimeLimit = 1.f / m_FPS;

    // TODO : 다시 DT로 수정하기
    // m_FrameTimer += DT;
    if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP) m_FrameTimer += E_DT;
    else m_FrameTimer += DT;
    
    if (m_FrameTimer > frameTimeLimit) // 한 프레임 보여주기 시간 끝
    {
        m_FrameTimer -= frameTimeLimit;
        
        if (!m_bPlayReverse) ++m_CurAnimatingSpriteIdx;
        else                 --m_CurAnimatingSpriteIdx;

        if (m_CurAnimatingSpriteIdx < 0 ||
            m_CurAnimatingSpriteIdx >= vecCurSelectedCategoryFlipbooks[m_CurSelectedFlipbookIdx]->GetSpriteCount()) // 애니메이션 한 바퀴 순회 끝
        {
            m_bCurCycleFinished = true;

            // 나간 Idx에 대해 다시 Boundary로 들어오게끔 처리
            if (!m_bPlayReverse) --m_CurAnimatingSpriteIdx;
            else                 ++m_CurAnimatingSpriteIdx;
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
    ApplyRenderTransformConst();
    GetMaterial()->Binding();
    GetMesh()->Render();
    
    GetMaterial()->Clear();
}

bool CFlipbookRender::CheckFinish()
{
    if (m_bStopped)              return true;   // Stopped된 상황
    if (!m_bCurCycleFinished)    return false;  // 아직 현재 사이클이 진행중
    
    const Ptr<AFlipbook> CurrentFlipbook = (*m_vecCurSelectedCategoryFlipbooks)[m_CurSelectedFlipbookIdx];
    
    if (m_RepeatCount == 0)
    {
        // 반복재생(-1)이 아닌 때에 현재 사이클이 종료되었고, RepeatCount 마저도 모두 소진한 상태 (이제 막 Animation이 모두 재생되어 Stopped 처리된 상황)
        m_bStopped = true;
        
        // EndEvent Callback이 걸려있다면 Callback 처리를 해준다.
        if (m_EndEvents.contains(CurrentFlipbook.Get()) && m_EndEvents[CurrentFlipbook.Get()])
            m_EndEvents[CurrentFlipbook.Get()](); // Callback 처리    
        
        return true;
    }
    
    // 현재 사이클이 종료됨
    // 반복재생(-1) 처리가 걸렸거나, 아직 RepeatCount가 남아있는 경우

    if (m_RepeatCount != -1)
    {
        // 무한루프 재생이 아닌 상황 (RepeatCount를 하나 감소시킨다)
        --m_RepeatCount;
    }

    m_CurAnimatingSpriteIdx = !m_bPlayReverse ? 0 : CurrentFlipbook->GetSpriteCount() - 1;
    m_bCurCycleFinished     = false;
    
    return false;
}

bool CFlipbookRender::AddNotifyFlipbookEndEvent(const wstring& _Category, UINT _FlipbookIdx, function<void()> _EndEvent)
{
    if (!m_mapCategoryFlipbooks.contains(_Category)) return false;
    if (_FlipbookIdx >= m_mapCategoryFlipbooks[_Category].size()) return false;
    
    m_EndEvents[ m_mapCategoryFlipbooks[_Category][_FlipbookIdx].Get() ] = _EndEvent;
    return true;
}

bool CFlipbookRender::SetCurrentCategory(const wstring& _CategoryKey, int _FlipbookToSelect, int _SpriteToSelect)
{
    if (!m_mapCategoryFlipbooks.contains(_CategoryKey)) return false;
    
    m_CurSelectedCategory               = _CategoryKey;
    m_vecCurSelectedCategoryFlipbooks   = &m_mapCategoryFlipbooks[_CategoryKey];

    // 현재 선택된 카테고리 내의 vecFlipbooks size를 넘기는 Idx가 들어왔다면 0으로 초기화
    m_CurSelectedFlipbookIdx = (_FlipbookToSelect >= m_vecCurSelectedCategoryFlipbooks->size()) ? 0 : _FlipbookToSelect; 

    m_CurAnimatingSpriteIdx = 0;
    m_RepeatCount           = 0;    
    m_bCurCycleFinished     = false;   
    
    m_FPS                   = 0.f;            
    m_FrameTimer            = 0.f;         
    m_bPlayReverse          = false;   
    m_bStopped              = true;


    // 빈 카테고리일 수도 있다
    if (m_CurSelectedFlipbookIdx < m_vecCurSelectedCategoryFlipbooks->size())
    {
        Ptr<AFlipbook> TargetFlipbook = m_vecCurSelectedCategoryFlipbooks->at(m_CurSelectedFlipbookIdx);
        SetRenderOffset(TargetFlipbook->GetRenderOffset());
        SetRenderScale(TargetFlipbook->GetRenderScale());
    }
}

bool CFlipbookRender::Play(int _FlipbookIdx, float _FPS, int _RepeatCount, bool _bPlayReverse)
{
    assert(_FPS > 0.f);

    if (!m_vecCurSelectedCategoryFlipbooks) return false; // 현재 골라놓은 카테고리가 없음
    if (_FlipbookIdx < 0 || _FlipbookIdx >= m_vecCurSelectedCategoryFlipbooks->size()) return false;
    if (_RepeatCount < -1 || _RepeatCount == 0) return false; // Invalid Repeat Count
    
    m_bPlayReverse = _bPlayReverse;
    m_CurSelectedFlipbookIdx = _FlipbookIdx;
    
    Ptr<AFlipbook> TargetFlipbook = m_vecCurSelectedCategoryFlipbooks->at(m_CurSelectedFlipbookIdx);
    
    m_CurAnimatingSpriteIdx  = !_bPlayReverse ? 0 : TargetFlipbook->GetSpriteCount() - 1;
    m_RepeatCount            = _RepeatCount;
    m_FPS                    = _FPS;
    m_FrameTimer             = 0.f;
    m_bCurCycleFinished      = true; // 이걸 true로 맞춰주어야 PlayCount가 정상적으로 동작
    m_bStopped               = false;
    
    // Render Transform 관련값 맞추기
    SetRenderOffset(TargetFlipbook->GetRenderOffset());
    SetRenderScale(TargetFlipbook->GetRenderScale());
    
    return true;
}

bool CFlipbookRender::Play(const wstring& _Category, int _FlipbookIdx, float _FPS, int _RepeatCount, bool _bPlayReverse)
{
    assert(_FPS > 0.f);

    if (!m_mapCategoryFlipbooks.contains(_Category)) return false;

    // 고른 카테고리로 데이터 세팅
    m_CurSelectedCategory = _Category;
    m_vecCurSelectedCategoryFlipbooks = &m_mapCategoryFlipbooks[_Category];
    
    return Play(_FlipbookIdx, _FPS, _RepeatCount, _bPlayReverse);
}

bool CFlipbookRender::Stop()
{
    // 현재 카테고리가 지정되어 있지 않은 상황
    if (!m_vecCurSelectedCategoryFlipbooks) return false;
    if (m_CurSelectedFlipbookIdx >= m_vecCurSelectedCategoryFlipbooks->size()) return false;
    
    Ptr<AFlipbook> TargetFlipbook = m_vecCurSelectedCategoryFlipbooks->at(m_CurSelectedFlipbookIdx);

    // 나머지 데이터 초기화
    m_CurAnimatingSpriteIdx = m_bPlayReverse ? 0 : TargetFlipbook->GetSpriteCount() - 1; // 처음 지점으로 돌리기
    
    m_RepeatCount       = 0;
    m_bCurCycleFinished = false;
    m_FrameTimer        = 0.f;
    m_bStopped          = true;
    
    return true;
}

bool CFlipbookRender::Stop(const wstring& _Category, int _FlipbookIdx, int _SpriteIdx)
{
    if (!m_mapCategoryFlipbooks.contains(_Category))                                     return false;
    if (_FlipbookIdx >= m_mapCategoryFlipbooks[_Category].size())                        return false;
    if (_SpriteIdx >= m_mapCategoryFlipbooks[_Category][_FlipbookIdx]->GetSpriteCount()) return false;

    // 현재 카테고리 지정
    SetCurrentCategory(_Category);

    m_CurSelectedFlipbookIdx    = _FlipbookIdx;
    m_CurAnimatingSpriteIdx     = _SpriteIdx;
    
    m_RepeatCount       = 0;
    m_bCurCycleFinished = false;
    m_FrameTimer        = 0.f;
    m_bStopped          = true;

    // 고른 Target Flipbook의 Render Transform 관련값으로 맞추기
    Ptr<AFlipbook> TargetFlipbook = m_mapCategoryFlipbooks[_Category][_FlipbookIdx];
    SetRenderOffset(TargetFlipbook->GetRenderOffset());
    SetRenderScale(TargetFlipbook->GetRenderScale());
    
    return true;
}

bool CFlipbookRender::Stop(int _FlipbookIdx, int _SpriteIdx)
{
    if (!m_vecCurSelectedCategoryFlipbooks)                                                  return false;
    if (_FlipbookIdx >= m_vecCurSelectedCategoryFlipbooks->size())                           return false;
    if (_SpriteIdx >= m_vecCurSelectedCategoryFlipbooks->at(_FlipbookIdx)->GetSpriteCount()) return false;

    m_CurSelectedFlipbookIdx    = _FlipbookIdx;
    m_CurAnimatingSpriteIdx     = _SpriteIdx;
    
    m_RepeatCount       = 0;
    m_bCurCycleFinished = false;
    m_FrameTimer        = 0.f;
    m_bStopped          = true;
    
    // 고른 Target Flipbook의 Render Transform 관련값으로 맞추기
    Ptr<AFlipbook> TargetFlipbook = m_mapCategoryFlipbooks[m_CurSelectedCategory][_FlipbookIdx];
    SetRenderOffset(TargetFlipbook->GetRenderOffset());
    SetRenderScale(TargetFlipbook->GetRenderScale());
    
    return true;
}

/*bool CFlipbookRender::Stop(int _FlipbookIdx, int _SpriteIdx)
{
    
}*/

bool CFlipbookRender::Stop(int _SpriteIdx)
{
    if (!m_vecCurSelectedCategoryFlipbooks) return false;
    if (_SpriteIdx >= (*m_vecCurSelectedCategoryFlipbooks)[m_CurSelectedFlipbookIdx]->GetSpriteCount()) return false;    
    
    // 나머지 데이터 초기화
    m_CurAnimatingSpriteIdx = _SpriteIdx;
    
    m_RepeatCount       = 0;
    m_bCurCycleFinished = false;
    m_FrameTimer        = 0.f;
    m_bStopped          = true;
    
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
        m_bCurCycleFinished = false;
        return true;
    }

    if (m_CurSelectedFlipbookIdx == _Idx)
    {
        m_CurSelectedFlipbookIdx = min(_Idx, vecFlipbooks.size() - 1);
        m_CurAnimatingSpriteIdx = 0;
        m_bCurCycleFinished = false;
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

    fwrite(&m_CurSelectedFlipbookIdx, sizeof(int), 1, _File); // 카테고리 내에서 지정된 Flipbook Idx 저장
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
    
    SetCurrentCategory(m_CurSelectedCategory); // 카테고리 지정

    fread(&m_CurSelectedFlipbookIdx, sizeof(int), 1, _File);
    fread(&m_CurAnimatingSpriteIdx, sizeof(int), 1, _File);
    fread(&m_FPS, sizeof(int), 1, _File);
    
}
