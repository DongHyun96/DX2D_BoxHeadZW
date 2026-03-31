#include "pch.h"
#include "AssetMgr.h"

#include "GameEngine/03.Manager/01.PathMgr/PathMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"

#include "Header/assets.h"

namespace fs = filesystem;

AssetMgr::AssetMgr()
{
}

AssetMgr::~AssetMgr()
{
}

Ptr<Asset> AssetMgr::Find(ASSET_TYPE _Type, const wstring& _Key, bool _ShowWarningLog)
{
    map<wstring, Ptr<Asset>>::iterator iter = m_mapAsset[static_cast<UINT>(_Type)].find(_Key);

    if (iter == m_mapAsset[static_cast<UINT>(_Type)].end())
    {
        if (_ShowWarningLog) DebugUtil::AddDebugLog(L"[AssetMgr::Find] Asset not found. Key = " + _Key );
        return nullptr;
    }

    return iter->second;
}

void AssetMgr::LoadAllLevels()
{
     // recursive_directory_iterator를 사용해 하위 폴더까지 일괄 탐색
    for (const auto& entry : fs::recursive_directory_iterator(CONTENT_PATH)) 
    {
        if (entry.is_regular_file()) 
        {
            fs::path filePath = entry.path();
            wstring extension = filePath.extension().wstring();

            // 확장자를 소문자로 일괄 변환 (이거 빼도 될듯?)
            transform(extension.begin(), extension.end(), extension.begin(), towlower);

            if (extension != L".lv") continue;
            
            // Level
            const wstring Assetkey      = L"Level\\" + filePath.filename().wstring(); // 에셋 키
            const wstring relativePath  = fs::relative(filePath, CONTENT_PATH).wstring(); // Content 폴더 기준의 상대 경로 추출
            Load<ALevel>(Assetkey, relativePath);
        }
    }
}

void AssetMgr::LoadAllTexMetaData()
{
    const wstring TexMetaDataFolderPath = CONTENT_PATH + L"\\_Meta\\_TextureMeta";
    
    for (const auto& entry : fs::directory_iterator(TexMetaDataFolderPath))
    {
        fs::path filePath = entry.path();
        wstring extension = filePath.extension().wstring();

        // 확장자를 소문자로 일괄 변환
        transform(extension.begin(), extension.end(), extension.begin(), towlower);

        if (extension != L".texmeta") continue;

        FILE* pFile{};
        
        if (_wfopen_s(&pFile, filePath.c_str(), L"rb") != 0 || !pFile)
        {
            DebugUtil::AddDebugLog(L"[AssetMgr::LoadAllTexMetaData] : Open File failed!");
            continue;
        }

        GUID Guid{}; uint64_t FileHash{};
        
        fread(&FileHash,    sizeof(uint64_t),   1, pFile);
        fread(&Guid,        sizeof(GUID),       1, pFile);
        fclose(pFile);

        m_mapTexMetaData.insert(make_pair(FileHash, Guid));
    }
}

void AssetMgr::RemoveAnyDeletedTexturesMetaData()
{
    // 불러온 모든 메타데이터 정보와 Texture Asset들 비교하기
    const auto& texMap = m_mapAsset[static_cast<UINT>(ASSET_TYPE::TEXTURE)];
    set<uint64_t> liveHashes{};

    for (const pair<const wstring, Ptr<Asset>>& Pair : texMap)
    {
        const Ptr<Asset>& AssetPtr = Pair.second;
        if (!AssetPtr) continue;

        const wstring FullPath  = CONTENT_PATH + AssetPtr->GetRelativePath();
        const uint64_t FileHash = CalculateFileHash64(FullPath);

        if (FileHash == 0)
        {
            DebugUtil::AddDebugLog(L"[AssetMgr::RemoveAnyDeletedTexturesMetaData] File hash failed: " + FullPath);
            continue;
        }

        liveHashes.insert(FileHash);
    }

    const wstring MetaFolderPath = CONTENT_PATH + L"\\_Meta\\_TextureMeta\\";

    for (auto iter = m_mapTexMetaData.begin(); iter != m_mapTexMetaData.end();)
    {
        const uint64_t MetaFileHash = iter->first;

        if (liveHashes.find(MetaFileHash) == liveHashes.end())
        {
            const wstring TargetMetaFilePath = MetaFolderPath + to_wstring(MetaFileHash) + L".texmeta";
            RemoveWindowFile(TargetMetaFilePath);
            iter = m_mapTexMetaData.erase(iter);
            continue;
        }

        ++iter;
    }
}

