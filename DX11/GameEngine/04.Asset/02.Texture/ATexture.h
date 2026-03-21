#pragma once
#include "GameEngine/04.Asset/Asset.h"

class ATexture : public Asset
{
	
	friend class TileMapUI;
	
private:
	
	uint64_t m_FileHash{}; // 파일 해시값
	
private:

	ScratchImage			m_Image{}; // Content 폴더에 있는 이미지 파일을 메모리(SysMem)로 불러들임
	ComPtr<ID3D11Texture2D> m_Tex2D{}; // SysMem로 로딩한 픽셀 데이터를 GPU 메모리로 전송
	D3D11_TEXTURE2D_DESC	m_Desc{};
	
	// 텍스쳐의 용도에 맞는 View들
	// Texture2D를 직접 GPU 렌더링 파이프라인으로 보내는 것이 아닌, View를 통해 보낸다
	ComPtr<ID3D11RenderTargetView> 		m_RTV{};
	ComPtr<ID3D11DepthStencilView> 		m_DSV{};
	ComPtr<ID3D11ShaderResourceView>	m_SRV{};

	int									m_RecentRegisterNum = -1; // 가장 최근에 몇 번 register에 binding되었는지 저장(-1인 경우, binding된 적 없음)
	
public:
	
	ATexture();
	virtual ~ATexture() override;

public:
	
	void Binding(UINT _RegisterNum);
	
	// Binding 처리한 Texture Clear
	void Clear();
	
public:
	
	virtual HRESULT Save(const wstring& _strFilePath) override;
	
	/// <summary>
	/// 이미지 파일 로드하기
	/// </summary>
	/// <param name="_strFilePath"></param>
	/// <returns></returns>
	virtual HRESULT Load(const wstring& _strFilePath) override;
	
	float GetWidth() const { return m_Desc.Width; }
	float GetHeight() const { return m_Desc.Height; }
	
	ComPtr<ID3D11RenderTargetView> GetRTV()		const { return m_RTV; }
	ComPtr<ID3D11ShaderResourceView> GetSRV()	const { return m_SRV; }
	ComPtr<ID3D11DepthStencilView> GetDSV()		const { return m_DSV; }
	
private:
	
	const ScratchImage& GetImage() const { return m_Image; }
	
};

