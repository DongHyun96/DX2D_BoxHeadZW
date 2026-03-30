#pragma once


#include "GameEngine/03.Manager/01.PathMgr/PathMgr.h"
#include "Header/assets.h"

/// <summary>
/// Asset 관리 Manager
/// </summary>
class AssetMgr : public Singleton<AssetMgr>
{
    
    SINGLE(AssetMgr);

private:
    
    map<wstring, Ptr<Asset>>    m_mapAsset[static_cast<UINT>(ASSET_TYPE::END)]{};   // Asset type별 map 배열
    map<uint64_t, GUID>         m_mapTexMetaData{};                                 // 텍스쳐 메타데이터 저장 (파일해시값, Asset GUID)
    bool                        m_Changed{};
    
private:
    
    const map<ASSET_TYPE, set<wstring>> m_mapLoadingAssetFileExtensions = 
    {
        {ASSET_TYPE::MATERIAL,          {L".mtrl"}},
        {ASSET_TYPE::TEXTURE,           {L".png", L".jpg", L".jpeg", L".bmp", L".tga", L".dds"}},
        {ASSET_TYPE::SPRITE,            {L".sprite"}},
        {ASSET_TYPE::FLIPBOOK,          {L".flip"}},
        {ASSET_TYPE::TILEMAP,           {L".tilemap"}},
        {ASSET_TYPE::PREFAB,            {L".pref"}},
        {ASSET_TYPE::LEVEL,             {L".lv"}},
        {ASSET_TYPE::SOUND,             {}}, // TODO : Sound 파일 확장자들 추가할 것
    };
    
public:
    
    /// <summary>
    /// 필요한 Asset 미리 제작 -> Engine 코드로 직접 제작해야 하는 Asset들 AssetMgr에 추가
    /// </summary>
    void Init();

    /// <summary>
    /// 현재 m_Changed 상태 확인 & m_Changed false로 원상복구처리 
    /// </summary>
    bool IsChanged()
    {
        bool Changed = m_Changed;
        m_Changed = false;
        return Changed;
    }
    
private:

    void CreateEngineMesh();
    void CreateEngineShader();
    void CreateEngineMaterial();

private:

    /// <summary>
    /// 해당 AssetType의 폴더 안에서 해당 Type의 Asset들 하위 폴더 안까지 일괄 불러오기
    /// </summary>
    /// <returns> 불러올 수 없는 AssetType이라면 return false </returns>
    template<typename T>
    bool LoadAssetsFromAssetFolderRecursively();
    
    /// <summary>
    /// Level 파일 불러오기
    /// </summary>
    void LoadAllLevels();

private:
    
    /// <summary>
    /// Texture 메타데이터 모두 불러와서 맵에 저장
    /// </summary>
    void LoadAllTexMetaData();

    /// <summary>
    /// AssetMgr::Init 마지막 시점에, 이전에 지운 Texture 파일이 있다면, 대응되는 메타데이터 찾아서 지우기
    /// </summary>
    void RemoveAnyDeletedTexturesMetaData();
    
public:
    /// <summary>
    /// 해당 ASSET_TYPE과 wstring key에 해당하는 Ptr<Asset> 반환
    /// </summary>
    /// <returns> : Ptr<Asset> 반환, 없다면 return nullptr</returns>
    Ptr<Asset> Find(ASSET_TYPE _Type, const wstring& _Key, bool _ShowWarningLog = true);

    /// <summary>
    /// <para> T형의 key값에 해당하는 Asset 반환 </para>
    /// <para> Ptr<T> 자료형으로 반환처리 (호출 Client단에서 casting 처리하지 않기 위한 용이성) </para>
    /// </summary>
    /// <param name="_Key"> : const wstring& </param>
    /// <returns> : 없다면 return nullptr </returns>
    template<typename T>
    Ptr<T> Find(const wstring& _Key);

    template<typename T>
    Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);
    
    void GetAssetKeys(ASSET_TYPE _Type, vector<wstring>& _Vec);

    /// <summary>
    /// Texture FileHash 값으로 Guid 있는지 조사 
    /// </summary>
    /// <param name="_FileHash"></param>
    /// <param name="_OutGuid"></param>
    /// <returns> : 만약에 해당 FileHash에 대응되는 Guid가 없다면 return false </returns>
    bool GetTextureAssetGuidByFileHash(const uint64_t& _FileHash, OUT GUID& _OutGuid) const;
    
