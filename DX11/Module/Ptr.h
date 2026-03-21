#pragma once

template<typename T>
class Ptr
{
private:
	T* m_Ptr{};

public:

	T* Get() const { return m_Ptr; }
	T** GetAddressOf() { return &m_Ptr; }

public:

	Ptr& operator=(const Ptr& _Other)
	{
		if (&_Other == this) return *this; // 자기대입 처리 x

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

	T* operator->() const { return m_Ptr; }

	bool operator==(T* _Ptr) const { return m_Ptr == _Ptr; }
	bool operator==(const Ptr& _Ptr) const { return m_Ptr == _Ptr.m_Ptr; }

	bool operator!=(T* _Ptr) const { return m_Ptr != _Ptr; }
	bool operator!=(const Ptr& _Ptr) const { return m_Ptr != _Ptr.m_Ptr; }
	
	explicit operator bool() const { return m_Ptr != nullptr; }


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

	~Ptr()
	{
		if (m_Ptr) m_Ptr->Release();
	}

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
