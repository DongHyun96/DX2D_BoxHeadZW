#include "pch.h"
#include "CStructureHandler.h"

#include "GameEngine/03.Manager/03.KeyMgr/KeyMgr.h"
#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "Source/ScriptMgr.h"
#include "Source/Manager/GameManager.h"
#include "Source/Scripts/BackgroundTile/CBackgroundTile.h"
#include "Source/Scripts/CharacterScript/PlayerScript/CPlayerScript.h"
#include "Source/Scripts/CharacterScript/PlayerScript/InvenScript/CInvenScript.h"
#include "Source/Scripts/Structure/CStructure.h"

CStructureHandler::CStructureHandler()
    : CScript(SCRIPT_TYPE::STRUCTUREHANDLER)
{
}

CStructureHandler::~CStructureHandler()
{
}

void CStructureHandler::Begin()
{
    m_MainPlayerScript  = GetOwner()->GetScriptComponent<CPlayerScript>().Get();
    m_InvenScript       = GetOwner()->GetScriptComponent<CInvenScript>().Get();
    
    // Init Prefabs & Preview Objects
    Ptr<APrefab> pAsset{};
    if (pAsset = FIND_ASSET(APrefab, L"Prefab\\BarricadePrefab.pref"))
    {
        m_mapStructureTypePrefabs[PLAYER_STRUCTURE_TYPE::BARRICADE] = pAsset.Get();
    } else DebugUtil::AddDebugLog("[CStructureHandler::Begin] : Failed to find Barricade prefab", DEF_COLOR_CYAN, 10.f);
    
    if (pAsset = FIND_ASSET(APrefab, L"Prefab\\BarrelPrefab.pref"))
    {
        m_mapStructureTypePrefabs[PLAYER_STRUCTURE_TYPE::BARREL] = pAsset.Get();
    } else DebugUtil::AddDebugLog("[CStructureHandler::Begin] : Failed to find Barrel prefab", DEF_COLOR_CYAN, 10.f);
    
    if (pAsset = FIND_ASSET(APrefab, L"Prefab\\Turret_MGPrefab.pref"))
    {
        m_mapStructureTypePrefabs[PLAYER_STRUCTURE_TYPE::TURRET_MACHINE_GUN] = pAsset.Get();
    } else DebugUtil::AddDebugLog("[CStructureHandler::Begin] : Failed to find Turret_MG prefab", DEF_COLOR_CYAN, 10.f);
    
    if (pAsset = FIND_ASSET(APrefab, L"Prefab\\Turret_MortarPrefab.pref"))
    {
        m_mapStructureTypePrefabs[PLAYER_STRUCTURE_TYPE::TURRET_MORTAR] = pAsset.Get();
    } else DebugUtil::AddDebugLog("[CStructureHandler::Begin] : Failed to find Turret_Mortar prefab", DEF_COLOR_CYAN, 10.f);
    
    if (pAsset = FIND_ASSET(APrefab, L"Prefab\\Turret_RocketPrefab.pref"))
    {
        m_mapStructureTypePrefabs[PLAYER_STRUCTURE_TYPE::TURRET_ROCKET] = pAsset.Get();
    } else DebugUtil::AddDebugLog("[CStructureHandler::Begin] : Failed to find Turret_Rocket prefab", DEF_COLOR_CYAN, 10.f);
}

void CStructureHandler::Tick()
{
    CreateStructureHoldingPreviewIfNecessary();
    
    if (
        m_MainPlayerScript->GetHandState() != PLAYER_HANDSTATE::UNARMED ||
        m_MainPlayerScript->GetMainState() != PLAYER_MAINSTATE::IDLE ||
        !m_InvenScript->HasAnyStructure()
        )
    {
        m_mapStructureTypePreviewObjects[m_CurrentStructureHolding]->SetActive(false);
        return;
    }
    
    // 설치 처리 중
    
    CBackgroundTile* BackgroundCellManager = GM->GetBackgroundCellManager();
    
    UpdateCurrentStructureHolding();
    
    const Vec2 MousePos                 = KeyMgr::GetInst()->GetMouseWorldPos2D();
    const CellCoord cellCoord           = BackgroundCellManager->GetWorldPosToCellCoord(MousePos);
    const Vec2 MousePosToCellWorldPos   = BackgroundCellManager->GetCellCoordToWorldPos(cellCoord);
    const bool CellAvailable            = BackgroundCellManager->IsCellAvailable(cellCoord);
    
    const bool PreviewObjectOverlapping = m_mapStructureTypePreviewObjects[m_CurrentStructureHolding]->GetCollider2D()->IsCurrentlyOverlapping();
    const bool StructureAvailable = CellAvailable && !PreviewObjectOverlapping;
    
    // 두 가지를 체크해야 함 -> Cell 위치가 Available한 Cell인지 & Preview Object와 Ovelapped 중인 물체가 있는지
    
    UpdatePreviewStructureObject(MousePosToCellWorldPos, StructureAvailable);
    UpdateSpawnStructure(MousePosToCellWorldPos, StructureAvailable);
    
}

void CStructureHandler::UpdateCurrentStructureHolding()
{
    // E키를 눌러 다음 Structure Type으로 넘어가거나, 현재 들고있는 StructureHolding의 개수가 모두 소진되었을 때에, 바로 다음 StructureType으로 넘어간다
    if (KEY_TAP(KEY::E) || !m_InvenScript->HasStructure(m_CurrentStructureHolding))
        UpdateToNextStructureTypeHolding();
    
    // Wheel 로 조정
    if (KeyMgr::GetInst()->GetMouseWheel() == 1)
        UpdateToNextStructureTypeHolding();
    else if (KeyMgr::GetInst()->GetMouseWheel() == -1)
        UpdateToPrevStructureTypeHolding();

}