void AssetMgr::LoadAllSoundMetaData()
{
    const wstring SoundMetaDataFolderPath = CONTENT_PATH + L"\\_Meta\\_SoundMeta";
    
    for (const auto& entry : fs::directory_iterator(SoundMetaDataFolderPath))
    {
        fs::path filePath = entry.path();
        wstring extension = filePath.extension().wstring();

        // 확장자를 소문자로 일괄 변환
        transform(extension.begin(), extension.end(), extension.begin(), towlower);

        if (extension != L".soundmeta") continue;

        FILE* pFile{};
        
        if (_wfopen_s(&pFile, filePath.c_str(), L"rb") != 0 || !pFile)
        {
            DebugUtil::AddDebugLog(L"[AssetMgr::LoadAllSoundMetaData] : Open File failed!");
            continue;
        }

        GUID Guid{}; uint64_t FileHash{};
        
        fread(&FileHash,    sizeof(uint64_t),   1, pFile);
        fread(&Guid,        sizeof(GUID),       1, pFile);
        fclose(pFile);

        m_mapSoundMetaData.insert(make_pair(FileHash, Guid));
    }
}

void AssetMgr::RemoveAnyDeletedSoundMetaData()
{
    // 불러온 모든 메타데이터 정보와 Texture Asset들 비교하기
    const auto& soundMap = m_mapAsset[static_cast<UINT>(ASSET_TYPE::SOUND)];
    set<uint64_t> liveHashes{};

    for (const pair<const wstring, Ptr<Asset>>& Pair : soundMap)
    {
        const Ptr<Asset>& AssetPtr = Pair.second;
        if (!AssetPtr) continue;

        const wstring FullPath  = CONTENT_PATH + AssetPtr->GetRelativePath();
        const uint64_t FileHash = CalculateFileHash64(FullPath);

        if (FileHash == 0)
        {
            DebugUtil::AddDebugLog(L"[AssetMgr::RemoveAnyDeletedSoundsMetaData] File hash failed: " + FullPath);
            continue;
        }

        liveHashes.insert(FileHash);
    }

    const wstring MetaFolderPath = CONTENT_PATH + L"\\_Meta\\_SoundMeta\\";

    for (auto iter = m_mapSoundMetaData.begin(); iter != m_mapSoundMetaData.end();)
    {
        const uint64_t MetaFileHash = iter->first;

        if (liveHashes.find(MetaFileHash) == liveHashes.end())
        {
            const wstring TargetMetaFilePath = MetaFolderPath + to_wstring(MetaFileHash) + L".soundmeta";
            RemoveWindowFile(TargetMetaFilePath);
            iter = m_mapSoundMetaData.erase(iter);
            continue;
        }

        ++iter;
    }
}


void AssetMgr::GetAssetKeys(ASSET_TYPE _Type, vector<wstring>& _Vec)
{
    for (const auto& Pair : m_mapAsset[static_cast<UINT>(_Type)])
        _Vec.push_back(Pair.first);
}

bool AssetMgr::GetTextureAssetGuidByFileHash(const uint64_t& _FileHash, GUID& _OutGuid) const
{
    // auto iter = m_mapTexMetaData[static_cast<UINT>(Type)].find(_Key);
    auto iter = m_mapTexMetaData.find(_FileHash);

    // if (iter == m_mapAsset[static_cast<UINT>(Type)].end())
    if (iter == m_mapTexMetaData.end())
    {
        // 여기에 들어오면 안됨
        DebugUtil::AddDebugLog(L"[AssetMgr::GetTextureAssetGuidByFilePath] Invalid FileHash received!", DEF_COLOR_WHITE, 10.f);
        _OutGuid = GUID_NULL;
        return false;
    }
    
    _OutGuid = m_mapTexMetaData.at(_FileHash);
    return true;
}

