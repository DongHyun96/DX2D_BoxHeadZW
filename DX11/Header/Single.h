#pragma once
#include <cstdlib>

template<typename T>
class Singleton
{
private:
    static T* m_This;
    
public:
    static T* GetInst()
    {
        if (!m_This) m_This = new T;
        return m_This;
    }
    
    static void Destroy()
    {
        if (m_This)
        {
            delete m_This;
            m_This = nullptr;
        }
    }

protected:
    
    /// <summary> 생성자는 자식에서만 호출 가능하게 처리 </summary> 
    Singleton()
    {
        // 프로그램 종료시, 등록시킨 함수를 호출시켜달라는 요청
        // 반환타입이 void이고, 인자도 아무것도 안받는 그런 함수의 주소만 받을 수 있다 +
        // (함수 호출규약이 __cdecl 인 경우만 가능)
        atexit(Destroy);
    }
    
private:
    
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
    
};

template <typename T>
T* Singleton<T>::m_This{};

