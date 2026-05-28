#pragma once
#include "CStructure.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"

class CBarrel : public CStructure
{
private:

    CellCoord m_CellCoord{};
    
    // Adjacent CellCoord를 체크해서 해당 Barrel 연쇄 폭파 처리를 할 때 사용
    // 주의 : 자기자신이 Destroy되면 없앨 것
    static map<CBarrel*, CellCoord> m_mapSpawnedBarrel;
    
private: // 지연 폭파 관련 

    bool m_MarkedLateExplosion{};
    float m_LateExplodeTimer{};
    static const float m_LateExplodeWaitTime;
    
private:
    
    ExplosionSpawnDesc m_ExplosionDesc{};
    ExplosionSpawnDesc m_LateExplosionSpawnDesc{};
    
public:
    
    CBarrel();
    virtual ~CBarrel() override;
    CLONE(CBarrel)
    
public:
    
    virtual void Begin() override;
    virtual void Tick() override;

private:
    
    void UpdateLateExplosion();

public:
    
    bool IsMarkedLateExplosion() const { return m_MarkedLateExplosion; }
    
private:
    
    /// <summary>
    /// Adjacent CellCoord Barrel 체킹용 map에서 TargetBarrel 지우기 
    /// </summary>
    static void RemoveSpawnedBarrelFromStaticMap(CBarrel* _TargetToRemove);

    /// <summary>
    /// 자기 자신과 Adjacent한 Cell에 Barrel이 존재한다면, 해당 Barrel 폭파 처리
    /// </summary>
    void TryExplodeAdjacentCells();

private:
    
    virtual bool DestroyStructure(bool _DestroyedByDamaged) override;
    
private:
    
    /// <summary>
    /// 지연 폭파 처리 (연쇄 폭파 작동에서 처리됨) 
    /// </summary>
    void ExecuteLateExplosion();
    
};
