#include "pch.h"
#include "ATexture.h"

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"

ATexture::ATexture()
    : Asset(ASSET_TYPE::TEXTURE)
    , m_RecentRegisterNum(-1)
{
}

ATexture::~ATexture() 
{
}

void ATexture::Binding(UINT _RegisterNum)
{
    m_RecentRegisterNum = _RegisterNum;
    
    // 어떠한 pipeline에서도 t0 register에 등록되어 있을 것이다.
    CONTEXT->VSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->GSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->HSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->DSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
    CONTEXT->PSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void ATexture::Clear()
{
    if (-1 == m_RecentRegisterNum) return;
    
    ID3D11ShaderResourceView* pSRV = nullptr;
    CONTEXT->VSSetShaderResources(m_RecentRegisterNum, 1, &pSRV);
    CONTEXT->GSSetShaderResources(m_RecentRegisterNum, 1, &pSRV);
    CONTEXT->HSSetShaderResources(m_RecentRegisterNum, 1, &pSRV);
    CONTEXT->DSSetShaderResources(m_RecentRegisterNum, 1, &pSRV);
    CONTEXT->PSSetShaderResources(m_RecentRegisterNum, 1, &pSRV);
}

HRESULT ATexture::Save(const wstring& _strFilePath)
{
    // Texture 용 MetaData 먼저 저장
    
    // FileHash 값이 초기화 되지 않은 Asset -> 파일해시값 계산해서 넣어주기
    if (m_FileHash == 0) m_FileHash = CalculateFileHash64(_strFilePath);
    
    FILE* pFile{}; // 파일스트림 커널
    // 추후 Texture 파일명을 바꿨을 때, 기존의 metadata가 남아서 쌓일 수 있기 때문에 고유의 파일해시값으로 파일명을 잡아줌
    const wstring MetaFilePath = CONTENT_PATH + L"\\_Meta\\_TextureMeta\\" + to_wstring(m_FileHash) + L".texmeta";
    
    // Texture 메타파일 저장
    
    if (_wfopen_s(&pFile, MetaFilePath.c_str(), L"wb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[ATexture::Save] : Open MetaFile failed!");
        return E_FAIL;
    }

    fwrite(&m_FileHash,     sizeof(uint64_t),   1, pFile);    // 파일해시값 저장
    fwrite(&GetGuidRef(),   sizeof(GUID),       1, pFile);      // Asset Guid 저장
    
    fclose(pFile);
    
    return S_OK;
}

HRESULT ATexture::Load(const wstring& _strFilePath)
{
    // MetaData에서 Asset Guid 및 파일해시값 찾기
    
    m_FileHash = CalculateFileHash64(_strFilePath);

    GUID Guid{};
    if (!AssetMgr::GetInst()->GetTextureAssetGuidByFileHash(m_FileHash, Guid))
    {
        // 해당 FileHash값에 대응하는 메타 데이터가 없었던 상황 (새로 추가된 Texture)
        // 새로운 Guid 할당해서 메타데이터 파일 새로 저장
        GetGuid();
        this->Save(_strFilePath); 
    }
    else SetGuid(Guid);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    wchar_t szExt[10]{};
    _wsplitpath_s
    (
        _strFilePath.c_str(),
        nullptr, 0,
        nullptr, 0,
        nullptr, 0,
        szExt,   10
    );
    
    wstring strExt(szExt);
    HRESULT hr = S_OK;
    if (L"dds" == strExt)       hr = LoadFromDDSFile(_strFilePath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_Image);
    else if (L"tga" == strExt)  hr = LoadFromTGAFile(_strFilePath.c_str(), nullptr, m_Image);
        
    // WIC(Window Image Component) .png, .jpg, .jpeg, .bmp
    else hr = LoadFromWICFile(_strFilePath.c_str(), TEXTURE_LOADING_FLAG, nullptr, m_Image); // TODO : 만일 Texture 색상이 이상하게 나오면 플래그 다른거 넣어볼 것
        
    if (FAILED(hr))
    {
        // 해당 경로로 찾기 실패 -> 파일명만 따로 떼어 다시금 검색해서 만일 이번에도 나오지 않으면 return E_FAIL 처리
        const wstring FileName = filesystem::path(_strFilePath).filename().wstring();
        wstring FullPath{};
        
        const bool bFound = AssetMgr::GetInst()->FindPathByAssetFileName(FileName, FullPath);

        if (bFound) return Load(FullPath); // 제대로 된 경로를 찾았고, 해당 경로로 다시금 에셋 로드 실행
        
        MessageBox(nullptr ,L"텍스처 시스템메모리 로딩 실패", L"텍스쳐 로딩 실패", MB_OK); // 제대로된 경로가 아님 -> 경로를 다시 찾아서 잡아주어야 함
        return E_FAIL;
    }

    // SysMem        -> GPU 과정
    // ScratchImage  -> Texture2D 생성

    // View 생성
    // Texture2D    -> RTV
    //              -> DSV
    //              -> SRV
    
    // 위의 View를 생성하는 과정을 DirectxTex 라이브러리의 함수로 처리가 가능하다
    // ScratchImage에 로딩된 이미지 데이터를 기반으로 Texture2D를 생성하고,
    // 다시 이걸로 ShaderResourceView 까지 만들어서 ShaderResourceView 주소를 알려줌
    hr = CreateShaderResourceView
    (
        DEVICE, m_Image.GetImages(),
        m_Image.GetImageCount(), m_Image.GetMetadata(),
        m_SRV.GetAddressOf()
    );
    if (FAILED(hr)) return E_FAIL;

    // 생성된 SRV를 이용해서, 먼저 만들어진 Texture2D의 주소를 알아냄
    m_SRV->GetResource(reinterpret_cast<ID3D11Resource**>(m_Tex2D.GetAddressOf()));

    // Texture2D를 생성할 때 세팅한 Desc 옵션정보를 알아냄 (Texture 이미지의 Width, Height 등)
    m_Tex2D->GetDesc(&m_Desc);  
    
    return S_OK;
}