public:
    
    /// <summary>
    /// 해당 Key값으로 Asset 추가
    /// </summary>
    /// <param name="_Key"> : wstring </param>
    /// <param name="_Asset"> : Ptr<Asset> </param>
    void AddAsset(const wstring& _Key, Ptr<Asset> _Asset);
    
    /// <summary>
    /// 해당 Type의 해당 Key로 들어간 Asset을 AssetMgr에서 제거
    /// </summary>
    /// <param name="_Type"></param>
    /// <param name="_TargetKey"></param>
    /// <returns> : 해당하는 Asset이 없었다면 return false </returns>
    bool RemoveAsset(ASSET_TYPE _Type, const wstring& _TargetKey);
    
public:
    
    /// <summary>
    /// Asset 파일명만으로, Asset 파일의 경로 찾기
    /// </summary>
    /// <param name="_AssetFileName"> : 찾을 Asset file name </param>
    /// <param name="_OutPath"> : (_IsFullPath가 true이면)찾은 FullPath 경로 / 아니라면, Content폴더 안의 상대 경로 </param>
    /// <param name="_IsFullPath"> : 절대경로를 찾는 중이면 true </param>
    /// <returns> : 제대로 찾았다면 return true </returns>
    bool FindPathByAssetFileName(const wstring& _AssetFileName, OUT wstring& _OutPath, bool _IsFullPath = true);

    /// <summary>
    /// <para> Asset Guid 값으로, Asset 파일의 경로 찾기 </para>
    /// <para> 주의 : 해당 Guid를 확인하는 과정에서 해당 guid와 매핑된 파일이 이미 열린 상태였다면, 제대로 못 찾는다. </para>
    /// </summary>
    /// <param name="_AssetGuid"> : 찾을 Asset의 Guid </param>
    /// <param name="_OutPath"> : (_IsFullPath가 true이면)찾은 FullPath 경로 / 아니라면, Content폴더 안의 상대 경로 </param>
    /// <param name="_IsFullPath"> : 절대경로를 찾는 중이면 true </param>
    /// <returns> : 제대로 찾았다면 return true </returns>
    bool FindPathByAssetGuid(const GUID& _AssetGuid, OUT wstring& _OutPath, bool _IsFullPath = true);
    
    /// <summary>
    /// m_mapAsset에 있는 모든 Asset들 자기자신이 들고있는 RelativePath에 일괄 저장
    /// </summary>
    /// <returns> : 제대로 저장되었다면 S_OK, 아니라면 E_FAIL </returns>
    HRESULT SaveAllAssets();
    
public:
    
    /// <summary>
    /// 에셋 MetaData 저장하기 
    /// </summary>
    /// <param name="_Key"> : Asset의 키값 </param>
    /// <param name="_Guid"> : Asset의 Guid 값 </param>
    /// <returns></returns>
    // HRESULT SaveAssetMetaData(const wstring& _Key, const GUID& _Guid);

    /// <summary>
    /// T type과 AssetName에 따른 Asset Key 자동 생성
    /// 겹치는 키가 있다면, name 끝 suffix에 indexing으로 해당 문제를 피함
    /// </summary>
    /// <param name="_AssetName"> : Asset Key에 들어갈 Asset명 (확장자명은 제거해서 보내주어야 함)</param>
    template<typename T>
    Ptr<T> CreateNewAsset(const wstring& _AssetName);

    /// <summary>
    /// Editor 내에서 Asset 복제를 통한 Asset 생성 및 AddAsset 처리
    /// 받은 _OriginAsset의 Key에 Suffix indexing을 붙여서 생성 처리한다
    /// </summary>
    /// <param name="_OriginAsset"> : 복제하려는 기반 Asset 객체 </param>
    /// <returns> : 제대로 복제 처리가 되지 않았다면 return nullptr </returns>
    // Ptr<Asset> CreateNewAsset(const Ptr<Asset>& _OriginAsset) { return _OriginAsset->CreateNewAsset(); }

    /// <summary>
    /// T type과 AssetName에 따른 Asset Key 자동 생성 -> Suffix에  XY indexing을 붙여 NameCommon에 넘버링으로 여러 Asset 생성
    /// </summary>
    /// <param name="_AssetNameCommon"></param>
    /// <param name="_OutAssets"></param>
    /// <returns> : 만일 겹치는 키값이 있다면 생성 x return false </returns>
    template<typename T>
    bool CreateNewAssetsBySuffixXYCount(const wstring& _AssetNameCommon, vector<Ptr<T>>& _OutAssets, const Vec2& _SuffixXYCount);
    
};

