#include "pch.h"
#include "ConstBuffer.h"

#include "GameEngine/02.Device/Device.h"

ConstBuffer::ConstBuffer()
	: m_Desc{}
	, m_Type(CB_TYPE::END)
{
}

ConstBuffer::~ConstBuffer()
{
}

HRESULT ConstBuffer::Create(CB_TYPE _Type, UINT _Size)
{
    m_Type = _Type;

    /* 상수 버퍼 생성 */

    m_Desc.ByteWidth        = _Size;
    m_Desc.Usage            = D3D11_USAGE_DYNAMIC;
    m_Desc.CPUAccessFlags   = D3D11_CPU_ACCESS_WRITE;

    // 버퍼 용도
    m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    /*D3D11_SUBRESOURCE_DATA tSub{};
    tSub.pSysMem = &constData;*/
    
    // 처음 버퍼 생성할때 전달시킬 데이터의 시작주소를 Sub 구조체에 담아서 CreateBuffer 함수에 넣어준다.
    if (FAILED(DEVICE->CreateBuffer(&m_Desc, nullptr, m_CB.GetAddressOf())))
        return E_FAIL;

	return S_OK;
}

void ConstBuffer::SetData(void* _SysMem, UINT _Size)
{
    if (_Size == 0) _Size = m_Desc.ByteWidth;

    // 전역변수에 들어있는 ObjectPos 정보를 ConstantBuffer에 복사
    D3D11_MAPPED_SUBRESOURCE  tMapSub{};
    CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMapSub);
    memcpy(tMapSub.pData, _SysMem, _Size);
    CONTEXT->Unmap(m_CB.Get(), 0);
}

void ConstBuffer::Binding()
{
    // ConstantBuffer 바인딩
    CONTEXT->VSSetConstantBuffers(static_cast<UINT>(m_Type)/*상수버퍼를 바인딩할 레지스터 번호*/, 1, m_CB.GetAddressOf());
    CONTEXT->HSSetConstantBuffers(static_cast<UINT>(m_Type)/*상수버퍼를 바인딩할 레지스터 번호*/, 1, m_CB.GetAddressOf());
    CONTEXT->DSSetConstantBuffers(static_cast<UINT>(m_Type)/*상수버퍼를 바인딩할 레지스터 번호*/, 1, m_CB.GetAddressOf());
    CONTEXT->GSSetConstantBuffers(static_cast<UINT>(m_Type)/*상수버퍼를 바인딩할 레지스터 번호*/, 1, m_CB.GetAddressOf());
    CONTEXT->PSSetConstantBuffers(static_cast<UINT>(m_Type)/*상수버퍼를 바인딩할 레지스터 번호*/, 1, m_CB.GetAddressOf());
    
}

