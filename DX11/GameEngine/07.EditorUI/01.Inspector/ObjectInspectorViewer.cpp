#include "pch.h"
#include "ObjectInspectorViewer.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "Source/ScriptMgr.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/03.Manager/09.EditorMgr/EditorMgr.h"
#include "GameEngine/04.Asset/04.Level/ALevel.h"
#include "GameEngine/04.Asset/09.Prefab/APrefab.h"

#include "GameEngine/07.EditorUI/04.ComponentUI/01.TransformUI/TransformUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/02.Collider2DUI/ColliderRectUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/02.Collider2DUI/ColliderCircleUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/02.Collider2DUI/ColliderPointUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/03.CameraUI/CameraUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/04.LightUI/Light2DUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/01.MeshRenderUI/MeshRenderUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/02.BillboardRenderUI/BillboardRenderUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/03.SpriteRenderUI/SpriteRenderUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/04.FlipbookRenderUI/FlipbookRenderUI.h"
#include "GameEngine/07.EditorUI/04.ComponentUI/05.RenderUI/05.TileRenderUI/TileRenderUI.h"

#define ADD_COMPONENT_UI(ComponentType, type, Size)                          \
    m_arrComUI[static_cast<UINT>(ComponentType)] = new type;                 \
    m_arrComUI[static_cast<UINT>(ComponentType)]->SetSizeAsChild(Size);      \
    m_Owner->AddChildUI(m_arrComUI[static_cast<UINT>(ComponentType)].Get());

ObjectInspectorViewer::ObjectInspectorViewer()
{
}

ObjectInspectorViewer::~ObjectInspectorViewer()
{
}

void ObjectInspectorViewer::Init(EditorUI* _Owner)
{
    if (m_Owner) return;
    m_Owner = _Owner;
    CreateChildUI();
}

void ObjectInspectorViewer::TickHeaderUI()
{
    if (!m_TargetObject) return;

    const wstring Name    = m_TargetObject->GetName();
    string StrName      = string(Name.begin(), Name.end());
    if (StrName.empty()) StrName = "UnNamed";

    EditorUI::ColoredButtonTitle(StrName, ImVec4(0, 0.702, 0.161, 1));
    ImGui::SeparatorText("");

    TickAddScriptUI();
    ImGui::SameLine();
    TickAddComponentUI();
    ImGui::SameLine();
    TickCreateThisObjectAsPrefabUI();
     
    ImGui::SeparatorText("");
    
    bool bIsActive = m_TargetObject->GetIsActive();
    if (ImGui::Checkbox("IsActive", &bIsActive))
        m_TargetObject->SetIsActive(bIsActive);
    
    ImGui::SameLine();
    bool bIsVisible = m_TargetObject->GetIsVisible();
    if (ImGui::Checkbox("IsVisible", &bIsVisible))
        m_TargetObject->SetIsVisible(bIsVisible);
    
    ImGui::Spacing();

    ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader("Layer", ImGuiTreeNodeFlags_None))
        TickLayerUI();
    
    ImGui::Spacing();
    ImGui::SeparatorText("");
    
    
}

void ObjectInspectorViewer::CreateChildUI()
{
    ADD_COMPONENT_UI(COMPONENT_TYPE::TRANSFORM,         TransformUI,            Vec2(0.f, 200.f));
    ADD_COMPONENT_UI(COMPONENT_TYPE::COLLIDER2D_RECT,   ColliderRectUI,         Vec2(0.f, 200.f));
    ADD_COMPONENT_UI(COMPONENT_TYPE::COLLIDER2D_CIRCLE, ColliderCircleUI,       Vec2(0.f, 200.f));
    ADD_COMPONENT_UI(COMPONENT_TYPE::COLLIDER2D_POINT,  ColliderPointUI,        Vec2(0.f, 200.f));
    ADD_COMPONENT_UI(COMPONENT_TYPE::CAMERA,            CameraUI,               Vec2(0.f, 300.f));
    ADD_COMPONENT_UI(COMPONENT_TYPE::LIGHT2D,           Light2DUI,              Vec2(0.f, 200.f));
    ADD_COMPONENT_UI(COMPONENT_TYPE::MESH_RENDER,       MeshRenderUI,           Vec2(0.f, 200.f));
    ADD_COMPONENT_UI(COMPONENT_TYPE::BILLBOARD_RENDER,  BillboardRenderUI,      Vec2(0.f, 200.f));
    ADD_COMPONENT_UI(COMPONENT_TYPE::SPRITE_RENDER,     SpriteRenderUI,         Vec2(0.f, 200.f));
    ADD_COMPONENT_UI(COMPONENT_TYPE::FLIPBOOK_RENDER,   FlipbookRenderUI,       Vec2(0.f, 500.f));
    ADD_COMPONENT_UI(COMPONENT_TYPE::TILE_RENDER,       TileRenderUI,           Vec2(0.f, 500.f));
}

