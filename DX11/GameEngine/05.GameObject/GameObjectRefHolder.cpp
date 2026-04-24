#include "pch.h"
#include "GameObjectRefHolder.h"

#include "GameEngine/04.Asset/04.Level/ALevel.h"

GameObjectRefHolder::GameObjectRefHolder()
{
}

GameObjectRefHolder::GameObjectRefHolder(const GameObjectRefHolder& _Origin)
    : m_RefGUID(_Origin.m_RefGUID) // GUID만 복사하고, GameObject는 다시금 연결처리를 해주어야 함
{
}

GameObjectRefHolder::GameObjectRefHolder(GameObject* _Object)
    : m_GameObject(_Object)
{
    if (_Object) m_RefGUID = _Object->GetGUID();
}

GameObjectRefHolder::GameObjectRefHolder(GUID _Guid)
    : m_RefGUID(_Guid)
{
}

GameObjectRefHolder::~GameObjectRefHolder()
{
}

void GameObjectRefHolder::SetGameObject(GameObject* _GameObject)
{
    m_GameObject = _GameObject;
    m_RefGUID    = (m_GameObject == nullptr) ? GUID_NULL : m_GameObject->GetGUID();
}

void GameObjectRefHolder::SaveToLevelFile(FILE* _File)
{
    fwrite(&m_RefGUID, sizeof(GUID), 1, _File);
}

void GameObjectRefHolder::LoadFromLevelFile(FILE* _File)
{
    fread(&m_RefGUID, sizeof(GUID), 1, _File);
}

void GameObjectRefHolder::LinkReferenceToGameObject(const Ptr<ALevel>& _Level)
{
    if (!_Level) return;
    m_GameObject = _Level->GetObjectByGUID(m_RefGUID);
}
