#pragma once

class Entity
{

	template<typename T>
	friend class Ptr;

private:

	static UINT g_NextID;
	const UINT	m_InstID; // 객체마다 가지는 고유 ID 값

	wstring		m_Name{}; // 객체마다 이름을 지정할 수 있다.
	
	int			m_RefCount; // 참조 카운팅

private:

	void AddRef() { ++m_RefCount; }
	void Release() { if (--m_RefCount <= 0) delete this; }
		

public:
	
	UINT GetID() const { return m_InstID; }
	
	void SetName(const wstring& name){ this->m_Name = name; }
	const wstring& GetName() const { return m_Name; }
	
public:
	
	Entity();
	
	/// <summary>
	/// <para> ID가 겹치지 않게 하기위해서, 직접 복사생성자 구현 </para>
	/// <para> 대입연산자는 const 멤버변수가 있는 경우, 컴파일러가 자동생성하지 않음 </para>
	/// </summary>
	Entity(const Entity& _Other);
	// Entity(const Entity& _Other) = delete; // 컴파일러가 해당 생성자를 자동생성 못하도록 아예 막고, 구현도 안할 경우 delete 키워드로 막을 수 있다
	
	virtual ~Entity();

};
