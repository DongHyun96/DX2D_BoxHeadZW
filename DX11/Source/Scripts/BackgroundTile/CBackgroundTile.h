#pragma once

/// <summary>
/// 설치물 설치 위치 기록
/// Cell Coord 관리
/// Enemy PathFinding 관련 Path 기록 -> Non-Available cell 기록
/// 
/// </summary>
class CBackgroundTile : public CScript
{
private:

    const float m_WorldSize     = 4500.f; // LevelWorldSize 가로세로 크기 (4500)
    const float m_WorldHalfSize = m_WorldSize * 0.5f;
    
    const float m_TileSize      = 60.f; // TileCell 한 칸당 크기 (가로, 세로 동일) (75)
    const float m_TileHalfSize  = 30.f;
    
private:

    // (y, x) Cell 좌표 상의 Cell에 물체가 있는지 저장
    vector<vector<bool>> m_CellTaken{};
    
public:
    
    CBackgroundTile();
    virtual ~CBackgroundTile() override;
    CLONE(CBackgroundTile)
    
public:

    virtual void Begin() override;
    virtual void Tick() override;

public:
    
    float GetWorldSize()        const { return m_WorldSize; }
    float GetWorldSizeHalf()    const { return m_WorldHalfSize; }
    float GetTileSize()         const { return m_TileSize; }

public:
    /// <summary>
    /// CellCoord에 대응되는 WorldPos 반환
    /// </summary>
    /// <param name="_CellCoord"></param>
    /// <returns></returns>
    Vec2 GetCellCoordToWorldPos(const CellCoord& _CellCoord) const;
    
public:
    
    /// <summary>
    /// WorldPos에 대응되는 CellCoord 반환
    /// </summary>
    /// <param name="_WorldPos"></param>
    /// <returns></returns>
    CellCoord GetWorldPosToCellCoord(const Vec2& _WorldPos) const;    
    CellCoord GetWorldPosToCellCoord(float x, float y) const { return GetWorldPosToCellCoord(Vec2(x, y)); }
    
    /// <summary>
    /// WorldPos에 대응되는 Cell의 WorldPos 반환
    /// </summary>
    Vec2 GetWorldPosToCellWorldPos(const Vec2& _WorldPos) const;  
    Vec2 GetWorldPosToCellWorldPos(float _WorldPosX, float _WorldPosY) const { return GetWorldPosToCellWorldPos(Vec2(_WorldPosX, _WorldPosY)); }
    
public:
    
    /// <summary>
    /// 해당 WorldPos의 Cell 위치가 설치물 또는, Enemy가 이동하기 가능한 위치인지 
    /// </summary>
    bool IsCellAvailable(const Vec2& _WorldPos) const { return IsCellAvailable(GetWorldPosToCellCoord(_WorldPos)); }

    /// <summary>
    /// 해당 Cell 위치가 설치물 또는, Enemy가 이동하기 가능한 위치인지
    /// </summary>
    bool IsCellAvailable(const CellCoord& _CellCoord) const;
    bool IsCellTaken(const CellCoord& _CellCoord) const;

    /// <summary>
    /// 해당 Cell 좌표의 Taken 처리 
    /// </summary>
    /// <returns></returns>
    bool SetCellTaken(const CellCoord& _CellCoord, bool _Taken);

    bool SetCellTaken(const Vec2& _WorldPos2D, bool _Taken);
    
    /// <summary>
    /// 해당 WorldPos가 바운더리를 넘어가는지 체크
    /// </summary>
    bool IsWorldPosOutOfBounds(const Vec2& _WorldPos) const;
    
    bool IsCellCoordOutOfBounds(const CellCoord& _CellCoord) const;
    
public:
    
    void SaveToLevelFile(FILE* _File) override;
    void LoadFromLevelFile(FILE* _File) override;
    
};
