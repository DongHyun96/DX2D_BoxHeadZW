#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"
#include "GameEngine/07.EditorUI/01.Inspector/Inspector.h"
#include "GameEngine/07.EditorUI/02.Outliner/Outliner.h"
#include "GameEngine/07.EditorUI/09.MainWindowDropDetectorUI/MainWindowDropDetectorUI.h"

class EditorMgr : public Singleton<EditorMgr>
{
    
    SINGLE(EditorMgr);

private:

    map<string, Ptr<EditorUI>>  m_mapUI{};
    bool                        m_bShowDemo{};
    Ptr<EditorUI>               m_FocusedUI{};

    vector<Ptr<GameObject>>     m_vecEditorObject{};
    
private:

    vector<Ptr<Inspector>>      m_vecInspector{};
    vector<Ptr<Inspector>>      m_PendingRemoveInspector{};
    
private:
    
    Ptr<Outliner>                   m_Outliner{};
    Ptr<MainWindowDropDetectorUI>   m_MainWindowDropDetectorUI{};
    
public:
    
    void AddUI(const string& _UIName, const Ptr<EditorUI>& _UI);
    
    Ptr<EditorUI> FindUI(const string& _UIName);
    Ptr<EditorUI> GetEditorUI(const string& _UIName) { return FindUI(_UIName); }
    
public:

    bool IsShowDemo() const { return m_bShowDemo; }
    void ShowDemo(bool _Show) { m_bShowDemo = _Show; }

    void RegisterFocusedUI(const Ptr<EditorUI>& _UI) { m_FocusedUI = _UI; }

    const vector<Ptr<GameObject>>& GetEditorGameObjects() const { return m_vecEditorObject; }
    
    const vector<Ptr<Inspector>>& GetInspectors() const { return m_vecInspector; }
    
    void SetTargetObjectToInspectors(const Ptr<GameObject>& _GameObject);
    
    void SetTargetAssetToInspectors(const Ptr<Asset>& _Asset);
    
public:
    
    void AddInspector();
    bool RemoveInspector(const Ptr<Inspector>& _Inspector);
    
public:
    
    void Init();
    void Progress();
    
private:
    
    void Tick();
    void Render();
    
private:
    void CreateEditorUI();
    void CreateEditorObject();
    
private:

    void GizmoToggleTick(const ImGuiIO& _io);
    void UpdateGizmo();
    
private:
    
    void FlushPendingInspectorRemove();
    
};
