#pragma once

/// <summary>
/// Non-attachable
/// </summary>
class CWeaponScript : public CScript
{
private:

    static vector<UINT>    m_HitScanLayer;               // HitScan 처리용 layer 정보 미리 담아두기 -> Rocket과 Grenade의 경우 필요하지 않지만, HitScan 판정처리로 Fire하는 Weapon이 대부분이라 넣어둠
    
    float               m_TimeAfterLastFire{};        // 마지막 발사 이후로 흐른 시간
    float               m_FireIntervalTime{};         // 발사 사이의 시간간격    (버프 가능)
    float               m_DamageAmountPerRound{};     // 한 발당 Damage량       (버프 가능)

public:
    
    CWeaponScript(SCRIPT_TYPE _ScriptType);
    virtual ~CWeaponScript() override;
    
public: // 시점함수들 호출 안되는 것 유의 ( Non-Attachable Script )

    virtual void Init()             override final {}
    virtual void Begin()            override final {}
    virtual void AfterLevelBegin()  override final {}
    virtual void Tick()             override final {}
    
public:
    
    void WeaponTick(); // EquipmentScript 에서 호출처리해줄 것 / FireIntervalTime 계산해서 무기 발사 가능한지 Update

public:
    
    static void AddHitScanLayer(UINT _Layer) { m_HitScanLayer.push_back(_Layer); }
    
public:

    // Fire effect 생성, Projectile or NonProjectile에 따른 구분 (Projectile의 경우 탄 Spawn 시켜서 사격 개시)
    
    /// <summary>
    /// 사격 처리 시도 
    /// </summary>
    /// <param name="_MuzzleWorldPos"> : 탄 발사 지점 </param>
    /// <param name="_FireDirection"> : 사격 방향 </param>
    /// <returns> : 사격이 정상적으로 이루어졌다면 return true </returns>
    virtual bool Fire(const Vec2& _MuzzleWorldPos, const Vec2& _FireDirection) = 0;
    
    virtual void OnFireReleased() {};
    
protected:

    void RewindTimeAfterLastFire() { m_TimeAfterLastFire = 0.f; }
    float GetTimeAfterLastFire() const { return m_TimeAfterLastFire; }
    
    float GetDamageAmountPerRound() const { return m_DamageAmountPerRound; }
    
    const vector<UINT>& GetHitScanLayers() const { return m_HitScanLayer; }
    
public:
    
    float GetFireIntervalTime() const { return m_FireIntervalTime; }
    void SetFireIntervalTime(float _FireIntervalTime) { m_FireIntervalTime = _FireIntervalTime; }
    
    void SetDamageAmountPerRound(float _DamageAmountPerRound) {  m_DamageAmountPerRound = _DamageAmountPerRound; }
    
public:
    
    void SaveToLevelFile(FILE* _File) override {};
    void LoadFromLevelFile(FILE* _File) override {};
    
};
