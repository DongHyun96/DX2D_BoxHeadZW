

#define GM GameManager::GetInst()


#define COLOR_WHITE        Vec4(1.f, 1.f, 1.f, 1.f)
#define COLOR_BLACK        Vec4(0.f, 0.f, 0.f, 1.f)
#define COLOR_TRANSPARENT  Vec4(0.f, 0.f, 0.f, 0.f)

// Gray 계열
#define COLOR_GRAY         Vec4(0.5f, 0.5f, 0.5f, 1.f)
#define COLOR_LIGHTGRAY    Vec4(0.75f, 0.75f, 0.75f, 1.f)
#define COLOR_DARKGRAY     Vec4(0.25f, 0.25f, 0.25f, 1.f)

// 기본 RGB
#define COLOR_RED          Vec4(1.f, 0.f, 0.f, 1.f)
#define COLOR_GREEN        Vec4(0.f, 1.f, 0.f, 1.f)
#define COLOR_BLUE         Vec4(0.f, 0.f, 1.f, 1.f)

// 보조 색상
#define COLOR_YELLOW       Vec4(1.f, 1.f, 0.f, 1.f)
#define COLOR_CYAN         Vec4(0.f, 1.f, 1.f, 1.f)
#define COLOR_MAGENTA      Vec4(1.f, 0.f, 1.f, 1.f)

// UI에서 자주 쓰이는 색
#define COLOR_ORANGE       Vec4(1.f, 0.5f, 0.f, 1.f)
#define COLOR_PURPLE       Vec4(0.5f, 0.f, 1.f, 1.f)
#define COLOR_PINK         Vec4(1.f, 0.4f, 0.7f, 1.f)
#define COLOR_BROWN        Vec4(0.6f, 0.3f, 0.1f, 1.f)

// Alpha 조절용
#define COLOR_WHITE_HALF   Vec4(1.f, 1.f, 1.f, 0.5f)
#define COLOR_BLACK_HALF   Vec4(0.f, 0.f, 0.f, 0.5f)



constexpr float CAMERA2D_POS_Z = -5000.f;