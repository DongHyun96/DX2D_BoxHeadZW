#include "pch.h"
#include "EditorMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"

namespace
{
    enum class GizmoMode { None, Translate, Rotate, Scale };
    enum class GizmoAxis { None, X, Y, Both };

    struct GizmoState
    {
        GizmoMode mode = GizmoMode::None;
        bool dragging = false;
        GizmoAxis axis = GizmoAxis::None;

        Vec3 startPos = Vec3::Zero;
        Vec3 startScale = Vec3::One;
        float startRotZ = 0.f;
        float startMouseAngle = 0.f;
        Vec3 startMouseWorld = Vec3::Zero;

        Ptr<GameObject> lastTarget{};
    };

    GizmoState g_Gizmo{};

    void ToggleGizmoMode(GizmoMode mode)
    {
        g_Gizmo.dragging = false;
        g_Gizmo.axis = GizmoAxis::None;

        if (g_Gizmo.mode == mode) g_Gizmo.mode = GizmoMode::None;
        else g_Gizmo.mode = mode;
    }
}


void EditorMgr::GizmoToggleTick(const ImGuiIO& _io)
{
    if (!_io.WantTextInput)
    {
        const ImGuiInputFlags flags = ImGuiInputFlags_RouteGlobal;

        if (ImGui::Shortcut(ImGuiKey_W, flags)) ToggleGizmoMode(GizmoMode::Translate);
        if (ImGui::Shortcut(ImGuiKey_E, flags)) ToggleGizmoMode(GizmoMode::Rotate);
        if (ImGui::Shortcut(ImGuiKey_R, flags)) ToggleGizmoMode(GizmoMode::Scale);
    }
}

