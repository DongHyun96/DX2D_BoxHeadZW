#include "pch.h"
#include "AGraphicShader.h"

#include "GameEngine/02.Device/Device.h"
#include "GameEngine/03.Manager/01.PathMgr/PathMgr.h"

AGraphicShader::AGraphicShader()
    : Asset(ASSET_TYPE::GRAPHICS_SHADER)
    , m_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
    , m_BSType(BS_TYPE::DEFAULT)
    , m_RSType(RS_TYPE::CULL_BACK)
    , m_DSType(DS_TYPE::LESS)
{
}

AGraphicShader::~AGraphicShader()
{
}


HRESULT AGraphicShader::CreateVertexShader(const wstring& _RelativeFilePath, const string& _FuncName)
{
    // VS 
    // 컴파일할 VertexShader 함수가 작성되어있는 파일의 절대 경로
    
    const wstring Path = CONTENT_PATH + _RelativeFilePath;
    
    HRESULT HR = D3DCompileFromFile
    (
        Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
        _FuncName.c_str(), "vs_5_0", D3D10_SHADER_DEBUG,
        0, m_VSBlob.GetAddressOf(), nullptr 
    );
    
    if (FAILED(HR)) return E_FAIL;
    
    if (FAILED(DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize(), nullptr, m_VS.GetAddressOf())))
        return E_FAIL;
    
    
    // Input Layout 생성하기
    /*D3D11_INPUT_ELEMENT_DESC InputDesc[3]{};

    InputDesc[0].SemanticName           = "POSITION";                  // Semantic 이름 지정
    InputDesc[0].SemanticIndex          = 0;                           // 시멘틱 이름이 중복된 경우, 구별하기 위한 숫자
    InputDesc[0].AlignedByteOffset      = 0;                           // 메모리 시작 위치(Offset)
    InputDesc[0].Format                 = DXGI_FORMAT_R32G32B32_FLOAT; // Offset으로부터 크기
    InputDesc[0].InputSlot              = 0;                           // 설명하는 정점이 들어있는 버퍼의 위치
    InputDesc[0].InputSlotClass         = D3D11_INPUT_PER_VERTEX_DATA; // 설명하는 정점이 들어있는 버퍼의 위치
    InputDesc[0].InstanceDataStepRate   = 0;                           // 인스턴스당 증가하는 오프셋(인스턴스상에 사용)
    
    InputDesc[1].SemanticName           = "TEXCOORD";                  
    InputDesc[1].SemanticIndex          = 0;                            
    InputDesc[1].AlignedByteOffset      = 12;                           
    InputDesc[1].Format                 = DXGI_FORMAT_R32G32_FLOAT;     
    InputDesc[1].InputSlot              = 0;  
    InputDesc[1].InputSlotClass         = D3D11_INPUT_PER_VERTEX_DATA;  
    InputDesc[1].InstanceDataStepRate   = 0;                      
    
    InputDesc[2].SemanticName           = "COLOR";                          
    InputDesc[2].SemanticIndex          = 0;                                
    InputDesc[2].AlignedByteOffset      = 20;   
    InputDesc[2].Format                 = DXGI_FORMAT_R32G32B32A32_FLOAT;   
    InputDesc[2].InputSlot              = 0;      
    InputDesc[2].InputSlotClass         = D3D11_INPUT_PER_VERTEX_DATA;      
    InputDesc[2].InstanceDataStepRate   = 0;

    // 레이아웃을 사용할 VertexShader의 코드 정보가 필요
    HR = DEVICE->CreateInputLayout
    (
        InputDesc, 3,
        m_VSBlob->GetBufferPointer(),
        m_VSBlob->GetBufferSize(),
        m_Layout.GetAddressOf()
    );*/
    
    // Reflection을 사용한 InputLayout 작성
    HR = D3DReflect
    (
        m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize(),
        IID_ID3D11ShaderReflection,
        reinterpret_cast<void**>(m_Reflection.GetAddressOf())
    );
    if (FAILED(HR)) return E_FAIL;
    
    D3D11_SHADER_DESC shaderDesc{};
    m_Reflection->GetDesc(&shaderDesc);
    
    vector<D3D11_INPUT_ELEMENT_DESC> inputLayouts;

    for (UINT i = 0; i < shaderDesc.InputParameters; i++)
    {
        D3D11_SIGNATURE_PARAMETER_DESC paramDesc{};
        m_Reflection->GetInputParameterDesc(i, &paramDesc);

        D3D11_INPUT_ELEMENT_DESC elementDesc{};
        elementDesc.SemanticName            = paramDesc.SemanticName;
        elementDesc.SemanticIndex           = paramDesc.SemanticIndex;
        elementDesc.InputSlot               = 0;
        elementDesc.AlignedByteOffset       = D3D11_APPEND_ALIGNED_ELEMENT;
        elementDesc.InputSlotClass          = D3D11_INPUT_PER_VERTEX_DATA;
        elementDesc.InstanceDataStepRate    = 0;

        if (paramDesc.Mask < 2)
        {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)       elementDesc.Format = DXGI_FORMAT_R32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  elementDesc.Format = DXGI_FORMAT_R32_SINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
        }
        else if (paramDesc.Mask < 4)
        {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)       elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)       elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
        }
        else if (paramDesc.Mask < 8)
        {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)       elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
        else if (paramDesc.Mask < 16)
        {
            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)       elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)  elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }

        string semantic = paramDesc.SemanticName;

        if (semantic == "POSITION")
            elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

        inputLayouts.push_back(elementDesc);
    }
    
    HR = DEVICE->CreateInputLayout
    (
        inputLayouts.data(),
        inputLayouts.size(),
        m_VSBlob->GetBufferPointer(),
        m_VSBlob->GetBufferSize(),
        m_Layout.GetAddressOf()
    );
    
    if (FAILED(HR)) return E_FAIL;
    
    return S_OK;
}

