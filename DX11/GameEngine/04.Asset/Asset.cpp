#include "pch.h"
#include "Asset.h"

#include "GameEngine/03.Manager/01.PathMgr/PathMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"


Asset::Asset(ASSET_TYPE _Type)
    : m_Type(_Type)
{
}

Asset::Asset(const Asset& _Origin)
    : Entity(_Origin) // 주의 : 부모쪽 생성자를 명시하지 않으면, 부모의 기본생성자를 호출함 / 명시적으로 복사생성자 호출 처리
    , m_Key(_Origin.m_Key)
    , m_Type(_Origin.m_Type)
    , m_IsProvidedByEngine(_Origin.m_IsProvidedByEngine)
    //, m_Guid(_Origin.m_Guid)
{
    GetGuid(); // 새 Guid로 Guid 초기화
}

Asset::~Asset()
{
}

GUID Asset::GetGuid()
{
    if (IsEqualGUID(m_Guid, GUID_NULL))
    {
        if (FAILED(CoCreateGuid(&m_Guid)))
            DebugUtil::AddDebugLog(L"[Asset::GetGuid] : Guid creation failed!");
    }
    
    return m_Guid;
}

HRESULT Asset::Save(const wstring& _FilePath)
{
    // 엔진 제공 Asset 콘텐츠인지 확인
    if (GetIsProvidedByEngine()) return E_FAIL; 
    
    FILE* pFile{}; // 파일스트림 커널

    // write binary 쓰기모드
    errno_t err = _wfopen_s(&pFile, _FilePath.c_str(), L"wb"); 
    
    if (err != 0 || !pFile)
    {
        wchar_t szErrMsg[256]{};
        _wcserror_s(szErrMsg, err);

        DebugUtil::AddDebugLog(L"[Asset::Save] :" + wstring(szErrMsg));
        return E_FAIL;
    }
    
    fwrite(&GetGuidRef(), sizeof(GUID), 1, pFile); // Asset Guid 저장
    
    // AssetMgr::GetInst()->SaveAssetMetaData(m_Key, m_Guid); // Guid MetaData 저장
    
    fclose(pFile);
    return S_OK;
}

void Asset::SetKey(const wstring& _Key)
{
    if (m_Key == _Key) return;
    
    wstring PrevKey = m_Key;
    m_Key = _Key;
    
    // AssetMgr에 이미 들어가있는 Asset인 경우였을 때, AssetMgr에서 다시금 새로운 Key로 잡아주고 RelativePath도 일괄적으로 잡아준다
    if (AssetMgr::GetInst()->Find(GetType(), PrevKey, false) == this)
    {
        if (AssetMgr::GetInst()->RemoveAsset(GetType(), PrevKey))
        {
            AssetMgr::GetInst()->AddAsset(m_Key, this);
            
            // 변한 Key값을 통해 RelativePath 또한 수정 처리해준다.
            filesystem::path srcPath(m_Key);
            filesystem::path dstPath(m_RelativePath);
            
            filesystem::path srcFileName = srcPath.filename();
            dstPath.replace_filename(srcFileName);
            
            m_RelativePath = dstPath.wstring();
        }
        else assert(nullptr);
    }
}

GUID& Asset::GetGuidRef()
{
    if (IsEqualGUID(m_Guid, GUID_NULL))
    {
        if (FAILED(CoCreateGuid(&m_Guid)))
            DebugUtil::AddDebugLog(L"[Asset::GetGuid] : Guid creation failed!");
    }
    
    return m_Guid;
}

HRESULT Asset::Load(const wstring& _FilePath)
{
    if (GetIsProvidedByEngine()) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"rb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[Asset::Load] : Open File failed!");
        return E_FAIL;
    }
    
    fread(&GetGuidRef(), sizeof(GUID), 1, pFile);

    fclose(pFile);
    return S_OK;
}

HRESULT Asset::SaveBySelfRelativePath()
{
    return Save(CONTENT_PATH + m_RelativePath);
}