void ObjectInspectorViewer::SetTargetObject(const Ptr<GameObject>& _Object)
{
    m_TargetObject = _Object;

    for (UINT i = 0; i < static_cast<UINT>(COMPONENT_TYPE::END); ++i)
    {
        if (!m_arrComUI[i]) continue;
        m_arrComUI[i]->SetTargetObject(m_TargetObject);
    }

    RefreshScripts();
}

void ObjectInspectorViewer::RefreshScripts()
{
    if (!m_TargetObject)
    {
        for (size_t i = 0; i < m_vecScriptUI.size(); ++i)
            m_vecScriptUI[i]->SetScript(nullptr);
        return;
    }

    const vector<Ptr<CScript>>& vecScripts = m_TargetObject->GetScripts();

    if (m_vecScriptUI.size() < vecScripts.size())
    {
        int addCount = static_cast<int>(vecScripts.size() - m_vecScriptUI.size());
        for (int i = 0; i < addCount; ++i)
        {
            ScriptUI* pScriptUI = new ScriptUI;
            pScriptUI->SetSizeAsChild(Vec2(0.f, 150.f));
            m_Owner->AddChildUI(pScriptUI);
            m_vecScriptUI.push_back(pScriptUI);
        }
    }

    for (size_t i = 0; i < m_vecScriptUI.size(); ++i)
    {
        if (vecScripts.size() <= i)  m_vecScriptUI[i]->SetScript(nullptr);
        else                         m_vecScriptUI[i]->SetScript(vecScripts[i].Get());
    }
}

