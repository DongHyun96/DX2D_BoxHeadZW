#pragma once
#include "Entity.h"

class Asset : public Entity
{
	
	friend class AssetMgr;
	friend class ContentUI;
	
private:
	
	bool m_IsProvidedByEngine{}; // 엔진 자체에서 직접 제작된 Asset인지 구분 (Mesh(100% 엔진 제작)와 Mtrl(일부 엔진 제공 Mtrl) Type
	
private:

	GUID	m_Guid{};		// IsEqualGUID로 GUID 값 비교, AssetLoad 실패 시(이 때는 Path 말고 Asset 파일의 이름까지 바뀌었을 때), 최후의 최후로 확인할 요소가 된다
	
	wstring m_Key{};			// Asset을 찾을 때 사용할 key값 이름 (L"AssetTypeFolder\\Asset파일 이름.Asset확장자명" 포맷 사용)
	wstring m_RelativePath{};	// 상대 경로(실제 Content 폴더에 저장되어있는 위치)

	// RTTI : Runtime Type Identification, 런타임 도중에, 객체의 실제 타입을 알아내는 기술
	const ASSET_TYPE m_Type; // 에셋 타입
	
public:
	// 기본생성자 제거, 자신이 어떤 타입의 Asset인지 입력으로 받는 버전의 생성자만 남겨둠
	Asset(ASSET_TYPE _Type);

	/// <summary>
	/// 이 Asset Reference를 가지고 있는 게임 오브젝트 또는 컴포넌트가 있을 때, 레벨 Play 시 복사처리할 때 호출된다
	/// </summary>
	Asset(const Asset& _Origin);
	
	virtual ~Asset() override;
	
public:

	ASSET_TYPE GetType() const { return m_Type; }
	
public:
	const wstring& GetKey() const { return m_Key; }
	
	GUID GetGuid();
	
	const wstring& GetRelativePath() const { return m_RelativePath; }
	
	void SetIsProvidedByEngine(bool _IsProvidedByEngine) { m_IsProvidedByEngine = _IsProvidedByEngine; }
	bool GetIsProvidedByEngine() const { return m_IsProvidedByEngine; }
	
private:
	
	void SetKey(const wstring& _Key);
	void SetRelativePath(const wstring& path) { m_RelativePath = path; } 

protected:

	void SetGuid(const GUID& guid) { m_Guid = guid; }
	GUID& GetGuidRef();
	
public:

	/// <summary>
	/// Asset 내용 불러오기 
	/// </summary>
	/// <param name="_FilePath"> : 파일 절대경로 </param>
	virtual HRESULT Load(const wstring& _FilePath);

	/// <summary>
	/// Asset 내용 저장하기
	/// </summary>
	/// <param name="_FilePath"> : 파일 절대경로 </param>
	virtual HRESULT Save(const wstring& _FilePath);

	/// <summary>
	/// 자기자신의 RelativePath에 접근하여, 해당 파일에 저장
	/// </summary>
	/// <returns> : 제대로 저장되었다면 return S_OK </returns>
	HRESULT SaveBySelfRelativePath();
	
};
