#pragma once
#include "GameEngine/04.Asset/Asset.h"

/// <summary>
/// Shader가 요구하는 인자의 종류
/// </summary>
enum class SHADER_PARAM
{
	INT,
	FLOAT,
	VEC2,
	VEC4,
	MAT,
	TEX,
};

/// <summary>
/// 특정 Shader에서 쓰일 
/// </summary>
struct ShaderParam
{
	SHADER_PARAM	Type{};
	int				Index{};
	wstring			Desc{}; 	// 이 Parameter의 사용 용도
	int				Step{}; 	// ImGui에서 값의 변화량
	bool			IsInput{};	// ImGui 에서 Input or Drag (UI 옵션)
};


/// <summary>
/// 에셋 - 공유자원
/// 렌더링 파이프라인 자체를 하나의 에셋으로 본다
/// </summary>
class AGraphicShader : public Asset
{
private:
	
	ComPtr<ID3DBlob>            	m_VSBlob{}; 										// HLSL로 작성한 VS 함수를 컴파일한 어셈블리코드를 저장시킬 버퍼 
	ComPtr<ID3DBlob>				m_GSBlob{};											// HLSL로 작성한 GS 함수를 컴파일한 어셈블리코드를 저장시킬 버퍼
	ComPtr<ID3DBlob>            	m_PSBlob{}; 										// HLSL로 작성한 PS 함수를 컴파일한 어셈블리코드를 저장시킬 버퍼
	                            	
	ComPtr<ID3D11VertexShader>  	m_VS{};
	ComPtr<ID3D11GeometryShader>  	m_GS{};
	ComPtr<ID3D11PixelShader>   	m_PS{};
									
	RS_TYPE							m_RSType{}; 										// 레스터라이저 컬링모드 
	DS_TYPE							m_DSType{}; 										// DepthStencilState 설정
	BS_TYPE							m_BSType{}; 										// 블랜딩 공식 설정
	                            	
	                            	
	ComPtr<ID3D11InputLayout>   	m_Layout{};											// Input Layout - 하나의 정점이 어떻게 구성되어있는지에 대한 정보
	ComPtr<ID3D11ShaderReflection>	m_Reflection{};

	
	D3D11_PRIMITIVE_TOPOLOGY		m_Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // 렌더링 과정에서, 정점들을 어떤 도형으로 인지할 것인지 

	vector<ShaderParam>				m_vecShaderParam{};
	
public:
	
	void AddShaderParam(SHADER_PARAM _Type, int _Idx, const wstring& _Desc, int _Step = 0, bool _IsInput = true)
	{
		m_vecShaderParam.push_back(ShaderParam(_Type, _Idx,_Desc, _Step, _IsInput));
	}
	
	const vector<ShaderParam>& GetShaderParams() const { return m_vecShaderParam; } 
	
public:
	
	AGraphicShader();
	virtual ~AGraphicShader() override;

public:
	
	/// <summary>
	/// VertexShader 생성 
	/// </summary>
	/// <param name="_RelativeFilePath"> : VS shader code 파일 상대 경로 </param>
	/// <param name="_FuncName"> : VS function name </param>
	/// <returns> : 제대로 생성되었다면 return HRESULT S_OK / 실패 시 E_FAIL </returns>
	HRESULT CreateVertexShader(const wstring& _RelativeFilePath, const string& _FuncName);

	HRESULT CreateGeometryShader(const wstring& _RelativeFilePath, const string& _FuncName);
	
	/// <summary>
	/// PixelShader 생성
	/// </summary>
	/// <param name="_RelativeFilePath"> : PS shader code 파일 상대 경로 </param>
	/// <param name="_FuncName"> : PS function name </param>
	/// <returns> : 제대로 생성되었다면 return HRESULT S_OK / 실패 시 E_FAIL </returns>
	HRESULT CreatePixelShader(const wstring& _RelativeFilePath, const string& _FuncName);

	
	GET_SET(D3D11_PRIMITIVE_TOPOLOGY, Topology)

	GET_SET(BS_TYPE, BSType);
	GET_SET(RS_TYPE, RSType);
	GET_SET(DS_TYPE, DSType);

	/// <summary> Draw call 이전, 세팅된 렌더링 파이프라인 바인딩 처리 </summary>
	void Binding();

};
