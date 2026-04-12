#pragma once

struct CellCoord
{
    int x{};
    int y{};
    
    CellCoord() {}
    CellCoord(UINT x, UINT y) : x(x), y(y) {}
    CellCoord(int x, int y) :x(x), y(y) {}
    
    bool operator==(const CellCoord& other) const
    {
        return (this->x == other.x && this->y == other.y);
    }

    bool operator!=(const CellCoord& other) const
    {
        return (this->x != other.x || this->y != other.y);
    }
    
    CellCoord operator+(const CellCoord& other) const
    {
        return CellCoord(this->x + other.x, this->y + other.y);
    }
    
    CellCoord operator-(const CellCoord& other) const
    {
        return CellCoord(this->x - other.x, this->y - other.y);
    }

    bool operator<(const CellCoord& other) const
    {
        if (this->x != other.x) return this->x < other.x;
        return this->y < other.y;
    }

    // 인접한 8칸 중 랜덤한 지점 구하기
    CellCoord GetRandomAdjacentCellCoord() const
    {
        static vector<CellCoord> Directions = 
        {
            {-1, -1}, {0, -1}, {1, -1},
            {-1,  0},          {1,  0},
            {-1,  1}, {0,  1}, {1,  1}
        };
        
        const CellCoord& Picked = Directions.at(GetRandom(0, static_cast<int>(Directions.size() - 1)));
        return *this + Picked;        
    }
};

struct ExplosionSpawnDesc
{
    Vec3        SpawnPos{};
    float       ExplosionSizeFactor              = 1.f;
    float       FPS                              = 50.f;
    // float       FPS                              = 800.f;
    float       DamageAmount                     = 75.f;
    CScript*    SpawnedBy                        = nullptr;
    bool        UseCollisionForDamaging          = true;
    bool        PlayExplosionSound               = true;
    Vec2        UpwardVelocity                   = { 0.f, 0.75f };
    float       DamagePulseDelaySec              = 0.06f;
    float       DamagePulseDurationSec           = 0.04f;
    int         DamagePulseSpriteIdx             = 2;
    int         SecondaryBurstCount              = 0;
    float       SecondaryBurstRadius             = 90.f;
    float       SecondaryBurstMinDelaySec        = 0.08f;
    float       SecondaryBurstMaxDelaySec        = 0.20f;
    float       SecondaryBurstDamageScale        = 0.6f;
    float       SecondaryBurstSizeScale          = 0.55f;
    bool        SecondaryBurstPlaySound          = false;
};

struct FirePillarSpawnDesc
{
    Vec3                SpawnPos{};
    int                 StepCount                        = 6;
    float               StepIntervalSec                  = 0.03f;
    float               StepHeight                       = 26.f;
    float               BaseExplosionSizeScale           = 1.2f;
    float               TopExplosionSizeScale            = 0.55f;
    float               HorizontalJitter                 = 6.f;
    float               VerticalJitter                   = 4.f;
    bool                PlaySoundEachStep                = false;
    ExplosionSpawnDesc  ExplosionTemplate{};
};

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
    
    // 일반 iterator (데이터 수정 가능)
    using iterator = vector<T>::iterator;
    iterator begin() { return values.begin(); }
    iterator end() { return values.end(); }

    // const iterator (const 객체에서 호출 시 사용됨, 데이터 수정 불가)
    using const_iterator = vector<T>::const_iterator;
    const_iterator begin() const { return values.begin(); }
    const_iterator end() const { return values.end(); }
};

