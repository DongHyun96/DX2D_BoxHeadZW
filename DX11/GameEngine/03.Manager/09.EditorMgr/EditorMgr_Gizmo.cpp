#include "pch.h"
#include "EditorMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"

namespace
{
    enum class GizmoMode { None, Translate, Rotate, Scale, Pivot };
    enum class GizmoAxis { None, X, Y, Both };

    struct GizmoTargetStart
    {
        Ptr<GameObject> object{};
        Vec3 startPos = Vec3::Zero;
        Vec3 startScale = Vec3::One;
        float startRotZ = 0.f;
        Vec3 startPivot = Vec3::Zero;
        Matrix parentEffectInv = XMMatrixIdentity();
    };

    struct GizmoState
    {
        GizmoMode mode = GizmoMode::None;
        bool dragging = false;
        GizmoAxis axis = GizmoAxis::None;

        float startMouseAngle = 0.f;
        Vec3 startMouseWorld = Vec3::Zero;
        Vec3 startPivotWorld = Vec3::Zero;

        vector<GizmoTargetStart> startTargets{};
        vector<Ptr<GameObject>> lastTargets{};
    };

    GizmoState g_Gizmo{};

    bool IsSameSelection(const vector<Ptr<GameObject>>& _Lhs, const vector<Ptr<GameObject>>& _Rhs)
    {
        if (_Lhs.size() != _Rhs.size()) return false;
        if (_Lhs.empty()) return true;

        vector<bool> matched(_Rhs.size(), false);

        for (const Ptr<GameObject>& lhsObj : _Lhs)
        {
            bool found = false;
            for (size_t i = 0; i < _Rhs.size(); ++i)
            {
                if (matched[i]) continue;
                if (_Rhs[i] != lhsObj) continue;

                matched[i] = true;
                found = true;
                break;
            }

            if (!found) return false;
        }

        return true;
    }

    Matrix CalcPivotDeltaWorldToLocalMatrix(const Ptr<GameObject>& _Target)
    {
        if (!_Target || !_Target->Transform()) return XMMatrixIdentity();

        Matrix parentEffect = XMMatrixIdentity();
        Ptr<GameObject> parent = _Target->GetParent();

        if (parent && parent->Transform())
        {
            parentEffect = parent->Transform()->GetWorldMatrix();

            if (_Target->Transform()->GetIndependentScale())
            {
                Vec3 parentScale = parent->Transform()->GetWorldScale();
                if (fabsf(parentScale.x) > FLT_EPSILON
                    && fabsf(parentScale.y) > FLT_EPSILON
                    && fabsf(parentScale.z) > FLT_EPSILON)
                {
                    Matrix matParentScale = XMMatrixScaling(parentScale.x, parentScale.y, parentScale.z);
                    Matrix matParentScaleInv = XMMatrixInverse(nullptr, matParentScale);
                    parentEffect = matParentScaleInv * parentEffect;
                }
            }
        }

        return XMMatrixInverse(nullptr, parentEffect);
    }

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
        if (ImGui::Shortcut(ImGuiKey_Q, flags)) ToggleGizmoMode(GizmoMode::Pivot);
    }
}