void EditorMgr::UpdateGizmo()
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP)
    {
        g_Gizmo.dragging = false;
        g_Gizmo.mode = GizmoMode::None;
        return;
    }

    Ptr<GameObject> TargetObject = m_Outliner->GetSelectedObject();
    if (!TargetObject || !TargetObject->Transform())
    {
        g_Gizmo.dragging = false;
        g_Gizmo.mode = GizmoMode::None;
        return;
    }

    if (g_Gizmo.mode == GizmoMode::None) return;

    if (g_Gizmo.lastTarget != TargetObject)
    {
        g_Gizmo.dragging = false;
        g_Gizmo.axis = GizmoAxis::None;
        g_Gizmo.lastTarget = TargetObject;
    }

    Vec3 mouseWorld = m_MainWindowDropDetectorUI->GetMouseWorldPosInSceneRect();

    const Vec3 worldPos = TargetObject->Transform()->GetWorldPos();
    const float EditorCamOrthoScale = RenderMgr::GetInst()->GetEditorCam()->Camera()->GetOrthoScale();
    const float axisLen     = 100.f * EditorCamOrthoScale;
    const float pickDist    = 16.f * EditorCamOrthoScale;
    const float rotRadius   = 60.f * EditorCamOrthoScale;
    const float scaleFactor = 0.5f * EditorCamOrthoScale;

    // ====== Draw Gizmo ======
    if (g_Gizmo.mode == GizmoMode::Translate)
    {
        DrawDebugLine(worldPos, worldPos + Vec3(axisLen, 0, 0), Vec4(1,0,0,1), 0.f);
        DrawDebugLine(worldPos, worldPos + Vec3(0, axisLen, 0), Vec4(0,1,0,1), 0.f);
    }
    else if (g_Gizmo.mode == GizmoMode::Rotate)
    {
        DrawDebugCircle(worldPos, rotRadius, Vec4(1,1,0,1), 0.f);
    }
    else if (g_Gizmo.mode == GizmoMode::Scale)
    {
        DrawDebugLine(worldPos, worldPos + Vec3(axisLen, 0, 0), Vec4(0,0.7f,1,1), 0.f);
        DrawDebugLine(worldPos, worldPos + Vec3(0, axisLen, 0), Vec4(0,0.7f,1,1), 0.f);

        DrawDebugRect(worldPos + Vec3(axisLen, 0, 0), Vec3(6.f, 6.f, 1.f), Vec3(0,0,0), Vec4(0,0.7f,1,1), 0.f);
        DrawDebugRect(worldPos + Vec3(0, axisLen, 0), Vec3(6.f, 6.f, 1.f), Vec3(0,0,0), Vec4(0,0.7f,1,1), 0.f);
    }

    ImGuiIO& io = ImGui::GetIO();

    // ====== Pick ======
    if (!g_Gizmo.dragging)
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !io.WantCaptureMouse)
        {
            Vec2 diff(mouseWorld.x - worldPos.x, mouseWorld.y - worldPos.y);
            const float dist = diff.Length();

            if (g_Gizmo.mode == GizmoMode::Translate)
            {
                if (dist <= pickDist) g_Gizmo.axis = GizmoAxis::Both;
                else if (fabsf(diff.y) <= pickDist && diff.x >= 0.f && diff.x <= axisLen) g_Gizmo.axis = GizmoAxis::X;
                else if (fabsf(diff.x) <= pickDist && diff.y >= 0.f && diff.y <= axisLen) g_Gizmo.axis = GizmoAxis::Y;

                if (g_Gizmo.axis != GizmoAxis::None)
                {
                    g_Gizmo.dragging = true;
                    g_Gizmo.startPos = TargetObject->Transform()->GetRelativePos();
                    g_Gizmo.startMouseWorld = mouseWorld;
                }
            }
            else if (g_Gizmo.mode == GizmoMode::Rotate)
            {
                if (dist <= rotRadius)
                {
                    g_Gizmo.dragging = true;
                    g_Gizmo.startRotZ = TargetObject->Transform()->GetRelativeRot().z;
                    g_Gizmo.startMouseAngle = atan2f(diff.y, diff.x);
                }
            }
            else if (g_Gizmo.mode == GizmoMode::Scale)
            {
                Vec2 handleX(worldPos.x + axisLen, worldPos.y);
                Vec2 handleY(worldPos.x, worldPos.y + axisLen);

                if ((Vec2(mouseWorld.x, mouseWorld.y) - handleX).Length() <= pickDist) g_Gizmo.axis = GizmoAxis::X;
                else if ((Vec2(mouseWorld.x, mouseWorld.y) - handleY).Length() <= pickDist) g_Gizmo.axis = GizmoAxis::Y;

                if (g_Gizmo.axis != GizmoAxis::None)
                {
                    g_Gizmo.dragging = true;
                    g_Gizmo.startScale = TargetObject->Transform()->GetRelativeScale();
                    g_Gizmo.startMouseWorld = mouseWorld;
                }
            }
        }
    }
    else
    {
        // ====== Drag ======
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            Vec3 delta = mouseWorld - g_Gizmo.startMouseWorld;

            if (g_Gizmo.mode == GizmoMode::Translate)
            {
                Vec3 pos = g_Gizmo.startPos;
                if (g_Gizmo.axis == GizmoAxis::X) pos.x += delta.x;
                else if (g_Gizmo.axis == GizmoAxis::Y) pos.y += delta.y;
                else if (g_Gizmo.axis == GizmoAxis::Both) { pos.x += delta.x; pos.y += delta.y; }

                TargetObject->Transform()->SetRelativePos(pos);
            }
            else if (g_Gizmo.mode == GizmoMode::Rotate)
            {
                Vec2 diff(mouseWorld.x - worldPos.x, mouseWorld.y - worldPos.y);
                float curAngle = atan2f(diff.y, diff.x);
                float deltaAngle = curAngle - g_Gizmo.startMouseAngle;
                Vec3 rot = TargetObject->Transform()->GetRelativeRot();
                rot.z = g_Gizmo.startRotZ + deltaAngle;
                TargetObject->Transform()->SetRelativeRot(rot);
            }
            else if (g_Gizmo.mode == GizmoMode::Scale)
            {
                Vec3 scale = g_Gizmo.startScale;

                if (g_Gizmo.axis == GizmoAxis::X)
                    scale.x = max(0.01f, g_Gizmo.startScale.x + delta.x * scaleFactor);
                else if (g_Gizmo.axis == GizmoAxis::Y)
                    scale.y = max(0.01f, g_Gizmo.startScale.y + delta.y * scaleFactor);

                TargetObject->Transform()->SetRelativeScale(scale);
            }
        }
        else
        {
            g_Gizmo.dragging = false;
            g_Gizmo.axis = GizmoAxis::None;
        }
    }
}