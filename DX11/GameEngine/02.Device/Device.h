#pragma once

#include "ConstBuffer/ConstBuffer.h"
#include "GameEngine/04.Asset/03.GraphicShader/AGraphicShader.h"
#include "Module/Ptr.h"

/// <summary>
/// DirectX11 라이브러리 함수를 이용해서
/// GPU를 제어할 수 있는 클래스
/// </summary>
class Device : public Singleton<Device>
{
    SINGLE(Device);
    
private:
    
    ComPtr<ID3D11Device>              m_Device{};               // GPU 메모리 관리
    ComPtr<ID3D11DeviceContext>       m_Context{};              // GPU 제어 및 렌더링 명령, GPGPU(General purpose GPU) 명령 - ComputeShader
    
    ComPtr<IDXGISwapChain>            m_SwapChain{};            
    ComPtr<ID3D11Texture2D>           m_RenderTarget{};  
    ComPtr<ID3D11Texture2D>           m_DepthStencilTarget{};
    
    ComPtr<ID3D11RenderTargetView>    m_RTV{};
    ComPtr<ID3D11DepthStencilView>    m_DSV{};
    
    ComPtr<ID3D11SamplerState>        m_arrSampler[2]{};
    ComPtr<ID3D11BlendState>          m_BSState[static_cast<UINT>(BS_TYPE::END)]{};
    ComPtr<ID3D11RasterizerState>     m_RSState[static_cast<UINT>(RS_TYPE::END)]{};
    ComPtr<ID3D11DepthStencilState>   m_DSState[static_cast<UINT>(DS_TYPE::END)]{};

private:
    
    HWND m_hWnd{};          // 출력 윈도우
    Vec2 m_RenderResol{};  // 렌더타겟 해상도
    
private:
    
    Ptr<ConstBuffer> m_CB[static_cast<UINT>(CB_TYPE::END)]{};
    

public:
    
    HRESULT Init(HWND _hwnd, Vec2 _Resolution);

    /// <summary> SwapChain present 처리 </summary>
    void Present() const { m_SwapChain->Present(0, 0); }
    
public:

    Vec2 GetRenderResolution() const { return m_RenderResol; }
    
    float GetRenderResX() const { return m_RenderResol.x; }
    float GetRenderResY() const { return m_RenderResol.y; }
    
    ComPtr<ID3D11Device> GetDevice() const { return m_Device; }
    ComPtr<ID3D11DeviceContext> GetContext() const { return m_Context; }

    ComPtr<ID3D11RasterizerState> GetRSState(RS_TYPE _Type) const { return m_RSState[static_cast<UINT>(_Type)]; }
    ComPtr<ID3D11BlendState> GetBSState(BS_TYPE _Type) const { return m_BSState[static_cast<UINT>(_Type)]; } 
    ComPtr<ID3D11DepthStencilState> GetDSState(DS_TYPE _Type) const { return m_DSState[static_cast<UINT>(_Type)]; }
    
    Ptr<ConstBuffer> GetCB(CB_TYPE _Type) const { return m_CB[static_cast<UINT>(_Type)]; }
    
    HWND GetMainWndHwnd() const { return m_hWnd; }
    
public:

    /// <summary>
    /// <para> RTV의 경우 Clear color로 초기화 시키고 DSV는 최대 깊이인 1로 초기화 </para>
    /// <para> Render() 과정의 가장 첫 단계에서 처리 </para>
    /// </summary>
    void ClearTarget();
    
    void OMSetTarget();

private:
    HRESULT CreateSwapChain();
    HRESULT CreateBuffer();
    HRESULT CreateBlendState();
    HRESULT CreateSampler();
    HRESULT CreateRasterizerState();    
    HRESULT CreateDepthStencilState();    
    
    
    HRESULT CreateConstBuffer();    
    
};

#define DEVICE  Device::GetInst()->GetDevice().Get()
#define CONTEXT Device::GetInst()->GetContext().Get()
