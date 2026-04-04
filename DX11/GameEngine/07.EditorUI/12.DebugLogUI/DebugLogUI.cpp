#include "pch.h"
#include "DebugLogUI.h"

#include "GameEngine/03.Manager/02.TimeMgr/TimeMgr.h"

list<DebugLogUIData> DebugLogUI::m_listDebugLogs{};
map<string, DebugLogUIData> DebugLogUI::m_mapPermanentDebugLogs{};

DebugLogUI::DebugLogUI()
    : EditorUI("DebugLogUI")
{
}

DebugLogUI::~DebugLogUI()
{
}

void DebugLogUI::Tick_UI()
{
    for (const pair<string, DebugLogUIData> Pair : m_mapPermanentDebugLogs)
    {
        const DebugLogUIData& Log = Pair.second;
        ImGui::TextColored(Log.Color, Log.LogStr.c_str());
    }
    
    list<DebugLogUIData>::iterator iter = m_listDebugLogs.begin();

    // 삭제 정방향으로 (이게 안전함)
    while (iter != m_listDebugLogs.end())
    {
        DebugLogUIData& Log = *iter;

        Log.Age += E_DT;

        if (Log.Age > Log.TotalLifeTime)
            iter = m_listDebugLogs.erase(iter);
        else ++iter;
    }
    
    // 출력 역방향 처리
    for (list<DebugLogUIData>::reverse_iterator iter = m_listDebugLogs.rbegin(); iter != m_listDebugLogs.rend(); ++iter)
    {
        const DebugLogUIData& Log = *iter;
        ImGui::TextColored(Log.Color, Log.LogStr.c_str());
    }
    
}
