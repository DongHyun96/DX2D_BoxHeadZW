#pragma once


enum class LIGHT_TYPE;
enum class TASK_TYPE;
enum class DEBUG_SHAPE;

struct Vertex
{
    Vec3 vPos{}; // 3D 공간상에서 정점의 위치
    Vec2 vUV{}; // 정점위에 이미지를 띄울 경우, 이미지의 어느지점을 참고하는지 좌표
    Vec4 vColor{}; // 정점 자체의 색상
};

typedef Vertex Vtx;

/// <summary>
/// DebugInfo : 디버그 렌더링 요청사항 정보
/// </summary>
struct DebugInfo
{
    DEBUG_SHAPE   Shape{};
    
    Vec3        Pos{}; // 셋 다 world 기준
    Vec3        Scale{};
    Vec3        Rotation{};
    Matrix      matWorld{};
    
    Vec4        Color{};
    
    float       Age{};  // 현재수명 (초 단위)
    float       Life{}; // 총 최대수명 (초 단위)
    
    bool        EnableDepthTest{};
    bool        UseUICamera{};
};


/// <summary>
/// ImGui 용 Log Data
/// </summary>
struct DebugLogUIData
{
    
    friend class DebugLogUI;
    
    string  LogStr{};
    Vec4    Color{};
    float   TotalLifeTime{};
    
private:
    float   Age{};
};

/// <summary>
/// TaskMgr가 처리할 작업 단위
/// </summary>
struct TaskInfo
{
    TASK_TYPE Type{};
    DWORD_PTR Param_0; // 추가 부수적인 Parameter들 (DWORD_PTR 8바이트 정수), 단순한 값을 저장 or 주솟값을 저장할 때 범용적으로 사용
    DWORD_PTR Param_1;
    DWORD_PTR Param_2;
};


// Light2D 정보
struct Light2DInfo
{
    LIGHT_TYPE  Type{};
    Vec3        Color{};
    Vec3        Ambient{};  // 환경광, 광원이 존재하면서 최소한으로 발생하는 빛의 세기
    Vec3        LightDir{}; // 광원의 빛이 향하는 방향
    Vec3        WorldPos{}; // 광원의 위치 (Point, Spot)
    float       Radius{};   // 빛의 영향 반경 (Point, Spot)
    float       Angle{};    // SpotLight 범위 각
};


struct TransformMatrix
{
    Matrix matWorld{};
    Matrix matView{};
    Matrix matProj{};
};

extern TransformMatrix g_Trans;

// 재질 const data
struct MtrlConst
{
    int     iArr[4]{};
    float   fArr[4]{};
    Vec2    v2Arr[4]{};
    Vec4    v4Arr[4]{};
    Matrix  mat[2]{};
    
    int     IsTex[TEX_END]{}; // 각 texture slot에 제대로 texture가 binding되었는지 bool 값으로 사용 
    int     DummyPadding[2];
};

struct GlobalData
{
    Vec2    Resolution{};       // 화면 해상도
    int     Light2DCount{};     // 2D 광원 개수
    int     Light3DCount{};     // 3D 광원 개수
    float   DeltaTime{};        // DeltaTime
    float   Time{};             // 게임 총 흐른 누적시간값
    float   EngineDT{};         // DeltaTime
    float   EngineTime{};       // 누적시간값
};
extern GlobalData g_Global;

struct GUIDHasher
{
    size_t operator()(const GUID& guid) const
    {
        // GUID는 총 16바이트
        // 가장 빠르고 효율적인 방법은 8바이트(uint64_t) 2개로 쪼개서 해시를 결합
        const uint64_t* p = reinterpret_cast<const uint64_t*>(&guid);
        
        size_t hash1 = hash<uint64_t>{}(p[0]);
        size_t hash2 = hash<uint64_t>{}(p[1]);
        
        // 두 해시값을 섞어주기 (Boost 라이브러리의 hash_combine 알고리즘과 동일)
        return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
    }
};

// 두 GUID가 같은지 비교하는 Equal 함수 객체
struct GUIDEqual
{
    bool operator()(const GUID& lhs, const GUID& rhs) const
    {
        return IsEqualGUID(lhs, rhs);
    }
};
