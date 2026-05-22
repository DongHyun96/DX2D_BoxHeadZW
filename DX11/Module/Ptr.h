#pragma once

template<typename T>
class Ptr
{
private:
	T* m_Ptr{};

public:
	Ptr()
		: m_Ptr(nullptr)
	{}

	Ptr(T* _Ptr)
		: m_Ptr(_Ptr)
	{
		if (m_Ptr) m_Ptr->AddRef();
	}

	Ptr(const Ptr& _Ptr)
		: m_Ptr(_Ptr.m_Ptr)
	{
		if (m_Ptr) m_Ptr->AddRef();
	}
	
	Ptr(Ptr&& _Other) noexcept
		: m_Ptr(_Other.m_Ptr)
	{
		// 소유권 이전받음, 원본 참조 연결 끊어버림
		_Other.m_Ptr = nullptr;
	}

	~Ptr()
	{
		if (m_Ptr)
		{
			m_Ptr->Release();
			m_Ptr = nullptr;
		}
	}
	
public:

	T* Get() const { return m_Ptr; }
	T** GetAddressOf() { return &m_Ptr; }

public:

	Ptr& operator=(const Ptr& _Other)
	{
		if (m_Ptr == _Other.m_Ptr) return *this; // 포인터가 동일한 경우, 조작 x

		if (m_Ptr) m_Ptr->Release(); // 이전에 자신이 소유했던 ptr가 있었다면, 기존에 갖고 있었던 ptr Release 처리

		m_Ptr = _Other.m_Ptr;

		if (m_Ptr) m_Ptr->AddRef();

		return *this;
	}

	Ptr& operator=(T* _Ptr)
	{
		if (m_Ptr == _Ptr) return *this;

		if (m_Ptr) m_Ptr->Release(); // 이전에 자신이 소유했던 ptr가 있었다면, 기존에 갖고 있었던 ptr Release 처리

		m_Ptr = _Ptr;

		if (m_Ptr) m_Ptr->AddRef();
		return *this;
	}
	
	Ptr& operator=(Ptr&& _Other) noexcept
	{
		if (m_Ptr == _Other.m_Ptr) return *this;

		if (m_Ptr) m_Ptr->Release(); // 이전에 자신이 소유했던 ptr가 있었다면, 기존에 갖고 있었던 ptr Release 처리

		m_Ptr        = _Other.m_Ptr; // 소유권 이전
		_Other.m_Ptr = nullptr; // 원본 포인터 해제 (마찬가지로 AddRef() 안함)

		return *this;
	}

	T* operator->() const { return m_Ptr; }

	bool operator==(T* _Ptr) const { return m_Ptr == _Ptr; }
	bool operator==(const Ptr& _Ptr) const { return m_Ptr == _Ptr.m_Ptr; }

	/*bool operator!=(T* _Ptr) const { return m_Ptr != _Ptr; }
	bool operator!=(const Ptr& _Ptr) const { return m_Ptr != _Ptr.m_Ptr; }*/
	
	explicit operator bool() const { return m_Ptr != nullptr; }

};

/// <summary>
/// rawPointer == Ptr<T>형 을 비교하기 위한 전역함수
/// </summary>
template<typename T>
bool operator==(T* _Pointer, const Ptr<T>& _Ptr)
{
	return _Pointer == _Ptr.Get();
}

/// <summary>
/// rawPointer != Ptr<T>형 을 비교하기 위한 전역함수
/// </summary>
template<typename T>
bool operator!=(T* _Pointer, const Ptr<T>& _Ptr)
{
	return _Pointer != _Ptr.Get();
}