template<typename T>
ASSET_TYPE GetAssetType()
{
    if      constexpr (is_same_v<T, AMesh>)             return ASSET_TYPE::MESH;
    else if constexpr (is_same_v<T, AGraphicShader>)    return ASSET_TYPE::GRAPHICS_SHADER;
    else if constexpr (is_same_v<T, ATexture>)          return ASSET_TYPE::TEXTURE;
    else if constexpr (is_same_v<T, AMaterial>)         return ASSET_TYPE::MATERIAL;
    else if constexpr (is_same_v<T, ASprite>)           return ASSET_TYPE::SPRITE;
    else if constexpr (is_same_v<T, AFlipbook>)         return ASSET_TYPE::FLIPBOOK;
    else if constexpr (is_same_v<T, ATileMap>)          return ASSET_TYPE::TILEMAP;
    else if constexpr (is_same_v<T, ALevel>)            return ASSET_TYPE::LEVEL;
    else if constexpr (is_same_v<T, APrefab>)           return ASSET_TYPE::PREFAB;
    else if constexpr (is_same_v<T, ASound>)            return ASSET_TYPE::SOUND;
    
    
    return ASSET_TYPE::END;
}

template <typename T>
bool AssetMgr::LoadAssetsFromAssetFolderRecursively()
{
    ASSET_TYPE Type = GetAssetType<T>();
    
    // 불러올 수 없는 Asset 종류
    if (!m_mapLoadingAssetFileExtensions.contains(Type)) return false;

    const wstring AssetTypeFolder = GetAssetTypeFolderWString(Type);
    
    // recursive_directory_iterator를 사용해 하위 폴더까지 일괄 탐색
    for (const auto& entry : filesystem::recursive_directory_iterator(CONTENT_PATH + AssetTypeFolder))
    {
        if (!entry.is_regular_file()) continue;
        
        filesystem::path filePath = entry.path();
        wstring extension = filePath.extension().wstring();

        // 확장자를 소문자로 일괄 변환
        transform(extension.begin(), extension.end(), extension.begin(), towlower);

        const set<wstring>& TargetSet = m_mapLoadingAssetFileExtensions.at(Type);
        
        if (!TargetSet.contains(extension)) continue; // Asset 파일이 아닌 다른 파일

        // 해당 Asset 파일 로딩하기
        
        // Level
        const wstring Assetkey      = AssetTypeFolder + filePath.filename().wstring(); // 에셋 키
        const wstring relativePath  = filesystem::relative(filePath, CONTENT_PATH).wstring(); // Content 폴더 기준의 상대 경로 추출
        Load<T>(Assetkey, relativePath);
    }
    
    return true;
}

template <typename T>
Ptr<T> AssetMgr::Find(const wstring& _Key)
{
    ASSET_TYPE Type = GetAssetType<T>();
    
    map<wstring, Ptr<Asset>>::iterator iter = m_mapAsset[static_cast<UINT>(Type)].find(_Key);

    return (iter == m_mapAsset[static_cast<UINT>(Type)].end()) ?
            nullptr : dynamic_cast<T*>(iter->second.Get());  
}

template <typename T>
Ptr<T> AssetMgr::Load(const wstring& _Key, const wstring& _RelativePath)
{
    // 동일키로 먼저 등록된 에셋이 있는지 확인
    Ptr<T> pAsset = Find<T>(_Key);
    
    // 동일키로 로딩된 Asset이 있다면 반환
    if (pAsset) return pAsset;
    
    // DebugUtil::AddDebugLog(L"[AssetMgr::Load] Above not found msg is irrelevant");

    // 에셋 객체 생성
    pAsset = new T;
    
    // 입력된 경로로부터 에셋 로딩작업 진행
    if (FAILED(pAsset->Load(CONTENT_PATH + _RelativePath)))
    {
        DebugUtil::AddDebugLog(L"[AssetMgr::Load] Failed to load asset " + _Key);
        return nullptr;
    }

    // T 타입에 해당하는 실제 AssetType 확인
    ASSET_TYPE type = GetAssetType<T>();
    
    // 맵에 에셋등록
    m_mapAsset[static_cast<UINT>(type)].insert(make_pair(_Key, pAsset.Get()));


    // 에셋이 자신이 매니저에 등록될 때 사용된 Key 와, 
    // 자신이 어떤 경로에 있는 파일로부터 로딩된 에셋인지 스스로 알 수 있도록 해줌
    pAsset->SetKey(_Key);
    pAsset->SetRelativePath(_RelativePath);
    
    m_Changed = true;
    
    return pAsset;
}

template <typename T>
Ptr<T> AssetMgr::CreateNewAsset(const wstring& _AssetName)
{
    const ASSET_TYPE Type = GetAssetType<T>();
    const wstring Key = GenerateNewAssetKeyBasedOnAssetName(Type, _AssetName);
    
    Ptr<T> NewAsset = new T;

    NewAsset->SetName(_AssetName);
    NewAsset->GetGuid();              // GUID 새로 생성
    NewAsset->SetKey(Key);            // Key 부여
    NewAsset->SetRelativePath(Key);   // RelativePath 값
    
    return NewAsset;
    // AddAsset(Key, _OutNewAsset.Get());
}

