#include "pch.h"
#include "ScriptMgr.h"

#include "Source/Scripts/AirplaneShootingScript\CBackgroundScript.h"
#include "Source/Scripts/AirplaneShootingScript\CBulletMgrScript.h"
#include "Source/Scripts/AirplaneShootingScript\CBulletScript.h"
#include "Source/Scripts/CCamMoveScript.h"
#include "Source/Scripts/CMonsterScript.h"
#include "Source/Scripts/Obstacle\CObstacle.h"
#include "Source/Scripts/PlayerScript\CPlayerAnimHandler.h"
#include "Source/Scripts/PlayerScript\CPlayerScript.h"
#include "Source/Scripts/PlayerScript\CPlayerSwapWeapon.h"

void ScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CBackgroundScript");
	_vec.push_back(L"CBulletMgrScript");
	_vec.push_back(L"CBulletScript");
	_vec.push_back(L"CCamMoveScript");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"CObstacle");
	_vec.push_back(L"CPlayerAnimHandler");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CPlayerSwapWeapon");
}

CScript * ScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CBackgroundScript" == _strScriptName)
		return new CBackgroundScript;
	if (L"CBulletMgrScript" == _strScriptName)
		return new CBulletMgrScript;
	if (L"CBulletScript" == _strScriptName)
		return new CBulletScript;
	if (L"CCamMoveScript" == _strScriptName)
		return new CCamMoveScript;
	if (L"CMonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"CObstacle" == _strScriptName)
		return new CObstacle;
	if (L"CPlayerAnimHandler" == _strScriptName)
		return new CPlayerAnimHandler;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CPlayerSwapWeapon" == _strScriptName)
		return new CPlayerSwapWeapon;
	return nullptr;
}

CScript * ScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::BACKGROUNDSCRIPT:
		return new CBackgroundScript;
	case (UINT)SCRIPT_TYPE::BULLETMGRSCRIPT:
		return new CBulletMgrScript;
	case (UINT)SCRIPT_TYPE::BULLETSCRIPT:
		return new CBulletScript;
	case (UINT)SCRIPT_TYPE::CAMMOVESCRIPT:
		return new CCamMoveScript;
	case (UINT)SCRIPT_TYPE::MONSTERSCRIPT:
		return new CMonsterScript;
	case (UINT)SCRIPT_TYPE::OBSTACLE:
		return new CObstacle;
	case (UINT)SCRIPT_TYPE::PLAYERANIMHANDLER:
		return new CPlayerAnimHandler;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
	case (UINT)SCRIPT_TYPE::PLAYERSWAPWEAPON:
		return new CPlayerSwapWeapon;
	}
	return nullptr;
}

const wchar_t * ScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::BACKGROUNDSCRIPT:
		return L"CBackgroundScript";
	case SCRIPT_TYPE::BULLETMGRSCRIPT:
		return L"CBulletMgrScript";
	case SCRIPT_TYPE::BULLETSCRIPT:
		return L"CBulletScript";
	case SCRIPT_TYPE::CAMMOVESCRIPT:
		return L"CCamMoveScript";
	case SCRIPT_TYPE::MONSTERSCRIPT:
		return L"CMonsterScript";
	case SCRIPT_TYPE::OBSTACLE:
		return L"CObstacle";
	case SCRIPT_TYPE::PLAYERANIMHANDLER:
		return L"CPlayerAnimHandler";
	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
	case SCRIPT_TYPE::PLAYERSWAPWEAPON:
		return L"CPlayerSwapWeapon";
	}
	return nullptr;
}