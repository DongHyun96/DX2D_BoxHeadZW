#pragma once

#include "GameEngine/04.Asset/Entity.h"

class ConstBuffer : public Entity
{
public:
	ConstBuffer();
	virtual ~ConstBuffer() override;

private:

	ComPtr<ID3D11Buffer>	m_CB{};
	D3D11_BUFFER_DESC		m_Desc{};
	CB_TYPE					m_Type{};

public:

	/// <summary>
	/// 상수 버퍼 생성
	/// </summary>
	/// <param name="_Type"></param>
	/// <param name="_Size"></param>
	/// <returns></returns>
	HRESULT Create(CB_TYPE _Type, UINT _Size);

	
	void SetData(void* _SysMem, UINT _Size = 0);
	void Binding();

};
