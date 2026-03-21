#pragma once
#include "GameEngine/04.Asset/Entity.h"

enum SB_TYPE
{
    SRV_ONLY,
    SRV_UAV
};

class StructuredBuffer : public Entity
{
private:
    
    ComPtr<ID3D11Buffer>                m_SB{};
    ComPtr<ID3D11Buffer>                m_SB_Write{};
    ComPtr<ID3D11Buffer>                m_SB_Read{};
    
    ComPtr<ID3D11ShaderResourceView>    m_SRV{};
    ComPtr<ID3D11UnorderedAccessView>   m_UAV{};
    
    D3D11_BUFFER_DESC                   m_Desc{};
    
    UINT                                m_ElementSize{};
    UINT                                m_ElementCount{};
    SB_TYPE                             m_SBType{};
    
    int                                 m_RecentRegisterNum = -1;
    int                                 m_RecentSRVNum = -1;
    int                                 m_RecentUAVNum = -1;
    bool                                m_SysMemMove{};
    

public:
    
    StructuredBuffer();
    StructuredBuffer(const StructuredBuffer& _Origin);
    virtual ~StructuredBuffer() override;
    
public:
    
    HRESULT Create
    (
        UINT    _ElementSize,
        UINT    _ElementCount,
        SB_TYPE _Type,
        bool    _bSysMove,
        void*   _SysMem = nullptr
    );

    void SetData(void* _SysMem, UINT _DataSize = 0);
    void GetData(void* _DstSysMem, UINT _DataSize = 0);
    
    void Binding(UINT _RegisterNum);
    void Clear();
    
    void Binding_CS_SRV(UINT _RegisterNum);
    void Clear_CS_SRV();
    
    void Binding_CS_UAV(UINT _RegisterNum);
    void Clear_CS_UAV();
    
    GET(UINT, ElementCount)
    GET(UINT, ElementSize)
    UINT GetBufferSize() const { return m_ElementCount * m_ElementSize; }
    
    GET(ComPtr<ID3D11ShaderResourceView>, SRV)
    GET(ComPtr<ID3D11UnorderedAccessView>, UAV)
    

    
};