template <typename T>
bool AssetMgr::CreateNewAssetsBySuffixXYCount(const wstring& _AssetNameCommon, vector<Ptr<T>>& _OutAssets, const Vec2& _SuffixXYCount)
{
    _OutAssets.clear();
    ASSET_TYPE Type = GetAssetType<T>();

    const UINT yCount = _SuffixXYCount.y;
    const UINT xCount = _SuffixXYCount.x;
    
    const UINT yWidth = to_wstring((yCount == 0) ? 0 : yCount).size();
    const UINT xWidth = to_wstring((xCount == 0) ? 0 : xCount).size();

    auto MakePaddedSuffix = [](UINT value, UINT width) -> wstring
    {
        wstring s = to_wstring(value);
        if (s.size() < width)
            s.insert(0, width - s.size(), L'0');
        return s;
    };
    
    for (UINT y = 0; y < yCount; ++y)
    {
        for (UINT x = 0; x < xCount; ++x)
        {
            const wstring ySuffix = MakePaddedSuffix(y, yWidth);
            const wstring xSuffix = MakePaddedSuffix(x, xWidth);

            const wstring AssetName = _AssetNameCommon + ySuffix + L"_" + xSuffix;
            wstring Key{};
            bool bGenerated = GenerateNewAssetKeyBasedOnAssetName(GetAssetType<T>(), AssetName, Key);

            if (!bGenerated)
            {
                _OutAssets.clear();
                return false;
            }

            Ptr<T> pAsset = new T;

            pAsset->GetGuid();              // GUID 새로 생성
            pAsset->SetKey(Key);            // Key 부여   
            pAsset->SetRelativePath(Key);   // RelativePath 값
            _OutAssets.push_back(pAsset);
        }
    }

    return true;
}


template<typename T>
Ptr<T> LoadAssetRef(FILE* _File)
{
    // Asset이 nullptr인지 아닌지 확인
    bool IsValid{};
    fread(&IsValid, sizeof(bool), 1, _File);
    
    if (!IsValid) return nullptr;
    
    // 기존의 AssetRef가 Valid했던 상황 (nullptr가 아니였었던 상황)
    
    GUID AssetGuid{}; fread(&AssetGuid, sizeof(GUID), 1, _File); // Guid 불러오기
    const wstring LastlySavedKey  = LoadWString(_File);
    
    // 여기서 Key값은 맞는데, RelativePath가 옮겨진 Asset일 수도 있음 -> RelativePath를 다시 맞게끔 잡아주어야함
    // 번외 : 엔진 제공 Asset의 경우, RelativePath는 공란으로 저장되어
    // filesystem::exists(CONTENT_PATH)로 적용되어, 바로 Load처리로 들어감(m_mapAssets에 만들어진 엔진 제공 Asset으로 로딩 처리가 잘된다)
    const wstring RelativePath    = LoadWString(_File);
    
    filesystem::path FullPath = filesystem::path(CONTENT_PATH) / RelativePath;
    
    if (filesystem::exists(FullPath)) 
        return AssetMgr::GetInst()->Load<T>(LastlySavedKey, RelativePath);
    
    // Invalid 한 경로 (이전에 Asset 폴더 경로를 옮겼으면, 여기로 들어와서 다시 찾아본다)
    
    const wstring FileName = filesystem::path(FullPath).filename().wstring();
    wstring NewRelativePath{};
    
    if (AssetMgr::GetInst()->FindPathByAssetFileName(FileName, NewRelativePath, false))
        return AssetMgr::GetInst()->Load<T>(LastlySavedKey, NewRelativePath); // 새로이 찾은 RelativePath로 Asset 로딩 처리
    
    // 경로의 수정과 무관하게 Asset명까지 바꾼 상황이라면, Asset의 Guid로 Asset을 한 번 더 찾아본다
    
    const bool bFound = AssetMgr::GetInst()->FindPathByAssetGuid(AssetGuid, NewRelativePath, false);
    
    if (bFound)
    {
        // 바뀐 파일명에 대한 새로운 키값 제작
        const wstring CurrentKey = GetAssetTypeFolderWString(GetAssetType<T>()) + GetFileName(NewRelativePath);
        return AssetMgr::GetInst()->Load<T>(CurrentKey, NewRelativePath);
    }
    
    // 최후 Guid까지 확인 했을 때, 찾지 못하였다면 nullptr 반환
    return nullptr; 
}

#define FIND_ASSET(Type, Key) AssetMgr::GetInst()->Find<Type>(Key)
#define LOAD_ASSET(Type, AssetPath) AssetMgr::GetInst()->Load<Type>(AssetPath, AssetPath)