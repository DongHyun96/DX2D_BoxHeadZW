#ifndef _STRUCT
#define _STRUCT

// Light2D 정보
struct Light2DInfo
{
    int         Type;
    float3      Color;
    float3      Ambient;  // 환경광, 광원이 존재하면서 최소한으로 발생하는 빛의 세기
    float3      LightDir; // 광원의 빛이 향하는 방향
    float3      WorldPos; // 광원의 위치 (Point, Spot)
    float       Radius;   // 빛의 영향 반경 (Point, Spot)
    float       Angle;    // SpotLight 범위 각
};

// Particle
struct tParticle
{
    float4  LocalPos;
    float4  WorldPos;
    float4  WorldInitScale;
    float4  WorldScale;
    float4  Color;

    float4  Force;
    float4  Velocity;
    float   Mass;
        
    float   NoiseForceAccTime;
    float3  NoiseForceDir;
    
    float   Age;
    float   Life;
    float   NormalizedAge;

    int     Active;
};

// SpawnCount
struct tSpawnCount
{
    int     SpawnCount;
    float3  vPadding;
};

// Particle Module
struct tParticleModule
{
    // Spawn Module
    float	SpawnRate;
	float4	StartColor;
    float3  MinScale;
    float3  MaxScale;
	float	MinLife;
	float	MaxLife;
	int		SpawnShape;		// 0 : Box, 1 : Sphere
    float3  SpawnShapeScale;
	int		SpaceType;		// 0 : Local, 1 : World
    
    uint    BlockSpawnShape; // 0 : Box,  1: Sphere
    float3  BlockSpawnShapeScale; // SpawnShapeScale.x == Radius
    
    // Spawn Burst
    uint    SpawnBurstCount;
    uint    SpawnBurstRepeat;       // Burst Ƚ��
    float   SpawnBurstRepeatTime;   // Brush ���ݽð�
    
    // Add Velocity
    uint    AddVelocityType; // 0 : Random, 1 : FromCenter, 2 : ToCenter, 4 : Fixed 
    float3  AddVelocityFixedDir;
    float   AddMinSpeed;
    float   AddMaxSpeed;
            
    // Scale
    float   StartScale;
    float   EndScale;
    
    // Drag Module
    float   DestNormalizedAge;
    float   LimitSpeed;
    
    // Noise Force Module
	float	NoiseForceTerm;
	float	NoiseForceScale;
    
    // Render Module
    float4  EndColor;
    int     FadeOut;
    float   StartRatio;
    uint    VelocityAlignment;
    uint    CrossMesh;
    
    float3  ObjectWorldPos;
    int     Module[7];   
};

#endif