void CStructureHandler::UpdateToNextStructureTypeHolding()
{
    // PrevType Active 끄기
    m_mapStructureTypePreviewObjects[m_CurrentStructureHolding]->SetActive(false);
    
    const int End       = static_cast<int>(PLAYER_STRUCTURE_TYPE::END);
    
    int CurrentType = static_cast<int>(m_CurrentStructureHolding);

    // 최소 하나 이상의 Valid한 Structure 보장(위의 Tick 시작 전 해당 사항을 걸렀음)
    while (true)
    {
        if (++CurrentType >= End) CurrentType = 0;

        // 첫 Valid한 Structure Type 도달
        if (m_InvenScript->HasStructure(static_cast<PLAYER_STRUCTURE_TYPE>(CurrentType)))
        {
            m_CurrentStructureHolding = static_cast<PLAYER_STRUCTURE_TYPE>(CurrentType);
            return;
        }
    }
}

void CStructureHandler::UpdateToPrevStructureTypeHolding()
{
    // 이전 Type Active 끄기
    m_mapStructureTypePreviewObjects[m_CurrentStructureHolding]->SetActive(false);
    
    int CurrentType = static_cast<int>(m_CurrentStructureHolding);

    // 최소 하나 이상의 Valid한 Structure 보장(위의 Tick 시작 전 해당 사항을 걸렀음)
    while (true)
    {
        if (--CurrentType < 0) CurrentType = static_cast<int>(PLAYER_STRUCTURE_TYPE::END) - 1;

        // 첫 Valid한 Structure Type 도달
        if (m_InvenScript->HasStructure(static_cast<PLAYER_STRUCTURE_TYPE>(CurrentType)))
        {
            m_CurrentStructureHolding = static_cast<PLAYER_STRUCTURE_TYPE>(CurrentType);
            return;
        }
    }
}

void CStructureHandler::UpdateSpawnStructure(const Vec2& _PreviewPos, bool _Available)
{
    // 설치를 할 수 없는 상황
    if (!_Available) return;
    
    if (KEY_TAP(KEY::MLB))
    {
        GameObject* SpawnedStructure = m_mapStructureTypePrefabs[m_CurrentStructureHolding]->InstantiateAndSpawnToCurLevel();
        SpawnedStructure->Transform()->SetRelativePos(ToVec3(_PreviewPos, _PreviewPos.y)); 
        
        m_InvenScript->ReduceCurrentStructureCount(m_CurrentStructureHolding); // 갯수를 하나 줄인다
        
        // Taken Cell 기록
        GM->GetBackgroundCellManager()->SetCellTaken(_PreviewPos, true);
    }
}

void CStructureHandler::CreateStructureHoldingPreviewIfNecessary()
{
    if (!m_mapStructureTypePreviewObjects.empty()) return;
    
    // 마우스위 설치 위치를 보여줄 Preview Object 생성해서 Level에 집어넣기 (기본 상태는 Active false)
    for (const pair<const PLAYER_STRUCTURE_TYPE, APrefab*>& Pair : m_mapStructureTypePrefabs)
    {
        GameObject* NewStructureObject = Pair.second->InstantiateAndSpawnToCurLevel();
        
        NewStructureObject->SetActive(false);
        NewStructureObject->GetScriptComponent<CStructure>()->SetIsPreviewObject(true);
        
        Ptr<AMaterial> DynamicMtrl = NewStructureObject->GetRenderCom()->CreateDynamicMaterial();
        DynamicMtrl->SetScalar(SCALAR_PARAM::VEC4_0, Vec4(1.f, 1.f, 1.f, PREVIEW_ALPHA)); // 알파값을 조정처리했음 (프리뷰 오브젝트라)
        
        switch (Pair.first)
        {
        case PLAYER_STRUCTURE_TYPE::TURRET_MACHINE_GUN: case PLAYER_STRUCTURE_TYPE::TURRET_MORTAR: case PLAYER_STRUCTURE_TYPE::TURRET_ROCKET:
            NewStructureObject->FlipbookRender()->Stop(L"Turret", 0, 0); // Flipbook Structure의 경우, 멈춘 상태로 PreviewObject를 보여준다  
            break;            
        }
        
        m_mapStructureTypePreviewObjects[Pair.first] = NewStructureObject;
    }
}

void CStructureHandler::UpdatePreviewStructureObject(const Vec2& _PreviewPos, bool _Available)
{
    GameObject* PreviewObject = m_mapStructureTypePreviewObjects[m_CurrentStructureHolding];
    PreviewObject->SetActive(true);
    
    Vec3 Pos = ToVec3(_PreviewPos, -3500.f); 
    PreviewObject->Transform()->SetRelativePos(Pos);
    
    Vec4 Color = _Available ? Vec4(1.f, 1.f, 1.f, PREVIEW_ALPHA) : Vec4(1.f, 0.5f, 0.5f, PREVIEW_ALPHA);
    
    // Begin에서 생성처리한 DynamicMaterial 이다
    PreviewObject->GetRenderCom()->GetMaterial()->SetScalar(SCALAR_PARAM::VEC4_0, Color);
}
