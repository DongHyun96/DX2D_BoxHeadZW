
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

static const wstring& GetHandStateAnimCategory(PLAYER_HANDSTATE _HandState)
{
    return mapPlayerHandStateAnimCategory.at(_HandState);
}
