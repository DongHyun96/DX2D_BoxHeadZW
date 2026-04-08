#include "pch.h"
#include "TaskMgr.h"

#include "GameEngine/03.Manager/04.AssetMgr/AssetMgr.h"
#include "GameEngine/03.Manager/05.LevelMgr/LevelMgr.h"
#include "GameEngine/04.Asset/04.Level/ALevel.h"
#include "GameEngine/05.GameObject/GameObject.h"

TaskMgr::TaskMgr()
{
}

TaskMgr::~TaskMgr()
{
}

void TaskMgr::Progress()
{
    // 쓰레기통 수거 (스마트포인터의 RefCount를 0으로 처리)
    m_Garbage.clear();

    // Task 처리
    for (const TaskInfo& task : m_vecTask)
    {
        switch (task.Type)
        {
        case TASK_TYPE::CREATE_OBJECT: // 주의 Level Begin중 처리 제대로 안됨
        {
            
            Ptr<GameObject> gObject = reinterpret_cast<GameObject*>(task.Param_0);
            Ptr<ALevel> pCurLevel   = LevelMgr::GetInst()->GetCurLevel();
            
            pCurLevel->AddObject(task.Param_1, gObject);
            pCurLevel->SetChanged();

            // 레벨에 추가된 오브젝트는, 레벨 시작시점 때 Begin을 호출받지 못하기 때문에, 여기서 Begin처리
            // 레벨이 Play모드일 때에만 Begin 처리
            if (LevelMgr::GetInst()->GetLevelState() == LEVEL_STATE::PLAY)
                gObject->Begin();
        }
            break;
        case TASK_TYPE::DESTROY_OBJECT:
        {
            Ptr<GameObject> gObject = reinterpret_cast<GameObject*>(task.Param_0);
            
            if (gObject->IsObjectDestroyed()) continue;
            
            gObject->m_ObjectDestroyed = true; // 지워질 Object 마킹 체크 (Tick 한 번은 호출되게끔 처리)
            m_Garbage.push_back(gObject);
            
            Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
            pCurLevel->SetChanged();
        }
            break;
        case TASK_TYPE::SPAWN_POOLED_OBJECT:
        {
            Ptr<GameObject> gObject = reinterpret_cast<GameObject*>(task.Param_0);
            gObject->SetActive(true);
        }
            break;
        case TASK_TYPE::CHANGE_LEVEL:
        {
            const wchar_t* pLevelName = reinterpret_cast<const wchar_t*>(task.Param_0);
            bool NextLevelStateToStop = static_cast<bool>(task.Param_1);
            
            Ptr<ALevel> pLevel = FIND_ASSET(ALevel, pLevelName);
            LevelMgr::GetInst()->ChangeCurLevel(pLevel, NextLevelStateToStop);
        }
            break;
        case TASK_TYPE::CHANGE_LEVEL_STATE:
        {
            LEVEL_STATE NextState = static_cast<LEVEL_STATE>(task.Param_0);
            LevelMgr::GetInst()->ChangeCurLevelState(NextState);
        }
            break;
        }   
    }
    m_vecTask.clear();
}
