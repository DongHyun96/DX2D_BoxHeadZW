#pragma once


#define PATH_CHAR_SIZE 255

class PathMgr : public Singleton<PathMgr>
{
	SINGLE(PathMgr);
	
private:

	// 윈도우 경로명 제한은 255글자까지
	wchar_t m_ContentPath[PATH_CHAR_SIZE];
	wchar_t m_CollisionTablePath[PATH_CHAR_SIZE]; // Not in used
	
public:
	
	void Init();
	
private:

	void InitPath(const wstring& _TargetFolder, wchar_t* _InitingPath);

public:
	
	const wchar_t* GetContentPath() const { return m_ContentPath; }
	const wchar_t* GetCollisionTablePath() const { return m_CollisionTablePath; }
	
};

#define CONTENT_PATH wstring(PathMgr::GetInst()->GetContentPath())
#define COLLISION_TABLE_PATH wstring(PathMgr::GetInst()->GetCollisionTablePath())