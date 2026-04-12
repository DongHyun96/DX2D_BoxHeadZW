#pragma once

// 기본생성자, 소멸자 -> private field 처리 (살아는 있어야 함)
// 이동생성자, 복사생성자, 복사대입연산자, 이동대입연산자 -> delete 처리
#define SINGLE(Type) private:\
					 Type();\
					 Type(const Type&) = delete;\
					 Type(Type&&) = delete;\
					 Type& operator=(const Type&) = delete;\
					 Type& operator=(Type&&) = delete;\
					 ~Type();\
					 friend class Singleton<Type>


#define MAX_LAYER 32

template <typename T>
struct SetterParam { using ParamType = const T&; };

// 포인터 타입 특수화
template <typename T>
struct SetterParam<T*> { using ParamType = T*; };

#define GET(Type, MemName) Type Get##MemName() const { return m_##MemName; }
#define SET(ValueType, MemName) void Set##MemName(typename SetterParam<ValueType>::ParamType _Data) { m_##MemName = _Data; }

#define GET_SET(ValueType, MemName) \
GET(ValueType, MemName) \
SET(ValueType, MemName)

#define CLONE(Type) Type* Clone() const { return new Type(*this); }

/*#define RESOL_X 1600.f
#define RESOL_Y 900.f
#define RESOL_HALF_X 800.f
#define RESOL_HALF_Y 450.f*/

#define RESOL_X 1920.f
#define RESOL_Y 1080.f
#define RESOL_HALF_X 960.f
#define RESOL_HALF_Y 540.f

/*#define RESOL_X 2560.f
#define RESOL_Y 1440.f
#define RESOL_HALF_X 1280.f
#define RESOL_HALF_Y 720.f*/

#define RESOL_DIAG_LENGTH RenderMgr::GetInst()->GetScreenResolDiagLength()
#define RESOL_DIAG_HALF_LENGTH RenderMgr::GetInst()->GetScreenResolHalfDiagLength()


#define TEXTURE_LOADING_FLAG WIC_FLAGS_NONE
// #define TEXTURE_LOADING_FLAG WIC_FLAGS_IGNORE_SRGB
