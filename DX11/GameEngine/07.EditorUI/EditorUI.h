#pragma once

#include "ImGui/imgui.h"

class EditorUI : public Entity
{
private:

    string                  m_UIName{};         // EditorUI 윈도우창 이름
    string                  m_UIKey{};          // 실제 키값

    bool                    m_IsModal{};        // 모달창인지 아닌지
    bool                    m_Active = true;
    bool                    m_Separator = true;

    EditorUI*               m_Parent{};
    vector<Ptr<EditorUI>>   m_vecChildUI{};

    Vec2                    m_SizeAsChild{};    // 자식 UI인 경우, 담당하는 영역 크기
    
public:

    EditorUI(const string& _Name);
    virtual ~EditorUI() override;
    
public:

    GET_SET(Vec2, SizeAsChild)
    GET_SET(string, UIName)

    void SetSeparator(bool _Separator) { m_Separator = _Separator; }
    void SetModal(bool _IsModal) { m_IsModal = _IsModal; }
    
    bool IsActive() const { return m_Active; }
    void SetActive(bool _Active)
    {
        if (m_Active == _Active) return;
        
        m_Active = _Active;
        
        if (m_Active) Activate();
        else Deactivate();
    }

protected:

    SET(string, UIKey)
    const string& GetUIKey() const { return m_UIKey; }

public:
    
    void AddChildUI(const Ptr<EditorUI>& _Child)
    {
        _Child->m_Parent = this;
        m_vecChildUI.push_back(_Child);
    }
    
    Ptr<EditorUI> GetParentUI() const { return m_Parent; }
    
private:
    
    /// <summary> 현재 Tick에서 자신이 Focused 되었는지 체크해서 EditorMgr에게 알림 </summary>
    void CheckFocus();
    
public:
    
    virtual void Tick();

protected:
    
    virtual void Tick_UI() = 0;
    
private:
    
    virtual void Activate() {}
    virtual void Deactivate() {}
    
public:
    
    static bool ShortCut(ImGuiKeyChord _KeyChord, ImGuiInputFlags _Flags = ImGuiInputFlags_RouteFocused);
    
public:
    
    static void ColoredButtonTitle(const string& _Title, const ImVec4& _Color = ImVec4(0.6f, 0.239f, 0.239f, 1.f));
    
};

typedef void(Entity::* DELEGATE_0)(void);
typedef void(Entity::* DELEGATE_1)(DWORD_PTR);
typedef void(Entity::* DELEGATE_2)(DWORD_PTR, DWORD_PTR);

typedef void(Entity::* DELEGATE_BOOL)(bool);

