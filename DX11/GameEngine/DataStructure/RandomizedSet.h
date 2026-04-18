#pragma once

template <typename T>
class RandomizedSet 
{
private:
    vector<T> values;
    unordered_map<T, size_t> valToIndex; // 인덱스는 size_t를 사용하는 것이 안전
    mutable mt19937 gen;

public:
    // 생성자: 난수 생성기 초기화
    RandomizedSet() : gen(random_device{}()) {}

    // 값 추가 (O(1))
    bool insert(const T& val) 
    {
        if (valToIndex.find(val) != valToIndex.end()) 
        {
            return false; // 이미 존재하는 값이면 무시
        }
        
        values.push_back(val);
        valToIndex[val] = values.size() - 1;
        return true;
    }

    // 값 삭제 (O(1))
    bool remove(const T& val) 
    {
        auto it = valToIndex.find(val);
        if (it == valToIndex.end()) 
        {
            return false; // 없는 값이면 무시
        }
        
        // 지울 값의 인덱스와, 벡터 맨 끝의 값을 확인
        size_t indexToRemove = it->second;
        const T& lastVal = values.back(); 
        
        // 맨 끝 값을 지울 위치로 덮어씌움 (Swap)
        values[indexToRemove] = lastVal;
        
        // Map에서 덮어씌워진 맨 끝 값의 인덱스 정보를 갱신
        valToIndex[lastVal] = indexToRemove;
        
        // 원래 지우려던 값 정리 (Pop)
        values.pop_back();
        valToIndex.erase(it); // it로 처리하는게 val보다 빠름
        
        return true;
    }
    
    const T* data() const
    {
        return values.data();
    }
    
    T* data()
    {
        return values.data();
    }
    
    // 랜덤 값 뽑기 (O(1))
    const T& getRandom() const 
    {
        if (values.empty())
        {
            throw out_of_range("Currently empty set");
        }
        uniform_int_distribution<size_t> dis(0, values.size() - 1);
        return values[dis(gen)];
    }

    size_t size() const 
    {
        return values.size();
    }
    
    bool empty() const 
    {
        return values.empty();
    }
    
    void clear() 
    {
        values.clear();
        valToIndex.clear();
    }

    T* find(const T& val)
    {
        auto it = valToIndex.find(val);
        if (it == valToIndex.end()) return nullptr;
        return &values[it->second];
    }

    const T* find(const T& val) const
    {
        auto it = valToIndex.find(val);
        if (it == valToIndex.end()) return nullptr;
        return &values[it->second];
    }
    
    // 일반 iterator (데이터 수정 가능)
    using iterator = vector<T>::iterator;
    iterator begin() { return values.begin(); }
    iterator end() { return values.end(); }

    // const iterator (const 객체에서 호출 시 사용됨, 데이터 수정 불가)
    using const_iterator = vector<T>::const_iterator;
    const_iterator begin() const { return values.begin(); }
    const_iterator end() const { return values.end(); }
};