bool AssetMgr::GetSoundAssetGuidByFileHash(const uint64_t& _FileHash, GUID& _OutGuid) const
{
    auto iter = m_mapSoundMetaData.find(_FileHash);

    // if (iter == m_mapAsset[static_cast<UINT>(Type)].end())
    if (iter == m_mapSoundMetaData.end())
    {
        DebugUtil::AddDebugLog(L"[AssetMgr::GetSoundAssetGuidByFileHash] Invalid FileHash received!", DEF_COLOR_WHITE, 10.f);
        _OutGuid = GUID_NULL;
        return false;
    }
    
    _OutGuid = m_mapSoundMetaData.at(_FileHash);
    return true;
}

void AssetMgr::AddAsset(const wstring& _Key, Ptr<Asset> _Asset)
{
    map<wstring, Ptr<Asset>>& targetMap = m_mapAsset[static_cast<UINT>(_Asset->GetType())];
    assert(targetMap.find(_Key) == targetMap.end());

    _Asset->SetKey(_Key);
    
    targetMap.insert(make_pair(_Key, _Asset.Get()));
    
    m_Changed = true;
}

bool AssetMgr::RemoveAsset(ASSET_TYPE _Type, const wstring& _TargetKey)
{
    // 해당 Type의 해당 TargetKey를 가진 Asset이 없었다.
    if (!m_mapAsset[static_cast<UINT>(_Type)].contains(_TargetKey)) return false; 

    // 제거할 Asset이 존재하는 상황
    m_Changed = true;
    Ptr<Asset> TargetAsset = m_mapAsset[static_cast<UINT>(_Type)].at(_TargetKey);

    // 해당 Asset map에서 삭제
    m_mapAsset[static_cast<UINT>(_Type)].erase(_TargetKey);
    
    // 제거할 Asset이 Texture 타입이라면, TexMetaData에서도 해당 데이터를 지운다.
    for (auto it = m_mapTexMetaData.begin(); it != m_mapTexMetaData.end(); ++it)
    {
        if (it->second == TargetAsset->GetGuid())
        {
            m_mapTexMetaData.erase(it);
            return true;
        }
    }
    
    return true;
}

bool AssetMgr::FindPathByAssetFileName(const wstring& _AssetFileName, wstring& _OutPath, bool _IsFullPath)
{
    _OutPath = L"";

    // recursive_directory_iterator를 사용해 하위 폴더까지 일괄 탐색
    for (const auto& entry : fs::recursive_directory_iterator(CONTENT_PATH))
    {
        if (entry.is_regular_file())
        {
            fs::path FilePath = entry.path();
            const wstring FileName = FilePath.filename().wstring();

            // 일치하는 파일명을 찾음
            if (FileName == _AssetFileName)
            {
                if (_IsFullPath) _OutPath = _IsFullPath;
                else             _OutPath = fs::relative(FilePath, CONTENT_PATH).wstring();
                return true;
            }
        }
    }
    
    return false;
}

bool AssetMgr::FindPathByAssetGuid(const GUID& _AssetGuid, wstring& _OutPath, bool _IsFullPath)
{
    _OutPath = L"";

    // recursive_directory_iterator를 사용해 하위 폴더까지 일괄 탐색
    for (const auto& entry : fs::recursive_directory_iterator(CONTENT_PATH))
    {
        if (entry.is_regular_file())
        {
            fs::path FilePath = entry.path();
            const wstring FileName = FilePath.filename().wstring();
            
            const wstring extension = FilePath.extension().wstring();

            FILE* pFile{};
            
            if (_wfopen_s(&pFile, FilePath.wstring().c_str(), L"rb") != 0 || !pFile)
            {
                DebugUtil::AddDebugLog(L"[AssetMgr::FindPathByAssetGuid] : Open File failed!, Prolly not the asset currently looking for...");
                continue;
            }
            
            GUID AssetGuid{};
            
            // Texture의 경우 예외처리, Guid 대신 파일해시값으로 찾아야함
            if (extension == L".png" || extension == L".jpg" || extension == L".jpeg" ||
                extension == L".bmp" || extension == L".tga" || extension == L".dds")
            {
                // 가장 처음 Init에서 초기화시킨 파일해시값을 토대로 특정 Texture 파일을 찝어서 걔를 특정시켜야 함
                uint64_t FileHash = CalculateFileHash64(FilePath.wstring());
                if (!m_mapTexMetaData.contains(FileHash)) return false; // 해당 Texture 메타데이터가 메모리에 올라가지 않은 상황 // TODO : 원래는 assert 걸어서 터치게 했었음 맞는지 확인할 것
                
                AssetGuid = m_mapTexMetaData.at(FileHash);
            }
            else fread(&AssetGuid, sizeof(GUID), 1, pFile); // 일반 Asset 바이너리 파일일 경우
            
            // Guid가 일치하는 파일을 찾음
            if (_AssetGuid == AssetGuid)
            {
                if (_IsFullPath) _OutPath = _IsFullPath;
                else             _OutPath = fs::relative(FilePath, CONTENT_PATH).wstring();
                
                fclose(pFile);
                return true;
            }
            
            fclose(pFile);
        }
    }
    
    return false;
}

