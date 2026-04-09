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
};

struct ExplosionSpawnDesc
{
    Vec3        SpawnPos{};
    float       ExplosionSizeFactor              = 1.f;
    float       FPS                              = 800.f;
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
