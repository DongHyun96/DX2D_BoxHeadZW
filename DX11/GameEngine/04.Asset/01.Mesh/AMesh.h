#pragma once
#include "GameEngine/04.Asset/Asset.h"

/// <summary>
/// 정점, 인덱스 정보로 구성된 모델 데이터
/// </summary>
class AMesh : public Asset
{
private:

    ComPtr<ID3D11Buffer>        m_VB{}; // 정점(Vertex) 버퍼
    D3D11_BUFFER_DESC           m_VBDesc{}; // 정점 버퍼 생성 옵션
    UINT                        m_VtxCount{}; // 정점 개수
    

    ComPtr<ID3D11Buffer>        m_IB{}; // 인덱스 버퍼
    D3D11_BUFFER_DESC           m_IBDesc{}; // 인덱스 버퍼 생성 옵션
    UINT                        m_IdxCount{}; // 인덱스 개수

    
    Vtx*  m_VtxSysMem{}; // 정점 데이터 시스템 메모리 저장
    UINT* m_IdxSysMem{}; // 정점 데이터 시스템 메모리 저장

    
public:
    
    /// <summary>
    /// Mesh 생성
    /// </summary>
    /// <param name="_VtxSysMem">   : 정점 데이터 array </param>
    /// <param name="_VtxCount">    : 정점 개수(정점 데이터 array size) </param>
    /// <param name="_IdxSysMem">   : 인덱스 데이터 array </param>
    /// <param name="_IdxCount">    : 인덱스 개수(인덱스 데이터 array) </param>
    /// <returns> 생성 성공 시 return true </returns>
    HRESULT Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount);
    
    const Vtx* GetVtxSysMem() { return m_VtxSysMem; }
    const UINT* GetIdxSysMem() { return m_IdxSysMem; }

private:
    
    /// <summary>
    /// GPU IA에 연결 binding 처리
    /// </summary>
    void Binding();

public:
    
    /// <summary>
    /// Binding 및 DrawCall 처리 
    /// </summary>
    void Render();

public:
    
    AMesh();
    virtual ~AMesh() override;
    
};

