#pragma once

/// <summary>
/// 주의 : 새로운 EffectPoolerType은 맨 마지막으로 추가할 것 (APrefab에 idx 순서로 잡아둔 데이터 때문에 전부 다 고쳐야 함)
/// </summary>
enum class FLIPBOOK_EFFECT_POOLER_TYPE
{
    MUZZLE_SMOKE_POOLER,
    MUZZLE_FLASH_POOLER,
    ROCKET_SMOKE_POOLER,
    EXPLOSION_DOME_EFFECT_POOLER,
    EXPLOSION_EFFECT_POOLER,
    END
};

class GameManager : public Singleton<GameManager>
{
    
    SINGLE(GameManager);

    /// 주의 : 여기서 Ptr로 잡아둔 GameObject의 경우, Destroy 처리가 제대로 이루어지지 않는다 (Garbage를 비워도 여기에 Reference를 들고 있기 때문)
    
private:
    
    GameObject* m_PlayerObject{};
    class CPlayerScript* m_MainPlayerScript{};

private: // BackgroundTile & Cell Manager

    class CBackgroundTile* m_BackgroundCellManager{};
    
private: // Poolers
    
    map<ENEMY_TYPE, CPoolComponent*> m_mapEnemyPoolers{};
    map<FLIPBOOK_EFFECT_POOLER_TYPE, CPoolComponent*> m_mapFlipbookEffectPoolers{};
    
    CPoolComponent* m_RocketProjectilePooler{};
    CPoolComponent* m_GrenadePooler{};
    
public:
    
    void OnLevelBegin();
    void OnLevelPlayToStop();
    void OnLevelChanged(class ALevel* _PrevLevel, ALevel* _NextLevel);
    
public:
    
    GameObject*	GetPlayerObject() const { return m_PlayerObject; }
    void SetPlayerObject(GameObject* _PlayerObject) { m_PlayerObject = _PlayerObject; }
    
    CPlayerScript* GetMainPlayerScript() const { return m_MainPlayerScript; }
    void SetMainPlayerScript(CPlayerScript* _Script) { m_MainPlayerScript = _Script; }
    
    void AddFlipbookEffectPooler(FLIPBOOK_EFFECT_POOLER_TYPE _PoolerType, CPoolComponent* _PoolComponent) { m_mapFlipbookEffectPoolers[_PoolerType] = _PoolComponent; }
    CPoolComponent* GetFlipbookEffectPooler(FLIPBOOK_EFFECT_POOLER_TYPE _PoolerType) const;
    
    void AddEnemyPooler(ENEMY_TYPE _EnemyType, CPoolComponent* _EnemyPooler) { m_mapEnemyPoolers[_EnemyType] = _EnemyPooler; }
    CPoolComponent* GetEnemyPooler(ENEMY_TYPE _EnemyType) const;
    
    void SetRocketProjectilePooler(CPoolComponent* _PoolComponent) { m_RocketProjectilePooler = _PoolComponent; }
    CPoolComponent* GetRocketProjectilePooler() const { return m_RocketProjectilePooler; }
    
    void SetGrenadePooler(CPoolComponent* _PoolComponent) { m_GrenadePooler = _PoolComponent; }
    CPoolComponent* GetGrenaderPooler() const { return m_GrenadePooler; }
    
    void SetBackgroundCellManager(CBackgroundTile* _BackgroundCellManager) { m_BackgroundCellManager = _BackgroundCellManager; }
    CBackgroundTile* GetBackgroundCellManager() const { return m_BackgroundCellManager; }
    
public: // Effect Spawn 관련

    void SpawnRocketSmoke(const Vec3& _SpawnPos);
    void SpawnExplosionDome(const Vec3& _SpawnPos, float _ExplosionSizeFactor = 1.f, float _FPS = 50.f, float _DamageAmount = 50.f, CScript* _SpawnedBy = nullptr);
    void SpawnExplosion
    (
        const Vec3& _SpawnPos,
        float _ExplosionSizeFactor = 1.f,
        float _FPS = 800.f,
        float _DamageAmount = 75.f,
        CScript* _SpawnedBy = nullptr,
        bool _UseSpawnExplosionCollisionForDamaging = true
    );
    
public: // Projectile Spawn 관련

    /// <summary>
    /// Rocket Projectile 스폰시키기
    /// </summary>
    /// <returns> 제대로 Spawn처리되지 않았다면 return false </returns>
    bool SpawnRocketProjectile(const Vec3& _SpawnPos, const Vec2& _Direction, float _Damage);

    /// <summary>
    /// 수류탄 spawn
    /// </summary>
    /// <param name="_SpawnPos">        : 스폰 위치 </param>
    /// <param name="_Direction">       : 던지는 방향 (Vec2) </param>
    /// <param name="_Damage">          : 데미지 량 </param>
    /// <param name="_BounceCount">     : 지면이나 물체에 튕길 수 있는 량 </param>
    /// <param name="_ThrowSpeedXY">    : 앞으로 던지는 Speed </param>
    /// <param name="_UpwardSpeed">     : 위로 던지는 Speed </param>
    /// <param name="_SpawnSubGrenade"> : SubGrenade 추가로 Spawn 시킬건지 </param>
    /// <param name="_IsSubGrenade"> : SubGrenade인지 (SubGrenade이라면, Scale을 좀 더 작게 준다) </param>
    /// <returns> : 제대로 Spawn되지 않았다면 return false </returns>
    bool SpawnGrenade
    (
        const Vec3&     _SpawnPos,
        const Vec2&     _Direction,
        float           _Damage,
        int             _BounceCount        = 3,
        float           _ThrowSpeedXY       = 400.f,
        float           _UpwardSpeed        = 300.f,
        bool            _SpawnSubGrenade    = false,
        bool            _IsSubGrenade       = false
    );
    
};
