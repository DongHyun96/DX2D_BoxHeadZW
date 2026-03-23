#include "pch.h"
#include "DebugLogUI.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"

list<DebugLogUIData> DebugLogUI::m_listDebugLogs{};

DebugLogUI::DebugLogUI()
    : EditorUI("DebugLogUI")
{
}

DebugLogUI::~DebugLogUI()
{
}

void DebugLogUI::Tick_UI()
{
    list<DebugLogUIData>::iterator iter = m_listDebugLogs.begin();
    
    while (iter != m_listDebugLogs.end())
    {
        DebugLogUIData& Log = *(iter);
        
        ImGui::TextColored(Log.Color, Log.LogStr.c_str());
        
        Log.Age += E_DT;
        
        if (Log.Age > Log.TotalLifeTime) iter = m_listDebugLogs.erase(iter);
        else ++iter;
    }
}
