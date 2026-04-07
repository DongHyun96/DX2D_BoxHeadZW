#pragma once





// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// DirectX11 객체들을 가리킬 수 있는 스마트 포인터
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX11 라이브러리
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

#include "Module/SimpleMath.h"

typedef SimpleMath::Vector2     Vec2;
typedef SimpleMath::Vector3     Vec3;
typedef SimpleMath::Vector4     Vec4;
typedef SimpleMath::Matrix      Matrix;

// DLL(동적 라이브러리)
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler") // .lib 생략가능
#pragma comment(lib, "dxguid")

// DirectTex Library
#include <DirectxTex/DirectxTex.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectxTex/DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectxTex/DirectXTex.lib")
#endif

#include <iostream>

// C++  타입 정보기능
#include <typeinfo>
#include <type_traits>

// STL 자료구조
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>
#include <stack>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <array>

#include <cfloat>
#include <cmath>

#include <crtdbg.h>
#include <random>

#include <filesystem>
#include <functional>


using namespace std;

// FMOD
#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_codec.h>

#ifdef _DEBUG
#pragma comment(lib, "FMOD/fmodL64_vc.lib")
#else
#pragma comment(lib, "FMOD/fmod64_vc.lib")
#endif


#include "resource.h"
#include "Header/global.h"
#include "Module/Ptr.h"


///////////////////////////////////////////////////User include///////////////////////////////////////////////////
#include "Source/UserHeader/userDefine.h"
#include "Source/UserHeader/userEnum.h"
#include "Source/UserHeader/userStruct.h"