#include "pch.h"
#include "CBackgroundTile.h"

#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

CBackgroundTile::CBackgroundTile()
    : CScript(SCRIPT_TYPE::BACKGROUNDTILE)
{
}

CBackgroundTile::~CBackgroundTile()
{
}

void CBackgroundTile::Begin()
{
    /*const Vec3 Scale = Transform()->GetWorldScale();
    
    m_WorldSize         = Scale.x; // 4500
    m_TileRowCount      = TileRender()->GetTileMap()->GetRow(); // 75
    m_TileSize          = m_WorldSize / m_TileRowCount; // 60*/
    
    GM->SetBackgroundCellManager(this);
}

void CBackgroundTile::Tick()
{
}

Vec2 CBackgroundTile::GetCellCoordToWorldPos(const CellCoord& _CellCoord) const
{
    const Vec2 StartPos = { -m_WorldHalfSize + m_TileHalfSize, -m_WorldHalfSize + m_TileHalfSize };
    return { StartPos.x + m_TileSize * _CellCoord.x, StartPos.y + m_TileSize * _CellCoord.y };    
}

CellCoord CBackgroundTile::GetWorldPosToCellCoord(const Vec2& _WorldPos) const
{
    const float startX = -m_WorldHalfSize + m_TileHalfSize;
    const float startY = -m_WorldHalfSize + m_TileHalfSize;

    const int x = static_cast<int>(lround((_WorldPos.x - startX) / m_TileSize));
    const int y = static_cast<int>(lround((_WorldPos.y - startY) / m_TileSize));

    return {x, y};
}

Vec2 CBackgroundTile::GetWorldPosToCellWorldPos(const Vec2& _WorldPos) const
{
    const CellCoord cellCoord = GetWorldPosToCellCoord(_WorldPos);
    return GetCellCoordToWorldPos(cellCoord);
}

bool CBackgroundTile::IsCellAvailable(const CellCoord& _CellCoord) const
{
    // Invalid Boundary
    if (_CellCoord.x < 0 || _CellCoord.x >= m_TileRowCount || _CellCoord.y < 0 || _CellCoord.y >= m_TileRowCount)
        return false;
    return !m_CellTaken[_CellCoord.x][_CellCoord.y];    
}

void CBackgroundTile::SaveToLevelFile(FILE* _File)
{
    for (const vector<bool>& ColVec : m_CellTaken)
    {
        for (const bool& Taken : ColVec)
            fwrite(&Taken, sizeof(bool), 1, _File);   
    }
}

void CBackgroundTile::LoadFromLevelFile(FILE* _File)
{
    m_CellTaken.resize(m_TileRowCount);
    for (vector<bool>& ColVec : m_CellTaken)
        ColVec.resize(m_TileRowCount);
    
    for (vector<bool>& ColVec : m_CellTaken)
    {
        for (int i = 0; i < m_TileRowCount; i++)
        {
            bool Temp{};
            fread(&Temp, sizeof(bool), 1, _File);
            ColVec[i] = Temp;
        }
    }
}
