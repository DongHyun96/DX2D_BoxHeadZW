#include "pch.h"
#include "CBackgroundTile.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"
#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"

const float CBackgroundTile::BLOODSTAIN_START_ALPHA = 0.5f;
const float CBackgroundTile::SCORCH_START_ALPHA     = 0.75f;

CBackgroundTile::CBackgroundTile()
    : CScript(SCRIPT_TYPE::BACKGROUNDTILE)
{
    m_CellTaken.assign(CELL_ROW_COUNT, vector<bool>(CELL_ROW_COUNT, false));
}

CBackgroundTile::~CBackgroundTile()
{
}

void CBackgroundTile::Init()
{
    
}

void CBackgroundTile::Begin()
{
    /*const Vec3 Scale = Transform()->GetWorldScale();
    
    m_WorldSize         = Scale.x; // 4500
    m_TileRowCount      = TileRender()->GetTileMap()->GetRow(); // 75
    m_TileSize          = m_WorldSize / m_TileRowCount; // 60*/
    
    GM->SetBackgroundCellManager(this);
    
    // Decal Sprite 정보 setting
    
    // Setting Blood stains
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (i == 2 && j == 1) break; // 이후로 Scorch Sprite 정보
            
            const wstring SpriteKey = L"Sprite\\BloodStainAndScorch" + to_wstring(i) + L"_" + to_wstring(j) + L".sprite";
            Ptr<ASprite> Sprite = FIND_ASSET(ASprite, SpriteKey);
            m_BloodStainSprites.push_back(Sprite);
        }
    }

    for (int i = 1; i <= 2; ++i)
    {
        const wstring SpriteKey = L"Sprite\\BloodStainAndScorch" + to_wstring(2) + L"_" + to_wstring(i) + L".sprite";
        Ptr<ASprite> Sprite = FIND_ASSET(ASprite, SpriteKey);
        m_ScorchSprites.push_back(Sprite);
    }
}

void CBackgroundTile::Tick()
{
    UpdateSpawnedDecal(m_BloodStainSpawned);
    UpdateSpawnedDecal(m_ScorchSpawned);
}

void CBackgroundTile::UpdateSpawnedDecal(RandomizedSet<SpawnedDecalData>& _SpawnedDecalData)
{
    static float DECAL_LIFETIME = 30.f;
    
    for (int i = 0; i < _SpawnedDecalData.size(); )
    {
        SpawnedDecalData& DecalData = _SpawnedDecalData.data()[i];
        
        DecalData.Timer += DT;
        
        if (DecalData.Timer > DECAL_LIFETIME)
        {
            TileRender()->RemoveDecal(DecalData.ID);
            _SpawnedDecalData.remove(DecalData);
        }
        else
        {
            DecalData.ColorAlpha = MappingToNewRange(DecalData.Timer, 0.f, DECAL_LIFETIME, BLOODSTAIN_START_ALPHA, 0.f); 
            TileRender()->SetDecalAlpha(DecalData.ID, DecalData.ColorAlpha);
            ++i;
        }
    }
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
    if (IsCellCoordOutOfBounds(_CellCoord)) return false;
    return !m_CellTaken[_CellCoord.y][_CellCoord.x];    
}

bool CBackgroundTile::IsCellTaken(const CellCoord& _CellCoord) const
{
    if (IsCellCoordOutOfBounds(_CellCoord)) return false;

    if (m_CellTaken.size() <= _CellCoord.y) return false;
    if (m_CellTaken[_CellCoord.y].size() <= _CellCoord.x) return false;

    return m_CellTaken[_CellCoord.y][_CellCoord.x];
}

bool CBackgroundTile::SetCellTaken(const CellCoord& _CellCoord, bool _Taken)
{
    if (IsCellCoordOutOfBounds(_CellCoord))
    {
        DebugUtil::AddDebugLog("[CBackgroundTile::SetCellTaken] : Invalid Cell Coord received!", DEF_COLOR_RED, 20.f);
        return false;
    }
    m_CellTaken[_CellCoord.y][_CellCoord.x] = _Taken;
    return true;
}

