
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
/// Player HandState
/// </summary>
enum class PLAYER_HANDSTATE
{
    UNARMED,
    PISTOL,
    UZI,
    SHOTGUN,
    MINIGUN,
    ROCKET,
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


/// <summary>
/// Enemy MainState
/// </summary>
enum class ENEMY_STATE
{
    WALK,
    ATTACK,
    PUSHED_OUT,
    DIE,
    END
};

static const map<ENEMY_STATE, wstring> mapEnemyMainStateAnimCategory = 
{
    {ENEMY_STATE::WALK,         L"Walk"},
    {ENEMY_STATE::ATTACK,       L"Attack"},
    {ENEMY_STATE::PUSHED_OUT,   L"PushedOut"},
    {ENEMY_STATE::DIE,          L"Die"}
};