void EditorMgr::UpdateGizmo()
{
    if (LevelMgr::GetInst()->GetLevelState() != LEVEL_STATE::STOP)
    {
        g_Gizmo.dragging = false;
        g_Gizmo.mode = GizmoMode::None;
        g_Gizmo.axis = GizmoAxis::None;
        g_Gizmo.startTargets.clear();
        g_Gizmo.lastTargets.clear();
        return;
    }

    vector<Ptr<GameObject>> targetObjects = m_Outliner->GetSelectedObjects();
    vector<Ptr<GameObject>> validTargets{};
    validTargets.reserve(targetObjects.size());

    for (const Ptr<GameObject>& target : targetObjects)
    {
        if (!target || !target->Transform()) continue;

        bool alreadyAdded = false;
        for (const Ptr<GameObject>& existing : validTargets)
        {
            if (existing == target)
            {
                alreadyAdded = true;
                break;
            }
        }
        if (!alreadyAdded) validTargets.push_back(target);
    }

    if (validTargets.empty())
    {
        g_Gizmo.dragging = false;
        g_Gizmo.mode = GizmoMode::None;
        g_Gizmo.axis = GizmoAxis::None;
        g_Gizmo.startTargets.clear();
        g_Gizmo.lastTargets.clear();
        return;
    }

    if (g_Gizmo.mode == GizmoMode::None) return;

    if (!IsSameSelection(g_Gizmo.lastTargets, validTargets))
    {
        g_Gizmo.dragging = false;
        g_Gizmo.axis = GizmoAxis::None;
        g_Gizmo.startTargets.clear();
        g_Gizmo.lastTargets = validTargets;
    }

    Vec3 mouseWorld = m_MainWindowDropDetectorUI->GetMouseWorldPosInSceneRect();

    Vec3 worldPos = Vec3::Zero;
    vector<Vec3> pivotWorldPoints{};
    pivotWorldPoints.reserve(validTargets.size());
    for (const Ptr<GameObject>& target : validTargets)
    {
        worldPos += target->Transform()->GetWorldPos();
        const Vec3 pivotWorld = Vec3::Transform(target->Transform()->GetPivot(), target->Transform()->GetWorldMatrix());
        pivotWorldPoints.push_back(pivotWorld);
    }
    const float targetCountInv = 1.f / static_cast<float>(validTargets.size());
    worldPos.x *= targetCountInv;
    worldPos.y *= targetCountInv;
    worldPos.z *= targetCountInv;
    const float EditorCamOrthoScale = RenderMgr::GetInst()->GetEditorCam()->Camera()->GetOrthoScale();
    const float axisLen     = 100.f * EditorCamOrthoScale;
    const float pickDist    = 16.f * EditorCamOrthoScale;
    const float rotRadius   = 60.f * EditorCamOrthoScale;
    const float scaleFactor = 0.5f * EditorCamOrthoScale;
    const float pivotRadius = 5.f * EditorCamOrthoScale;
    const float pivotPickDist = 12.f * EditorCamOrthoScale;

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
    else if (g_Gizmo.mode == GizmoMode::Pivot)
    {
        for (const Vec3& pivotWorld : pivotWorldPoints)
            DrawDebugCircle(pivotWorld, pivotRadius, Vec4(1,1,0,1), 0.f);
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
                    g_Gizmo.startMouseWorld = mouseWorld;
                    g_Gizmo.startPivotWorld = worldPos;
                    g_Gizmo.startTargets.clear();
                    g_Gizmo.startTargets.reserve(validTargets.size());

                    for (const Ptr<GameObject>& target : validTargets)
                    {
                        GizmoTargetStart start{};
                        start.object = target;
                        start.startPos = target->Transform()->GetRelativePos();
                        start.startScale = target->Transform()->GetRelativeScale();
                        start.startRotZ = target->Transform()->GetRelativeRot().z;
                        g_Gizmo.startTargets.push_back(start);
                    }
                }
            }
            else if (g_Gizmo.mode == GizmoMode::Rotate)
            {
                if (dist <= rotRadius)
                {
                    g_Gizmo.dragging = true;
                    g_Gizmo.startPivotWorld = worldPos;
                    g_Gizmo.startMouseAngle = atan2f(diff.y, diff.x);
                    g_Gizmo.startMouseWorld = mouseWorld;
                    g_Gizmo.startTargets.clear();
                    g_Gizmo.startTargets.reserve(validTargets.size());

                    for (const Ptr<GameObject>& target : validTargets)
                    {
                        GizmoTargetStart start{};
                        start.object = target;
                        start.startPos = target->Transform()->GetRelativePos();
                        start.startScale = target->Transform()->GetRelativeScale();
                        start.startRotZ = target->Transform()->GetRelativeRot().z;
                        g_Gizmo.startTargets.push_back(start);
                    }
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
                    g_Gizmo.startMouseWorld = mouseWorld;
                    g_Gizmo.startPivotWorld = worldPos;
                    g_Gizmo.startTargets.clear();
                    g_Gizmo.startTargets.reserve(validTargets.size());

                    for (const Ptr<GameObject>& target : validTargets)
                    {
                        GizmoTargetStart start{};
                        start.object = target;
                        start.startPos = target->Transform()->GetRelativePos();
                        start.startScale = target->Transform()->GetRelativeScale();
                        start.startRotZ = target->Transform()->GetRelativeRot().z;
                        g_Gizmo.startTargets.push_back(start);
                    }
                }
            }
            else if (g_Gizmo.mode == GizmoMode::Pivot)
            {
                Vec2 mousePos(mouseWorld.x, mouseWorld.y);
                int pickedIdx = -1;
                float closestDist = pivotPickDist + 1.f;

                for (size_t i = 0; i < pivotWorldPoints.size(); ++i)
                {
                    const Vec2 targetPivotPos(pivotWorldPoints[i].x, pivotWorldPoints[i].y);
                    const float curDist = (mousePos - targetPivotPos).Length();
                    if (curDist <= pivotPickDist && curDist < closestDist)
                    {
                        closestDist = curDist;
                        pickedIdx = static_cast<int>(i);
                    }
                }

                if (pickedIdx != -1)
                {
                    g_Gizmo.dragging = true;
                    g_Gizmo.axis = GizmoAxis::Both;
                    g_Gizmo.startMouseWorld = mouseWorld;
                    g_Gizmo.startPivotWorld = pivotWorldPoints[pickedIdx];
                    g_Gizmo.startTargets.clear();
                    g_Gizmo.startTargets.reserve(1);

                    const Ptr<GameObject>& target = validTargets[pickedIdx];
                    GizmoTargetStart start{};
                    start.object = target;
                    start.startPos = target->Transform()->GetRelativePos();
                    start.startScale = target->Transform()->GetRelativeScale();
                    start.startRotZ = target->Transform()->GetRelativeRot().z;
                    start.startPivot = target->Transform()->GetPivot();
                    start.parentEffectInv = CalcPivotDeltaWorldToLocalMatrix(target);
                    g_Gizmo.startTargets.push_back(start);
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
                for (const GizmoTargetStart& start : g_Gizmo.startTargets)
                {
                    if (!start.object || !start.object->Transform()) continue;

                    Vec3 pos = start.startPos;
                    if (g_Gizmo.axis == GizmoAxis::X) pos.x += delta.x;
                    else if (g_Gizmo.axis == GizmoAxis::Y) pos.y += delta.y;
                    else if (g_Gizmo.axis == GizmoAxis::Both)
                    {
                        pos.x += delta.x;
                        pos.y += delta.y;
                    }

                    start.object->Transform()->SetRelativePos(pos);
                }
            }
            else if (g_Gizmo.mode == GizmoMode::Rotate)
            {
                Vec2 diff(mouseWorld.x - g_Gizmo.startPivotWorld.x, mouseWorld.y - g_Gizmo.startPivotWorld.y);
                float curAngle = atan2f(diff.y, diff.x);
                float deltaAngle = curAngle - g_Gizmo.startMouseAngle;

                for (const GizmoTargetStart& start : g_Gizmo.startTargets)
                {
                    if (!start.object || !start.object->Transform()) continue;

                    Vec3 rot = start.object->Transform()->GetRelativeRot();
                    rot.z = start.startRotZ + deltaAngle;
                    start.object->Transform()->SetRelativeRot(rot);
                }
            }
            else if (g_Gizmo.mode == GizmoMode::Scale)
            {
                for (const GizmoTargetStart& start : g_Gizmo.startTargets)
                {
                    if (!start.object || !start.object->Transform()) continue;

                    Vec3 scale = start.startScale;

                    if (g_Gizmo.axis == GizmoAxis::X)
                        scale.x = max(0.01f, start.startScale.x + delta.x * scaleFactor);
                    else if (g_Gizmo.axis == GizmoAxis::Y)
                        scale.y = max(0.01f, start.startScale.y + delta.y * scaleFactor);

                    start.object->Transform()->SetRelativeScale(scale);
                }
            }
            else if (g_Gizmo.mode == GizmoMode::Pivot)
            {
                for (const GizmoTargetStart& start : g_Gizmo.startTargets)
                {
                    if (!start.object || !start.object->Transform()) continue;

                    Vec3 localDelta = Vec3::TransformNormal(delta, start.parentEffectInv);
                    localDelta.z = 0.f;

                    Vec3 pivot = start.startPivot + localDelta;
                    start.object->Transform()->SetPivot(pivot);
                }
            }
        }
        else
        {
            g_Gizmo.dragging = false;
            g_Gizmo.axis = GizmoAxis::None;
            g_Gizmo.startTargets.clear();
        }
    }
}
