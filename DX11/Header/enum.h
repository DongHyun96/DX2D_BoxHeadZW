#pragma once

/// <summary> ConstantBuffer Type </summary>
enum class CB_TYPE
{
    TRANSFORM,  // b0 W, V, P 행렬 넘기는 용
    MATERIAL,   // b1
    GLOBAL,     // b2
    END,
};


/// <summary> BlendState Type </summary>
enum class BS_TYPE
{
    DEFAULT,        // SrcRGB * 1 + DestRGB * 0
    ALPHA_BLEND,    // (SrcRGB * SrcA) + (DestRGB * (1 - SrcA)) 
    // ONE_ONE,        // (SrcRGB * 1) + (DestRGB * 1)
    ADDITIVE,        
    END
};

/// <summary> RasterizerState Type </summary>
enum class RS_TYPE
{
    CULL_BACK,  // 뒷면(정점 반시계방향) culling 처리, default setting
    CULL_FRONT, // 앞면 culling 처리
    CULL_NONE,  // Culling x
    WIRE_FRAME, // 정점과 정점 사이를 잇는 부분을 제외
    END
};

/// <summary> DepthState Type </summary>
enum class DS_TYPE
{
    LESS,               // Default setting, 깊이판정 LESS(가까운게 판정 성공), 판정 성공 시 깊이 재기록
    LESS_EQUAL,         // 깊이판정 작거나 같은 경우까지 허용
    LESS_WRITE,      
    NO_TEST,            // 깊이판정 x, 무조건 성공처리, 깊이쓰기 o
    LESS_NO_WRITE,      // 깊이판정은 하되, 깊이 쓰기 x (반투명용)
    NO_TEST_NO_WRITE,   // 깊이판정 x, 무조건 성공처리, 깊이쓰기 x,
    END,
};

enum class ASSET_TYPE
{
    MESH,
    MATERIAL,
    TEXTURE,
    SPRITE,
    FLIPBOOK,
    TILEMAP,
    GRAPHICS_SHADER,
    COMPUTE_SHADER,
    LEVEL,
    SOUND,
    PREFAB,
    END,
};

enum class COMPONENT_TYPE
{
    // Engine 제공 Component Type들
    TRANSFORM,              
    CAMERA,                 
    
    COLLIDER2D_RECT,        
    COLLIDER2D_CIRCLE,
    COLLIDER2D_POINT,
    
    LIGHT2D,
    LIGHT3D,
    
    MESH_RENDER,
    BILLBOARD_RENDER,
    FLIPBOOK_RENDER,
    PARTICLE_RENDER,
    SPRITE_RENDER,
    TILE_RENDER,
    
    POOL,   // Object Pooling Component

    END,
    
    SCRIPT, // 사용자 지정 component type (GameObject당, 가질 수 있는 ScriptComponent는 여러개가 될 수 있다)
};

enum class DIR
{
    RIGHT,
    UP,
    FRONT,
    
    END
};

/// <summary>
/// AMaterial의 Texture 6칸 Param Type
/// </summary>
enum TEX_PARAM
{
    TEX_0,
    TEX_1,
    TEX_2,
    TEX_3,
    TEX_4,
    TEX_5,
    
    TEX_END
};


/// <summary>
/// 재질 Constant Data Param Type 
/// </summary>
enum SCALAR_PARAM
{
    INT_0,
    INT_1,
    INT_2,
    INT_3,
    
    FLOAT_0,
    FLOAT_1,
    FLOAT_2,
    FLOAT_3,
    
    VEC2_0,
    VEC2_1,
    VEC2_2,
    VEC2_3,
    
    VEC4_0,
    VEC4_1,
    VEC4_2,
    VEC4_3,
    
    MAT_0,
    MAT_1,
};

enum class DEBUG_SHAPE
{
    RECT,
    CIRCLE,
    LINE,
    CUBE,
    SPHERE,
};

/// <summary> 작업관리자 Task type </summary>
enum class TASK_TYPE
{
    CREATE_OBJECT,
    DESTROY_OBJECT,
    SPAWN_POOLED_OBJECT,
    CHANGE_LEVEL,
    CHANGE_LEVEL_STATE,
};

/*enum class LAYER_TYPE
{
    DEFAULT,
    BACKGROUND,
    TILE,
    PLAYER,
    PLAYER_PROJECTILE,
    ENEMY,
    ENEMY_PROJECTILE,
    END
};*/

enum class LIGHT_TYPE
{
    DIRECTIONAL,    // 방향성 광원 - 멀리서부터 오는 광원에게 설정(빛이 월드로 오는 방향이 모두 같다고 본다)
    POINT,          // 점광원 - 기본적인 광원, 한 점으로부터 주변으로 빛이 뻗어나감
    SPOT,           // 스포트라이트 - 특수한 광원, 빛을 특정 방향으로 모아서 보내는 것으로 봄
};

/// <summary> 재질이 렌더링되는 시점 </summary>
enum class RENDER_DOMAIN
{
    DOMAIN_OPAQUE,              // 불투명
    DOMAIN_MASKED,              // 불투명 + 완전 투명 (Discard 처리하는 Material의 경우)
    DOMAIN_TRANSPARENT,         // 반투명
    DOMAIN_TRANSPARENT_EFFECT,  // 반투명 effect
    DOMAIN_POSTPROCESS,         // 후처리 - 가장 마지막에 동작, 기존에 그려진 장면을 재 가공
    
    DOMAIN_DEBUG,       // 디버그 렌더링 전용 도메인
    
    DOMAIN_NONE,        // 미지정
};

enum class LEVEL_STATE
{
    PLAY,
    PAUSE,
    STOP
};
