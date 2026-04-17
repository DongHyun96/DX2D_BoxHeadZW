#include "pch.h"
#include "CameraUI.h"

#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/04.Asset/04.Level/ALevel.h"

CameraUI::CameraUI()
    : ComponentUI(COMPONENT_TYPE::CAMERA, "CameraUI")
{
    m_ComponentTitle = "Camera";
}

CameraUI::~CameraUI()
{
}

void CameraUI::Tick_UI()
{
    ComponentUI::Tick_UI();
    
    UINT        m_LayerCheck{};     // 어떤 레이어만 화면에 렌더링 할 것인지 비트체크


    // 현재 레벨에서 유일한 UICam 및 FirstMainCamera인지 체킹
    ImGui::BeginDisabled(true);
    bool IsFirstMainCam = GetTargetObject()->Camera()->GetIsFirstMainCamera();
    bool IsUICam        = GetTargetObject()->Camera()->GetIsUICamera();
    ImGui::Checkbox("IsFirstMainCamera", &IsFirstMainCam);
    ImGui::Checkbox("IsUICam", &IsUICam);
    ImGui::EndDisabled();
    
    // const char* items[] = { "AAAA", "BBBB", "CCCC" };
    // static int item_current = 0; // 선택한 Item에서의 Index 번호
    // ImGui::Combo("combo", &item_current, items, 3);
    // ImGui::Combo("combo", &item_current, items, IM_COUNTOF(items));

    /* Projection Type setting */
    ImGui::Text("ProjectionType");
    ImGui::SameLine();
    PROJ_TYPE ProjType = GetTargetObject()->Camera()->GetType();
    const char* items[] = { "ORTHOGRAPHIC", "PERSPECTIVE" };
    if (ImGui::Combo("##ProjectionType", reinterpret_cast<int*>(&ProjType), items, IM_COUNTOF(items)))
        GetTargetObject()->Camera()->SetType(ProjType);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    /* Far */
    ImGui::Text("Far");
    ImGui::SameLine(100);
    float Far = GetTargetObject()->Camera()->GetFar();
    if (ImGui::SliderFloat("##Far", &Far, 5.f, 10000.f))
        GetTargetObject()->Camera()->SetFar(Far);

    /* Ortho Width */
    ImGui::BeginDisabled(ProjType == PROJ_TYPE::PERSPECTIVE);
    ImGui::Text("OrthoWidth");
    ImGui::SameLine(100);
    float Width = GetTargetObject()->Camera()->GetWidth();
    if (ImGui::SliderFloat("##Width", &Width, 0.1f, 3000.f))
        GetTargetObject()->Camera()->SetWidth(Width);
    ImGui::EndDisabled();
    
    /* AspectRatio */
    ImGui::Text("AspectRatio");
    ImGui::SameLine(100);
    float Ratio = GetTargetObject()->Camera()->GetAspectRatio();
    if (ImGui::DragFloat("##AspectRatio", &Ratio))
        GetTargetObject()->Camera()->SetAspectRatio(Ratio);
    
    /* FOV */
    ImGui::BeginDisabled(ProjType == PROJ_TYPE::ORTHOGRAPHIC);
    ImGui::Text("FOV");
    ImGui::SameLine(100);
    float FOV = GetTargetObject()->Camera()->GetFOV();
    if (ImGui::DragFloat("##FOV", &FOV, 0.1f))
    {
        if (FOV > 1.f) GetTargetObject()->Camera()->SetFOV(FOV);
    }
    ImGui::EndDisabled();
    
    /* OrthoScale */
    ImGui::BeginDisabled(ProjType == PROJ_TYPE::PERSPECTIVE);
    ImGui::Text("OrthoScale");
    ImGui::SameLine(100);
    float OrthoScale = GetTargetObject()->Camera()->GetOrthoScale();
    if (ImGui::DragFloat("##OrthoScale", &OrthoScale, 0.01f))
    {
        if (OrthoScale > 0.f) GetTargetObject()->Camera()->SetOrthoScale(OrthoScale);
    }
    ImGui::EndDisabled();
    
    /* LayerCheck */

    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    
    string LayerTitleName = "<LAYER>";
    float windowWidth = ImGui::GetWindowSize().x;
    float textWidth   = ImGui::CalcTextSize(LayerTitleName.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(LayerTitleName.c_str());

    
    // 하나라도 꺼져 있으면 전부 켜기
    // 전부 켜져 있으면 전부 끄기
    if (ImGui::Button("Toggle All layers"))
    {
        UINT LayerCheck = GetTargetObject()->Camera()->GetLayerCheck();
        if (LayerCheck == 0xffffffff) GetTargetObject()->Camera()->LayerCheckClear();
        else GetTargetObject()->Camera()->LayerCheckAll();
    }

    ImGui::Spacing(); ImGui::Spacing();
    
    if (ImGui::TreeNode("LayerCheck"))
    {
        UINT LayerCheck = GetTargetObject()->Camera()->GetLayerCheck();
        Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
        
        string strLayerName[MAX_LAYER] = {};
        bool selected[MAX_LAYER] = {};

        for (UINT i = 0; i < MAX_LAYER; ++i)
        {
            wstring Name = pCurLevel->GetLayer(i)->GetName();
            strLayerName[i] = string(Name.begin(), Name.end());

            if (strLayerName[i].empty())
            {
                char buff[255] = {};
                sprintf_s(buff, "%d. None", i);
                strLayerName[i] = buff;
            }
            else strLayerName[i] = to_string(i) + ". " + strLayerName[i]; 
             
            selected[i] = LayerCheck & (1 << i);
        }
        
        if (ImGui::BeginTable("##LayerCheck", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
        {
            for (int i = 0; i < 32; i++)
            {  
                ImGui::TableNextColumn();

                if (ImGui::Selectable(strLayerName[i].c_str(), &selected[i]))
                {
                    GetTargetObject()->Camera()->LayerCheck(i);
                }
            }
            ImGui::EndTable();
        }

        SetSizeAsChild(Vec2(0.f, 1100.f));

        ImGui::TreePop();
    }
    else
    {
        SetSizeAsChild(Vec2(0.f, 300.f));
    }
}
