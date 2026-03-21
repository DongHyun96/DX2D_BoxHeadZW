#include "pch.h"
#include "APrefab.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"

set<GameObject*> APrefab::m_setPrefabObjects{};

APrefab::APrefab()
    : Asset(ASSET_TYPE::PREFAB)
{
}

APrefab::~APrefab()
{
}

bool APrefab::SetProtoObj(const Ptr<GameObject>& _Object)
{
    if (!_Object) return false;
    
    m_ProtoObj = _Object->Clone();
    m_setPrefabObjects.insert(m_ProtoObj.Get());        
}

GameObject* APrefab::Instantiate()
{
    return !m_ProtoObj ? nullptr : m_ProtoObj->Clone();
}

GameObject* APrefab::InstantiateAndSpawnToCurLevel()
{
    if (!m_ProtoObj) return nullptr;
    
    GameObject* pObject = m_ProtoObj->Clone();

    CreateObject(pObject, pObject->GetLayerIdx());
    return pObject;
}

bool APrefab::IsPrefabPrototypeObject(const Ptr<GameObject>& _Object)
{
    return m_setPrefabObjects.contains(_Object.Get());
}

HRESULT APrefab::Save(const wstring& _FilePath)
{
    if (FAILED(Asset::Save(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"wb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[APrefab::Save] : Open File failed!");
        return E_FAIL;
    }

    m_ProtoObj->SaveToLevelFile(pFile);
    
    fclose(pFile);

    return S_OK;
}

HRESULT APrefab::Load(const wstring& _FilePath)
{
    if (FAILED(Asset::Load(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"rb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[APrefab::Load] : Open File failed!");
        return E_FAIL;
    }
    
    m_ProtoObj = new GameObject;
    m_ProtoObj->LoadFromLevelFile(pFile);

    fclose(pFile);
    
    // 프리팹 ProtoObject, set에 추가
    m_setPrefabObjects.insert(m_ProtoObj.Get());

    return S_OK;
}
