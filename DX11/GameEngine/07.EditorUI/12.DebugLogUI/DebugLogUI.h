#pragma once
#include "GameEngine/07.EditorUI/EditorUI.h"

struct DebugLogUIData;

class DebugLogUI : public EditorUI
{
private:

    static list<DebugLogUIData> m_listDebugLogs;

public:
    
    DebugLogUI();
    virtual ~DebugLogUI() override;
    
public:
    void Tick_UI() override;
    
public:
    
    static void AddDebugInfo(const DebugLogUIData& _Log) { m_listDebugLogs.push_back(_Log); }
    
};