bool CBackgroundTile::SetCellTaken(const Vec2& _WorldPos2D, bool _Taken)
{
    if (IsWorldPosOutOfBounds(_WorldPos2D))
    {
        DebugUtil::AddDebugLog("[CBackgroundTile::SetCellTaken] : Invalid WorldPos received!", DEF_COLOR_RED, 20.f);
        return false;
    }
    return SetCellTaken(GetWorldPosToCellCoord(_WorldPos2D), _Taken);
}

bool CBackgroundTile::IsWorldPosOutOfBounds(const Vec2& _WorldPos) const
{
    return _WorldPos.x < -m_WorldHalfSize || _WorldPos.x > m_WorldHalfSize ||
           _WorldPos.y < -m_WorldHalfSize || _WorldPos.y > m_WorldHalfSize;
}

bool CBackgroundTile::IsCellCoordOutOfBounds(const CellCoord& _CellCoord) const
{
    return _CellCoord.x < 0 || _CellCoord.x >= CELL_ROW_COUNT || _CellCoord.y < 0 || _CellCoord.y >= CELL_ROW_COUNT;
}

void CBackgroundTile::RemoveFirstSpawnDestination(FIRST_SPAWN_LOC _SpawnLoc, const CellCoord& _CellCoord)
{
    erase(m_FirstSpawnDestinations[_SpawnLoc], _CellCoord);
}

const CellCoord& CBackgroundTile::GetRandomFirstSpawnLocDestination(FIRST_SPAWN_LOC _SpawnLoc)
{
    if (m_FirstSpawnDestinations[_SpawnLoc].empty()) return CellCoord{};
    return m_FirstSpawnDestinations[_SpawnLoc].at(GetRandom(0, static_cast<int>(m_FirstSpawnDestinations[_SpawnLoc].size() - 1)));   
}

void CBackgroundTile::SpawnBloodStainDecal(const Vec2& _StainPos, const Vec2& _Scale)
{
    int SpawnedID = TileRender()->AddDecal(_StainPos, _Scale, PickRandom(m_BloodStainSprites), BLOODSTAIN_START_ALPHA);
    if (SpawnedID == -1) return;
    
    m_BloodStainSpawned.insert(SpawnedDecalData(SpawnedID, 0.f, BLOODSTAIN_START_ALPHA));
}

void CBackgroundTile::SpawnScorchDecal(const Vec2& _ScorchPos, const Vec2& _Scale)
{
    int SpawnedID = TileRender()->AddDecal(_ScorchPos, _Scale, PickRandom(m_ScorchSprites), BLOODSTAIN_START_ALPHA);
    if (SpawnedID == -1) return;
    
    m_ScorchSpawned.insert(SpawnedDecalData(SpawnedID, 0.f, BLOODSTAIN_START_ALPHA));
}

void CBackgroundTile::SaveToLevelFile(FILE* _File)
{
    for (const vector<bool>& ColVec : m_CellTaken)
    {
        for (const bool& Taken : ColVec)
            fwrite(&Taken, sizeof(bool), 1, _File);   
    }

    for (const pair<const FIRST_SPAWN_LOC, vector<CellCoord>>& Pair : m_FirstSpawnDestinations)
    {
        UINT SetSize = static_cast<UINT>(Pair.second.size());
        fwrite(&SetSize, sizeof(UINT), 1, _File);

        for (const CellCoord& cellCoord : Pair.second)
            fwrite(&cellCoord, sizeof(CellCoord), 1, _File);
    }
}

void CBackgroundTile::LoadFromLevelFile(FILE* _File)
{
    m_CellTaken.resize(CELL_ROW_COUNT);
    for (vector<bool>& ColVec : m_CellTaken)
        ColVec.resize(CELL_ROW_COUNT);
    
    for (vector<bool>& ColVec : m_CellTaken)
    {
        for (int i = 0; i < CELL_ROW_COUNT; i++)
        {
            bool Temp{};
            fread(&Temp, sizeof(bool), 1, _File);
            ColVec[i] = Temp;
        }
    }
    
    for (pair<const FIRST_SPAWN_LOC, vector<CellCoord>>& Pair : m_FirstSpawnDestinations)
    {
        UINT SetSize{};
        fread(&SetSize, sizeof(UINT), 1, _File);
        
        for (UINT i = 0; i < SetSize; i++)
        {
            CellCoord CellCoord{};
            fread(&CellCoord, sizeof(CellCoord), 1, _File);
            Pair.second.push_back(CellCoord);
        }
    }
}
