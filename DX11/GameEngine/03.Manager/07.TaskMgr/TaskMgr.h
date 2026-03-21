#pragma once

class TaskMgr : public Singleton<TaskMgr>
{
    
    SINGLE(TaskMgr);

private:

    vector<TaskInfo>        m_vecTask{};
    vector<Ptr<GameObject>> m_Garbage{};

public:
    
    void AddTask(const TaskInfo& _Info) { m_vecTask.push_back(_Info); }
    
public:
    
    void Progress();
    
};