void ObjectInspectorViewer::TickLayerUI()
{
    string LayerTitleName = "<LAYER>";
    float windowWidth = ImGui::GetWindowSize().x;
    float textWidth = ImGui::CalcTextSize(LayerTitleName.c_str()).x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(LayerTitleName.c_str());

    const bool canEditLayer =
        (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::STOP) &&
        (m_TargetObject->GetParent() == nullptr);

    ImGui::BeginDisabled(!canEditLayer);

    Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
    const int curLayer = m_TargetObject->GetLayerIdx();

    if (ImGui::BeginTable("##ObjectLayer", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        for (int i = 0; i < MAX_LAYER; ++i)
        {
            wstring Name = pCurLevel->GetLayer(i)->GetName();
            string strLayerName = string(Name.begin(), Name.end());

            if (strLayerName.empty())
            {
                char buff[255] = {};
                sprintf_s(buff, "%d. None", i);
                strLayerName = buff;
            }
            else
            {
                strLayerName = to_string(i) + ". " + strLayerName;
            }

            ImGui::TableNextColumn();

            const bool selected = (i == curLayer);
            if (ImGui::Selectable(strLayerName.c_str(), selected))
            {
                if (curLayer != i) m_TargetObject->SetLayerIdx(i);
            }
        }

        ImGui::EndTable();
    }
    
    ImGui::EndDisabled();
}

void ObjectInspectorViewer::TickAddScriptUI()
{
    if (ImGui::Button("Add Script"))
        ImGui::OpenPopup("AddScriptPopup");

    if (ImGui::BeginPopup("AddScriptPopup"))
    {
        vector<wstring> vecScriptName{};
        ScriptMgr::GetScriptInfo(vecScriptName);

        for (const wstring& ScriptName : vecScriptName)
        {
            if (ImGui::MenuItem(string(ScriptName.begin(), ScriptName.end()).c_str()))
            {
                if (m_TargetObject)
                {
                    CScript* pNewScript = ScriptMgr::GetScript(ScriptName);
                    m_TargetObject->AddComponent(pNewScript);
                    SetTargetObject(m_TargetObject);
                }
            }
        }
        ImGui::EndPopup();
    }
}

void ObjectInspectorViewer::TickAddComponentUI()
{
    // Add Component UI
if (ImGui::Button("Add Component"))
    ImGui::OpenPopup("AddComponentPopup");

if (ImGui::BeginPopup("AddComponentPopup"))
{
    // Target 없으면 종료
    if (!m_TargetObject)
    {
        ImGui::EndPopup();
        return;
    }

    // Helper: 이미 존재하는 컴포넌트인지 확인
    auto HasCom = [&](COMPONENT_TYPE type) -> bool
    {
        return m_TargetObject->GetComponent(type) != nullptr;
    };
    
    auto EnsureTransform = [&]()
    {
        if (!m_TargetObject->GetComponent(COMPONENT_TYPE::TRANSFORM))
        {
            Ptr<CTransform> pNewTransform = new CTransform;
            pNewTransform->SetRelativeScale(200.f, 200.f, 1.f);
            m_TargetObject->AddComponent(pNewTransform.Get());
            
        }
    };

    // Transform은 대부분 항상 존재한다고 가정한다면, 그냥 숨겨도 됨
    // 필요시 아래처럼 조건 처리

    struct ComItem
    {
        const char* label{};
        COMPONENT_TYPE type{};
        
        
    };

    // 추가 가능한 컴포넌트 목록
    const ComItem items[] =
    {
        {"Transform",       COMPONENT_TYPE::TRANSFORM},
        {"Camera",          COMPONENT_TYPE::CAMERA},
        {"ColliderRect",    COMPONENT_TYPE::COLLIDER2D_RECT},
        {"ColliderCircle",  COMPONENT_TYPE::COLLIDER2D_CIRCLE},
        {"ColliderPoint",   COMPONENT_TYPE::COLLIDER2D_POINT},
        {"Light2D",         COMPONENT_TYPE::LIGHT2D},
        {"MeshRender",      COMPONENT_TYPE::MESH_RENDER},
        {"BillboardRender",      COMPONENT_TYPE::BILLBOARD_RENDER},
        {"SpriteRender",    COMPONENT_TYPE::SPRITE_RENDER},
        {"FlipbookRender",  COMPONENT_TYPE::FLIPBOOK_RENDER},
        {"TileRender",      COMPONENT_TYPE::TILE_RENDER},
    };

    const bool bHasAnyCollider = m_TargetObject->GetCollider2D().Get();
    
    for (const ComItem& item : items)
    {

        bool bSameGroupComponentHasAny{}; // 같은 묶음 그룹의 Component 중 하나라도 들고 있다면(CCollider2D, RenderComponent 종류) 
        
        if (item.type == COMPONENT_TYPE::COLLIDER2D_RECT ||
            item.type == COMPONENT_TYPE::COLLIDER2D_CIRCLE ||
            item.type == COMPONENT_TYPE::COLLIDER2D_POINT)
        {
            if (m_TargetObject->GetCollider2D()) bSameGroupComponentHasAny = true; // Collider 중 하나라도 들고 있음
        }
        
        if (
            item.type == COMPONENT_TYPE::MESH_RENDER ||
            item.type == COMPONENT_TYPE::BILLBOARD_RENDER ||
            item.type == COMPONENT_TYPE::SPRITE_RENDER || 
            item.type == COMPONENT_TYPE::FLIPBOOK_RENDER ||
            item.type == COMPONENT_TYPE::TILE_RENDER
            )
        {
            if (m_TargetObject->GetRenderCom()) bSameGroupComponentHasAny = true;
        }
        
        
        ImGui::BeginDisabled(HasCom(item.type) || bSameGroupComponentHasAny);

        if (ImGui::MenuItem(item.label))
        {
            Component* pNew = nullptr;

            switch (item.type)
            {
            case COMPONENT_TYPE::TRANSFORM:
            {
                CTransform* pNewTransform = new CTransform;
                pNewTransform->SetRelativeScale(200.f, 200.f, 1.f);
                pNew = pNewTransform;
            }
                break;
            case COMPONENT_TYPE::CAMERA:          pNew = new CCamera; break;
            case COMPONENT_TYPE::COLLIDER2D_RECT:
                EnsureTransform();
                pNew = new CColliderRect;
                break;
            case COMPONENT_TYPE::COLLIDER2D_CIRCLE:
                EnsureTransform();
                pNew = new CColliderCircle;
                break;
            case COMPONENT_TYPE::COLLIDER2D_POINT:
                EnsureTransform();
                pNew = new CColliderPoint;
                break;
            case COMPONENT_TYPE::LIGHT2D:               pNew = new CLight2D;          break;
            case COMPONENT_TYPE::MESH_RENDER:           pNew = new CMeshRender;       break;
            case COMPONENT_TYPE::BILLBOARD_RENDER:      pNew = new CBillboardRender;  break;
            case COMPONENT_TYPE::SPRITE_RENDER:         pNew = new CSpriteRender;     break;
            case COMPONENT_TYPE::FLIPBOOK_RENDER:       pNew = new CFlipbookRender;   break;
            case COMPONENT_TYPE::TILE_RENDER:           pNew = new CTileRender;       break;
            default: break;
            }

            if (pNew) m_TargetObject->AddComponent(pNew);
            
            EditorMgr::GetInst()->SetTargetObjectToInspectors(m_TargetObject);
        }

        ImGui::EndDisabled();
    }

    ImGui::EndPopup();
}
}

void ObjectInspectorViewer::TickCreateThisObjectAsPrefabUI()
{
    bool bIsChildObject = m_TargetObject->GetParent() != nullptr;
    bool bIsPrefabProtoObject = APrefab::IsPrefabPrototypeObject(m_TargetObject);

    // TODO : 엔진에서 제작한 GameObject들(Camera 등)은 어캄? 
    ImGui::BeginDisabled(bIsChildObject || bIsPrefabProtoObject); // 최상위 부모 오브젝트로만 Prefab을 만들 수 있도록 설정 & Prefab Prototype 오브젝트로 프리팹 생성 x
    if (ImGui::Button("Create this object as prefab"))
    {
        
        const wstring AssetName = m_TargetObject->GetName() + L"Prefab";
        
        Ptr<APrefab> Prefab = AssetMgr::GetInst()->CreateNewAsset<APrefab>(AssetName);
        Prefab->SetProtoObj(m_TargetObject);
        AssetMgr::GetInst()->AddAsset(Prefab->GetKey(), Prefab.Get());
        DebugUtil::AddDebugLog(AssetName + L" created", COLOR_YELLOW);
    }
    
    ImGui::EndDisabled();
}