HRESULT AGraphicShader::CreatePixelShader(const wstring& _RelativeFilePath, const string& _FuncName)
{
    const wstring Path = CONTENT_PATH + _RelativeFilePath;

    ComPtr<ID3DBlob> Err{}; 
    
    HRESULT HR = D3DCompileFromFile
    (
        Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
        _FuncName.c_str(), "ps_5_0", D3D10_SHADER_DEBUG,
        0, m_PSBlob.GetAddressOf(), Err.GetAddressOf()
    );
    
    if (FAILED(HR))
    {
        /*const char* pErrMsg = static_cast<const char*>(Err->GetBufferPointer());
        
        MessageBox(nullptr, pErrMsg, L"쉐이더 생성 실패", MB_OK);*/
        return E_FAIL;
    }
    
    if (FAILED(DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer(), m_PSBlob->GetBufferSize(), nullptr, m_PS.GetAddressOf())))
        return E_FAIL;
    
    return S_OK;
}

void AGraphicShader::Binding()
{
    // 정점으로 구성할 도형의 상태, 모양
    // TriangleList는 점 3개를 이어서 만든 삼각형(내부까지 색을 사용)
    CONTEXT->IASetPrimitiveTopology(m_Topology);
    
    // 정점 하나 안에 들어있는 데이터를 구분해주는 정보
    CONTEXT->IASetInputLayout(m_Layout.Get());
    
    // VS 
    CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);

    // RS
    CONTEXT->RSSetState(Device::GetInst()->GetRSState(m_RSType).Get()); // CULL_MODE : CULL_BACK, FILL_MODE : SOLID
    
    // PS
    CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

    // OM DepthStencilState 세팅
    CONTEXT->OMSetDepthStencilState(Device::GetInst()->GetDSState(m_DSType).Get(), 0);
    
    // BlendState 세팅
    ComPtr<ID3D11BlendState> pBSState = Device::GetInst()->GetBSState(m_BSType);
    CONTEXT->OMSetBlendState(pBSState.Get(), nullptr, 0xffffffff);
}
