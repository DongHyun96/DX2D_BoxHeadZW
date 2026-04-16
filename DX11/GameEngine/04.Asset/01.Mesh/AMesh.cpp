#include "pch.h"
#include "AMesh.h"

#include "GameEngine/02.Device/Device.h"

HRESULT AMesh::Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount)
{
    m_VtxCount = _VtxCount;
    m_IdxCount = _IdxCount;
    
    m_VBDesc.ByteWidth = sizeof(Vtx) * m_VtxCount; // 버퍼의 크기
    
    // CPU를 통해서 버퍼의 내용을 쓰거나, 읽을 수 있는지
    // D3D11_USAGE_DYNAMIC + D3D11_CPU_ACCESS_WRITE
    // ==> 버퍼를 생성한 이후에도, CPU를 통해서 버퍼의 내용을 수정할 수 있다.
    
    // D3D11_USAGE_DEFAULT + 0
    // 버퍼를 생성한 이후에 수정할 수 없다
    m_VBDesc.Usage            = D3D11_USAGE_DEFAULT;
    m_VBDesc.CPUAccessFlags   = 0;
    
    m_VBDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;

    // 처음 버퍼 생성할 때 전달시킬 데이터의 시작주소를 Sub구조체에 담아서 CreateBuffer 함수에 넣어준다.ㅁ
    D3D11_SUBRESOURCE_DATA tSub{};
    tSub.pSysMem = _VtxSysMem;

    if (FAILED(DEVICE->CreateBuffer(&m_VBDesc, &tSub, m_VB.GetAddressOf())))
        return E_FAIL;

    m_VtxSysMem = new Vtx[_VtxCount];
    memcpy(m_VtxSysMem, _VtxSysMem, _VtxCount*sizeof(Vtx));
    
    // Index buffer 생성
    m_IBDesc.ByteWidth = sizeof(UINT) * m_IdxCount;

    // D3D11_USAGE_DEFAULT + 0
    // 버퍼를 생성한 이후에 수정할 수 없다
    m_IBDesc.Usage            = D3D11_USAGE_DEFAULT;
    m_IBDesc.CPUAccessFlags   = 0;
    m_IBDesc.BindFlags        = D3D11_BIND_INDEX_BUFFER;

    // 처음 버퍼 생성할 때 전달시킬 데이터의 시작주소를 Sub구조체에 담아서 CreateBuffer 함수에 넣어준다.
    tSub = {};
    tSub.pSysMem = _IdxSysMem;

    if (FAILED(DEVICE->CreateBuffer(&m_IBDesc, &tSub, m_IB.GetAddressOf())))
        return E_FAIL;
    
    m_IdxSysMem = new UINT[_IdxCount];
    memcpy(m_IdxSysMem, _IdxSysMem, _IdxCount * sizeof(UINT));
    
    return S_OK;
}

void AMesh::Binding()
{
    // IA
    
    // 정점을 프로그래머가 설계하기 때문에, 전달한 버텍스 버퍼안에서, 하나의 정점 단위크기를 알려줘야 한다.
    UINT Stride = sizeof(Vtx); // 정점 데이터 간격
    UINT Offset = 0; // 기준이 되는 곳으로부터 얼만큼 떨어져 있는지 (기준 위치로부터 떨어진 위치에서부터 렌더링 처리하겠다는 의미)
    CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &Stride, &Offset);

    // 인덱스 버퍼 설정, 정점 버퍼안에 있는 정점을 가리키는 인덱스 정보, 인덱스 하나의 크기가 몇바이트인지 픽셀포멧으로 알려준다.
    // IndexBuffer 형도 내가 정해준 형(UINT) -> 32비트 포멧 사용
    CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void AMesh::Render()
{
    Binding();
    
    // Draw 이저까지는 순서가 상관 없음 (GPU 동작 세팅의 순서는 상관 없고, 마지막의 Draw call만 마지막 순서 지키면 됨)
    // 렌더링 파이프라인 실제 구동 시작 처리하는 함수
    // Draw가 호출되기 전까지 설정해놓은 세팅을 기반으로 실제 렌더링 파이프라인이 실행됨
    // 그 이전까지는 각 단계별로 실행할 옵션을 설정만 함
    // CONTEXT->Draw(6, 0);
    CONTEXT->DrawIndexed(m_IdxCount, 0, 0); // 인덱스 버퍼까지 사용한 draw call   
}

void AMesh::Render_Particle(UINT _Count)
{
    Binding();
    CONTEXT->DrawIndexedInstanced(m_IdxCount, _Count, 0, 0, 0);
}

void AMesh::RenderInstanced(UINT _InstanceCount)
{
    if (0 == _InstanceCount) return;
    
    Binding();
    CONTEXT->DrawIndexedInstanced(m_IdxCount, _InstanceCount, 0, 0, 0);
}

AMesh::AMesh()
    : Asset(ASSET_TYPE::MESH)
{
}

AMesh::~AMesh()
{
    delete[] m_VtxSysMem;
    delete[] m_IdxSysMem;
}
