#include "pch.h"
#include "ALevel.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/08.CollisionMgr/CollisionMgr.h"
#include "Source/Manager/GameManager.h"

ALevel::ALevel()
    : Asset(ASSET_TYPE::LEVEL)
    , m_CollisionMatrix{}
{
    for (int i = 0; i < MAX_LAYER; ++i)
    {
        m_arrLayer[i].m_LayerIdx = i;
        m_arrLayer[i].m_OwnerLevel = this;
    }
}

ALevel::~ALevel()
{
}

void ALevel::AddObject(int _LayerIdx, const Ptr<GameObject>& _Object)
{
    m_arrLayer[_LayerIdx].AddObject(_Object);
}

void ALevel::Deregister()
{
    for (Layer& layer : m_arrLayer) layer.DeregisterObjectsFromAllObjects();
}

void ALevel::Begin()
{
    m_mapLayerNameIndex.clear();
    
    GM->OnLevelBegin();
    RenderMgr::GetInst()->OnLevelBegin();
    AssetMgr::GetInst()->OnLevelBegin();
    
    for (Layer& layer : m_arrLayer)
        m_mapLayerNameIndex[layer.GetName()] = layer.m_LayerIdx;
        
    for (Layer& layer : m_arrLayer)
        layer.Begin();
}

void ALevel::AfterLevelBegin()
{
    for (Layer& layer : m_arrLayer)
        layer.AfterLevelBegin();
    
    CollisionMgr::GetInst()->CalculateOptimalGridCellSize(this);
}

void ALevel::Tick()
{
    for (Layer& layer : m_arrLayer)
        layer.Tick();
}

void ALevel::FinalTick()
{
    for (Layer& layer : m_arrLayer)
        layer.FinalTick();    
}

bool ALevel::IsLayerNameDuplicated(const wstring& _LayerName, int _SelfIdx)
{
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        if (i == _SelfIdx) continue;        
        if (m_arrLayer[i].GetName() == _LayerName) return true;
    }
    return false;
}

int ALevel::GetLayerIndexByLayerName(const wstring& _LayerName) const
{
    if (!m_mapLayerNameIndex.contains(_LayerName)) return -1;
    return m_mapLayerNameIndex.at(_LayerName);
}

void ALevel::CheckCollisionLayer(UINT _LayerIdx1, UINT _LayerIdx2)
{
    UINT Row = _LayerIdx1;
    UINT Col = _LayerIdx2;

    // 더 작은 레이어 인덱스를 행으로 사용
    if (_LayerIdx2 < _LayerIdx1)
    {
        Row = _LayerIdx2;
        Col = _LayerIdx1;
    }
    
    m_CollisionMatrix[Row] ^= (1 << Col);
    
}

void ALevel::CheckCollisionLayer(const wstring& _LayerName1, const wstring& _LayerName2)
{
    UINT Idx1 = -1; 
    UINT Idx2 = -1;
    for (const Layer& layer : m_arrLayer)
    {
        if (layer.GetName() == _LayerName1) Idx1 = layer.m_LayerIdx;
        if (layer.GetName() == _LayerName2) Idx2 = layer.m_LayerIdx;
        if (Idx1 != -1 && Idx2 != -1) break;
    }
    
    CheckCollisionLayer(Idx1, Idx2);    
}

Ptr<GameObject> ALevel::FindObjectByName(const wstring& _Name)
{
    for (const Layer& layer : m_arrLayer)
    {
        const vector<Ptr<GameObject>>& vecParents = layer.GetParentObjects();

        for (const Ptr<GameObject>& object : vecParents)
        {
            list<Ptr<GameObject>> queue{};
            queue.push_back(object);
            
            while (!queue.empty())
            {
                Ptr<GameObject> pObject = queue.front();
                queue.pop_front();

                // 찾았다
                if (pObject->GetName() == _Name) return pObject;
                
                const vector<Ptr<GameObject>>& vecChild = pObject->GetChildren();
                for (const Ptr<GameObject>& child : vecChild) queue.push_back(child);
            }
        }
    }

    // 없다
    return nullptr;
}

HRESULT ALevel::Save(const wstring& _FilePath)
{
    if (FAILED(Asset::Save(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"wb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[ALevel::Save] : Open File failed!");
        return E_FAIL;
    }

    // 레벨 이름
    wstring LevelName = GetName();
    SaveWString(pFile, LevelName);

    // 충돌 체크 정보
    fwrite(m_CollisionMatrix, sizeof(UINT), MAX_LAYER, pFile);
    
    // 레이어 정보
    for (const Layer& layer : m_arrLayer)
    {
        // 레이어의 이름 저장
        SaveWString(pFile, layer.GetName());

        // 레이어 소속 최상위 부모 오브젝트를 계층구조로 저장한다.
        const vector<Ptr<GameObject>>& vecParents = layer.GetParentObjects();

        size_t ParentSize = vecParents.size();
        fwrite(&ParentSize, sizeof(size_t), 1, pFile);
        
        for (const Ptr<GameObject>& Object : vecParents)
            Object->SaveToLevelFile(pFile);
    }
    
    fclose(pFile);
    
    return S_OK;
}

HRESULT ALevel::Load(const wstring& _FilePath)
{
    if (FAILED(Asset::Load(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"rb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[ALevel::Load] : Open File failed!");
        return E_FAIL;
    }

    // 레벨 이름
    wstring LevelName = LoadWString(pFile);
    SetName(LevelName);

    // 충돌 체크 정보
    fread(m_CollisionMatrix, sizeof(UINT), MAX_LAYER, pFile);
    

    // 레이어 정보
    for (UINT i = 0; i < MAX_LAYER; i++)
    {
        // 레이어의 이름
        wstring LayerName = LoadWString(pFile);
        m_arrLayer[i].SetName(LayerName);
        
        size_t ParentCount{};
        fread(&ParentCount, sizeof(size_t), 1, pFile);
        
        for (size_t j = 0; j < ParentCount; j++)
        {
            Ptr<GameObject> Object = new GameObject;
            Object->LoadFromLevelFile(pFile);
            AddObject(i, Object);
        }
    }
    
    // Scale Z값 처리 (0인 경우 자동적으로 1로 잡는 처리를 넣어주었다)
    for (Layer& layer : m_arrLayer)
    {
        for (const Ptr<GameObject>& ParentObject : layer.GetParentObjects())
        {
            queue<Ptr<GameObject>> q{};
            q.push(ParentObject);
            
            while (!q.empty())
            {
                Ptr<GameObject> pCurrent = q.front(); q.pop();
                for (const Ptr<GameObject>& Child : pCurrent->GetChildren())
                    q.push(Child);
                
                if (pCurrent->Transform() && pCurrent->Transform()->GetRelativeScaleZ() == 0.f)
                    pCurrent->Transform()->SetRelativeScaleZ(1.f);
            }
        }
    }
    
    
    fclose(pFile);
    
    return S_OK;
}