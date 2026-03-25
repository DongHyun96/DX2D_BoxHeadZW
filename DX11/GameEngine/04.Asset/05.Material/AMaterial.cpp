#include "pch.h"
#include "AMaterial.h"

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"


AMaterial::AMaterial()
    : Asset(ASSET_TYPE::MATERIAL)
    , m_Const{}
    , m_Domain(RENDER_DOMAIN::DOMAIN_NONE)
{
    m_Const.iArr[0] = 100;
}

AMaterial::AMaterial(const AMaterial& _Other)
    : Asset(ASSET_TYPE::MATERIAL)
    , m_Shader(_Other.m_Shader)
    , m_Tex{}
    , m_Const(_Other.m_Const)
    , m_Domain(_Other.m_Domain)
{
    for (int i = 0; i < TEX_END; ++i)
        m_Tex[i] = _Other.m_Tex[i];
}

AMaterial::~AMaterial()
{
}

HRESULT AMaterial::Save(const wstring& _FilePath)
{
    if (FAILED(Asset::Save(_FilePath))) return E_FAIL; 
        
    
    FILE* pFile{}; // 파일스트림 커널

    // write binary 쓰기모드
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"wb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[AMaterial::Save] : Open File failed!");
        return E_FAIL;
    }

    // 커널
    
    // 재질이 사용하는 쉐이더 파이프라인 정보
    SaveAssetRef(pFile, m_Shader.Get());
    
    // 파이프라인 동작 시, 어떤 텍스쳐를 전달하기로 했었는지
    for (const Ptr<ATexture>& Texture : m_Tex)
        SaveAssetRef(pFile, Texture.Get());
    
    // 파이프라인 동작 시, 전달한 상수 데이터
    fwrite(&m_Const, sizeof(MtrlConst), 1, pFile);
    
    // 렌더링 시점, 도메인
    fwrite(&m_Domain, sizeof(RENDER_DOMAIN), 1, pFile);

    fclose(pFile);
    
    return S_OK;
}

HRESULT AMaterial::Load(const wstring& _FilePath)
{
    if (FAILED(Asset::Load(_FilePath))) return E_FAIL;
    
    FILE* pFile{};
    if (_wfopen_s(&pFile, _FilePath.c_str(), L"rb") != 0 || !pFile)
    {
        DebugUtil::AddDebugLog(L"[AMaterial::Load] : Open File failed!");
        return E_FAIL;
    }
    
    // 커널
    
    // 재질이 사용하는 쉐이더 파이프라인 정보
    m_Shader = LoadAssetRef<AGraphicShader>(pFile);

    if (GetKey() == L"Material\\Cliff1Material.mtrl")
    {
        int a = 0;
    }
    
    // 파이프라인 동작 시, 어떤 텍스쳐를 전달하기로 했었는지
    for (Ptr<ATexture>& Texture : m_Tex)
        Texture = LoadAssetRef<ATexture>(pFile);
    
    // 파이프라인 동작 시, 전달한 상수 데이터
    fread(&m_Const, sizeof(MtrlConst), 1, pFile);
    
    // 렌더링 시점, 도메인
    fread(&m_Domain, sizeof(RENDER_DOMAIN), 1, pFile);
    
    fclose(pFile);

    return S_OK;
}

AMaterial* AMaterial::Clone() const
{
    return new AMaterial(*this); // 복사생성으로 생성한 재질 객체 반환
}

void AMaterial::Binding()
{
    m_Shader->Binding();

    for (int i = 0; i < TEX_END; ++i)
    {
        if (!m_Tex[i])
        {
            m_Const.IsTex[i] = false;
            continue;
        }
        m_Const.IsTex[i] = true;
        m_Tex[i]->Binding(i);
    }

    /*m_Const.iArr[0];
    m_Const.iArr[1];
    m_Const.iArr[2];
    m_Const.iArr[3];*/
    
    Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->SetData(&m_Const);
    Device::GetInst()->GetCB(CB_TYPE::MATERIAL)->Binding();
}

void AMaterial::Clear()
{
    for (int i = 0; i < TEX_END; ++i)
    {
        if (!m_Tex[i]) continue;
        m_Tex[i]->Clear();
    }    
}