HRESULT AssetMgr::SaveAllAssets()
{
    // Level이 Stop 상태가 아니라면 전체 저장 불가능하도록 처리
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP) return E_FAIL;
    
    HRESULT SaveResult = S_OK;
    
    for (UINT i = 0; i < static_cast<UINT>(ASSET_TYPE::END); ++i)
    {
        for (const pair<const wstring, Ptr<Asset>>& Pair : m_mapAsset[i])
        {
            // 엔진에서 자체 생성, 제공하는 에셋이 아니고, 저장 처리가 제대로 안된 상황
            if (!Pair.second->GetIsProvidedByEngine() && FAILED(Pair.second->SaveBySelfRelativePath()))
            {
                SaveResult = E_FAIL;
                DebugUtil::AddDebugLog(L"[AssetMgr::SaveAllAssets] : Saving " + Pair.second->GetKey() + L" Failed!");
            }
        }
    }
    
    return SaveResult;
}

void AssetMgr::StopAllSounds()
{
    for (const pair<const wstring, Ptr<Asset>>& assetPair : m_mapAsset[static_cast<int>(ASSET_TYPE::SOUND)])
    {
        ASound* Sound = dynamic_cast<ASound*>(assetPair.second.Get());
        Sound->Stop();
    }
}

/*HRESULT AssetMgr::SaveAssetMetaData(const wstring& _Key, const GUID& _Guid)
{
    if (IsEqualGUID(_Guid, GUID_NULL)) return E_FAIL; // 초기화되지 않은 Guid로 MetaData 저장 시도
        
    
    const wstring MetaFolderPath = CONTENT_PATH + L"\\_Meta";
    const wstring FilePath = MetaFolderPath + _Key;

    FILE* pFile{};
    
    if (_wfopen_s(&pFile, FilePath.c_str(), L"wb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[AssetMgr::SaveAssetMetaData] : Open File failed!");
        return E_FAIL;
    }
    
    SaveWString(pFile, _Key);
    fwrite(&_Guid, sizeof(GUID), 1, pFile);
}*/

/*
void AssetMgr::LoadAssetMetaData()
{
    wstring MetaFolderPath = CONTENT_PATH + L"\\_Meta";

    // 메타폴더에 저장된 마지막으로 저장시켰던 metadata 읽기
    for (const auto& entry : fs::directory_iterator(MetaFolderPath))
    {
        if (entry.is_regular_file())
        {
            fs::path filePath = entry.path();
            wstring extension = filePath.extension().wstring();
            
            // 확장자를 소문자로 일괄 변환
            transform(extension.begin(), extension.end(), extension.begin(), towlower);
            
            FILE* pFile{}; // 파일스트림 커널

            // write binary 쓰기모드
            if (_wfopen_s(&pFile, filePath.wstring().c_str(), L"wb") != 0 || !pFile)
            {
                DebugUtil::AddDebugLog(L"[AssetMgr::LoadAssetMetaData] : Open File failed!");
                continue;
            }
            
            if (extension != L".meta")
            {
                fclose(pFile);
                continue; // 메타데이터 파일만 확인
            }
            
            const wstring Assetkey = LoadWString(pFile);
            GUID guid{}; 
            fread(&guid, sizeof(GUID), 1, pFile);
            
            m_mapLastSavedMetaData[Assetkey] = guid;
            
            fclose(pFile);
        }
    }
}
*/
