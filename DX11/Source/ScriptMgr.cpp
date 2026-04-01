#include "pch.h"
#include "ScriptMgr.h"

#include "Source/Scripts/AirplaneShootingScript\CBackgroundScript.h"
#include "Source/Scripts/AirplaneShootingScript\CBulletMgrScript.h"
#include "Source/Scripts/AirplaneShootingScript\CBulletScript.h"
#include "Source/Scripts/CCamMoveScript.h"
#include "Source/Scripts/CharacterScript\CCharacterAnimHandler.h"
#include "Source/Scripts/CharacterScript\CCharacterScript.h"
#include "Source/Scripts/CharacterScript\EnemyScript\CEnemyAnimHandler.h"
#include "Source/Scripts/CharacterScript\EnemyScript\CEnemyScript.h"
#include "Source/Scripts/CharacterScript\PlayerScript\CPlayerAnimHandler.h"
#include "Source/Scripts/CharacterScript\PlayerScript\CPlayerScript.h"
#include "Source/Scripts/CharacterScript\PlayerScript\CPlayerWeaponHandler.h"
#include "Source/Scripts/CharacterScript\PlayerScript\InvenScript\CEquipmentScript.h"
#include "Source/Scripts/CharacterScript\PlayerScript\InvenScript\CInvenScript.h"
#include "Source/Scripts/CharacterScript\PlayerScript\WeaponScript\CWeaponMinigun.h"
#include "Source/Scripts/CharacterScript\PlayerScript\WeaponScript\CWeaponPistol.h"
#include "Source/Scripts/CharacterScript\PlayerScript\WeaponScript\CWeaponRocket.h"
#include "Source/Scripts/CharacterScript\PlayerScript\WeaponScript\CWeaponScript.h"
#include "Source/Scripts/CharacterScript\PlayerScript\WeaponScript\CWeaponShotgun.h"
#include "Source/Scripts/CharacterScript\PlayerScript\WeaponScript\CWeaponUzi.h"
#include "Source/Scripts/CMonsterScript.h"
#include "Source/Scripts/EffectScript\CFlipbookEffectScript.h"
#include "Source/Scripts/EffectScript\MuzzleEffects\CMuzzleFlashScript.h"
#include "Source/Scripts/EffectScript\MuzzleEffects\CMuzzleSmokeScript.h"
#include "Source/Scripts/Obstacle\CObstacle.h"
#include "Source/Scripts/ProjectileScript\CRocketProjectile.h"
#include "Source/Scripts/StatScript\CStatScript.h"
#include "Source/Scripts/StatScript\EnemyStat\CEnemyStat.h"
#include "Source/Scripts/StatScript\PlayerStat\CPlayerStat.h"

void ScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CBackgroundScript");
	_vec.push_back(L"CBulletMgrScript");
	_vec.push_back(L"CBulletScript");
	_vec.push_back(L"CCamMoveScript");
	_vec.push_back(L"CEnemyAnimHandler");
	_vec.push_back(L"CEnemyScript");
	_vec.push_back(L"CPlayerAnimHandler");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CPlayerWeaponHandler");
	_vec.push_back(L"CEquipmentScript");
	_vec.push_back(L"CInvenScript");
	_vec.push_back(L"CWeaponMinigun");
	_vec.push_back(L"CWeaponPistol");
	_vec.push_back(L"CWeaponRocket");
	_vec.push_back(L"CWeaponShotgun");
	_vec.push_back(L"CWeaponUzi");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"CFlipbookEffectScript");
	_vec.push_back(L"CMuzzleFlashScript");
	_vec.push_back(L"CMuzzleSmokeScript");
	_vec.push_back(L"CObstacle");
	_vec.push_back(L"CRocketProjectile");
	_vec.push_back(L"CStatScript");
	_vec.push_back(L"CEnemyStat");
	_vec.push_back(L"CPlayerStat");
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
	if (L"CCharacterAnimHandler" == _strScriptName)
		return nullptr;
	if (L"CCharacterScript" == _strScriptName)
		return nullptr;
	if (L"CEnemyAnimHandler" == _strScriptName)
		return new CEnemyAnimHandler;
	if (L"CEnemyScript" == _strScriptName)
		return new CEnemyScript;
	if (L"CPlayerAnimHandler" == _strScriptName)
		return new CPlayerAnimHandler;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CPlayerWeaponHandler" == _strScriptName)
		return new CPlayerWeaponHandler;
	if (L"CEquipmentScript" == _strScriptName)
		return new CEquipmentScript;
	if (L"CInvenScript" == _strScriptName)
		return new CInvenScript;
	if (L"CWeaponMinigun" == _strScriptName)
		return new CWeaponMinigun;
	if (L"CWeaponPistol" == _strScriptName)
		return new CWeaponPistol;
	if (L"CWeaponRocket" == _strScriptName)
		return new CWeaponRocket;
	if (L"CWeaponScript" == _strScriptName)
		return nullptr;
	if (L"CWeaponShotgun" == _strScriptName)
		return new CWeaponShotgun;
	if (L"CWeaponUzi" == _strScriptName)
		return new CWeaponUzi;
	if (L"CMonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"CFlipbookEffectScript" == _strScriptName)
		return new CFlipbookEffectScript;
	if (L"CMuzzleFlashScript" == _strScriptName)
		return new CMuzzleFlashScript;
	if (L"CMuzzleSmokeScript" == _strScriptName)
		return new CMuzzleSmokeScript;
	if (L"CObstacle" == _strScriptName)
		return new CObstacle;
	if (L"CRocketProjectile" == _strScriptName)
		return new CRocketProjectile;
	if (L"CStatScript" == _strScriptName)
		return new CStatScript;
	if (L"CEnemyStat" == _strScriptName)
		return new CEnemyStat;
	if (L"CPlayerStat" == _strScriptName)
		return new CPlayerStat;
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
	case (UINT)SCRIPT_TYPE::CHARACTERANIMHANDLER:
		return nullptr;
	case (UINT)SCRIPT_TYPE::CHARACTERSCRIPT:
		return nullptr;
	case (UINT)SCRIPT_TYPE::ENEMYANIMHANDLER:
		return new CEnemyAnimHandler;
	case (UINT)SCRIPT_TYPE::ENEMYSCRIPT:
		return new CEnemyScript;
	case (UINT)SCRIPT_TYPE::PLAYERANIMHANDLER:
		return new CPlayerAnimHandler;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
	case (UINT)SCRIPT_TYPE::PLAYERWEAPONHANDLER:
		return new CPlayerWeaponHandler;
	case (UINT)SCRIPT_TYPE::EQUIPMENTSCRIPT:
		return new CEquipmentScript;
	case (UINT)SCRIPT_TYPE::INVENSCRIPT:
		return new CInvenScript;
	case (UINT)SCRIPT_TYPE::WEAPONMINIGUN:
		return new CWeaponMinigun;
	case (UINT)SCRIPT_TYPE::WEAPONPISTOL:
		return new CWeaponPistol;
	case (UINT)SCRIPT_TYPE::WEAPONROCKET:
		return new CWeaponRocket;
	case (UINT)SCRIPT_TYPE::WEAPONSCRIPT:
		return nullptr;
	case (UINT)SCRIPT_TYPE::WEAPONSHOTGUN:
		return new CWeaponShotgun;
	case (UINT)SCRIPT_TYPE::WEAPONUZI:
		return new CWeaponUzi;
	case (UINT)SCRIPT_TYPE::MONSTERSCRIPT:
		return new CMonsterScript;
	case (UINT)SCRIPT_TYPE::FLIPBOOKEFFECTSCRIPT:
		return new CFlipbookEffectScript;
	case (UINT)SCRIPT_TYPE::MUZZLEFLASHSCRIPT:
		return new CMuzzleFlashScript;
	case (UINT)SCRIPT_TYPE::MUZZLESMOKESCRIPT:
		return new CMuzzleSmokeScript;
	case (UINT)SCRIPT_TYPE::OBSTACLE:
		return new CObstacle;
	case (UINT)SCRIPT_TYPE::ROCKETPROJECTILE:
		return new CRocketProjectile;
	case (UINT)SCRIPT_TYPE::STATSCRIPT:
		return new CStatScript;
	case (UINT)SCRIPT_TYPE::ENEMYSTAT:
		return new CEnemyStat;
	case (UINT)SCRIPT_TYPE::PLAYERSTAT:
		return new CPlayerStat;
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
	case SCRIPT_TYPE::CHARACTERANIMHANDLER:
		return L"CCharacterAnimHandler";
	case SCRIPT_TYPE::CHARACTERSCRIPT:
		return L"CCharacterScript";
	case SCRIPT_TYPE::ENEMYANIMHANDLER:
		return L"CEnemyAnimHandler";
	case SCRIPT_TYPE::ENEMYSCRIPT:
		return L"CEnemyScript";
	case SCRIPT_TYPE::PLAYERANIMHANDLER:
		return L"CPlayerAnimHandler";
	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
	case SCRIPT_TYPE::PLAYERWEAPONHANDLER:
		return L"CPlayerWeaponHandler";
	case SCRIPT_TYPE::EQUIPMENTSCRIPT:
		return L"CEquipmentScript";
	case SCRIPT_TYPE::INVENSCRIPT:
		return L"CInvenScript";
	case SCRIPT_TYPE::WEAPONMINIGUN:
		return L"CWeaponMinigun";
	case SCRIPT_TYPE::WEAPONPISTOL:
		return L"CWeaponPistol";
	case SCRIPT_TYPE::WEAPONROCKET:
		return L"CWeaponRocket";
	case SCRIPT_TYPE::WEAPONSCRIPT:
		return L"CWeaponScript";
	case SCRIPT_TYPE::WEAPONSHOTGUN:
		return L"CWeaponShotgun";
	case SCRIPT_TYPE::WEAPONUZI:
		return L"CWeaponUzi";
	case SCRIPT_TYPE::MONSTERSCRIPT:
		return L"CMonsterScript";
	case SCRIPT_TYPE::FLIPBOOKEFFECTSCRIPT:
		return L"CFlipbookEffectScript";
	case SCRIPT_TYPE::MUZZLEFLASHSCRIPT:
		return L"CMuzzleFlashScript";
	case SCRIPT_TYPE::MUZZLESMOKESCRIPT:
		return L"CMuzzleSmokeScript";
	case SCRIPT_TYPE::OBSTACLE:
		return L"CObstacle";
	case SCRIPT_TYPE::ROCKETPROJECTILE:
		return L"CRocketProjectile";
	case SCRIPT_TYPE::STATSCRIPT:
		return L"CStatScript";
	case SCRIPT_TYPE::ENEMYSTAT:
		return L"CEnemyStat";
	case SCRIPT_TYPE::PLAYERSTAT:
		return L"CPlayerStat";
	}
	return nullptr;
}