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
            
            if (gObject->m_ObjectDestroyed) continue;
            
            gObject->m_ObjectDestroyed = true; // 지워질 Object 마킹 체크 (Tick 한 번은 호출되게끔 처리)
            m_Garbage.push_back(gObject);

            // 부모가 비활성 상태여도 다음 프레임에 계층 FinalTick이 한 번은 돌도록 보장한다.
            // (자식 제거 + Collision EndOverlap/OverlapCount 정리를 위해 필요)
            GameObject* rootObject = gObject.Get();
            while (rootObject->GetParent()) rootObject = rootObject->GetParent();
            rootObject->m_ObjectMarkedDeactivated = true;
            
            // 자식 오브젝트 중, 오브젝트 pooling 처리된 자식이 존재하는 경우 해당 자식의 Parent를 nullptr로 세팅하고
            // Pool로 되돌아가야 한다 (recursively)
            
            queue<Ptr<GameObject>> q{};
            q.push(gObject);
            
            while (!q.empty())
            {
                Ptr<GameObject> CurrentChild = q.front(); q.pop();
                
                if (CurrentChild->GetOwnerPoolComponent())
                {
                    // 최상위 parent로 해방 처리
                    // 만일 poolComponent를 소유한 GameObject의 자식이었다면, Pool로 다시 돌아가는 처리만 해줌

                    if (!CurrentChild->GetOwnerPoolComponent()->IsPooledObjectAttachedToPooler())
                    {
                        CurrentChild->DisconnectWithParent(); 
                        CurrentChild->RegisterAsParent();
                    }
                    CurrentChild->SetActive(false); // Pool로 다시 돌아가는 처리
                }

                for (const Ptr<GameObject>& Child : CurrentChild->GetChildren())
                    q.push(Child);
            }
            
            Ptr<ALevel> pCurLevel = LevelMgr::GetInst()->GetCurLevel();
            pCurLevel->SetChanged();
        }
            break;
        case TASK_TYPE::SPAWN_POOLED_OBJECT:
        {
            Ptr<GameObject> gObject = reinterpret_cast<GameObject*>(task.Param_0);
            bool ActiveHierarchy = static_cast<bool>(task.Param_1);
            gObject->SetActive(true, ActiveHierarchy);
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
