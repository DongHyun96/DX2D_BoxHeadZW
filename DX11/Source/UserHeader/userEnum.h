
/// <summary>
/// Eight Directions
/// </summary>
enum class EDIRECTION
{
    RIGHT,      // 1
    RIGHT_UP,   // 2
    UP,         // 3
    UP_LEFT,    // 4
    LEFT,       // 5
    LEFT_DOWN,  // 6
    DOWN,       // 7
    DOWN_RIGHT, // 8
    END     
};

/// <summary>
/// Runner Enemy용 Sixteen direction -> 길찾기 알고리즘 사용이 아닌, 해당 방향 직진으로 달려갈 때 Anim 방향 State를 정하기 위함
/// </summary>
enum class SIXTEEN_DIRECTION
{
    RIGHT,          // 1
    
    RIGHT_UP1,      // 2
    RIGHT_UP2,      // 3
    RIGHT_UP3,      // 4
    
    UP,             // 5
    
    UP_LEFT1,       // 6
    UP_LEFT2,       // 7
    UP_LEFT3,       // 8
    
    LEFT,           // 9
    
    LEFT_DOWN1,     // 10
    LEFT_DOWN2,     // 11
    LEFT_DOWN3,     // 12
    
    DOWN,           // 13
    
    DOWN_RIGHT1,    // 14
    DOWN_RIGHT2,    // 15
    DOWN_RIGHT3,    // 16
    
    END
};

/// <summary>
/// Player HandState
/// </summary>
enum class PLAYER_HANDSTATE
{
    UNARMED, // 설치물 설치 가능한 State
    PISTOL,
    UZI,
    SHOTGUN,
    MINIGUN,
    ROCKET,
    END
};

enum class PLAYER_MAINSTATE
{
    IDLE,
    PUSHED_OUT,
    DIE,
    END
};

/// <summary>
/// 설치물 Type
/// </summary>
enum class PLAYER_STRUCTURE_TYPE
{
    BARRICADE,
    BARREL,
    TURRET_MACHINE_GUN,
    TURRET_MORTAR,
    TURRET_ROCKET,
    END
};

/// <summary>
/// Enemy MainState
/// </summary>
enum class ENEMY_MAINSTATE
{
    WALK,
    ATTACK,
    PUSHED_OUT,
    DIE,
    END
};

/// <summary>
/// P_HANDSTATE -> CFlipbookRender에서 지정한 카테고리 항목 이름
/// </summary>
static const map<PLAYER_HANDSTATE, wstring> mapPlayerHandStateAnimCategory =
{
    { PLAYER_HANDSTATE::UNARMED,    L"UnArmed"},
    { PLAYER_HANDSTATE::PISTOL,     L"Pistol"},
    { PLAYER_HANDSTATE::UZI,        L"Uzi"},
    { PLAYER_HANDSTATE::SHOTGUN,    L"Shotgun"},
    { PLAYER_HANDSTATE::MINIGUN,    L"MiniGun"},
    { PLAYER_HANDSTATE::ROCKET,     L"Rocket"}
};


enum class ENEMY_TYPE
{
    ZOMBIE,     // 0
    MUMMY,      // 1
    RUNNER,     // 2
    VAMPIRE,    // 3
    DEVIL,      // 4
    END
};

enum class ROUND_STATE
{
    WAIT,
    ROUND_GOING,
    GAME_OVER
};

// 우선순위로 따지면 : Die / PushedOut / Attack / Walk 순
static const map<ENEMY_MAINSTATE, wstring> mapEnemyMainStateAnimCategory = 
{
    {ENEMY_MAINSTATE::WALK,         L"Walk"},
    {ENEMY_MAINSTATE::ATTACK,       L"Attack"},
    {ENEMY_MAINSTATE::PUSHED_OUT,   L"PushedOut"},
    {ENEMY_MAINSTATE::DIE,          L"Die"}
};

static const map<PLAYER_MAINSTATE, wstring> mapPlayerMainStateAnimCategory =
{
    // Attack은 애초에 없고, Walk의 경우 현재 HandState에 따른 Animation으로 이어진다
    {PLAYER_MAINSTATE::PUSHED_OUT, L"PushedOut"},    
    {PLAYER_MAINSTATE::DIE, L"Die"},    
};

static const wstring& GetEnemyMainAnimCategory(ENEMY_MAINSTATE _EMainState)
{
    if (!mapEnemyMainStateAnimCategory.contains(_EMainState)) return L"";
    return mapEnemyMainStateAnimCategory.at(_EMainState);
}

static const wstring& GetPlayerMainAnimCategory(PLAYER_MAINSTATE _PlayerState)
{
    if (!mapPlayerMainStateAnimCategory.contains(_PlayerState)) return L"";
    return mapPlayerMainStateAnimCategory.at(_PlayerState);
}

//////////////////////////////////////////////////////////////////////////////////////////

enum FIRST_SPAWN_LOC
{
    FIRST_SPAWN_LOC_NONE,
    FIRST_SPAWN_LOC1,
    FIRST_SPAWN_LOC2,
    FIRST_SPAWN_LOC3,
    FIRST_SPAWN_LOC4,
    FIRST_SPAWN_LOC5,
    FIRST_SPAWN_LOC6,
};
