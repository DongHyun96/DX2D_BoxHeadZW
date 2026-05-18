#include "pch.h"
#include "ScriptMgr.h"

#include "Source/Scripts/AirplaneShootingScript\CBackgroundScript.h"
#include "Source/Scripts/AirplaneShootingScript\CBulletMgrScript.h"
#include "Source/Scripts/AirplaneShootingScript\CBulletScript.h"
#include "Source/Scripts/AirStrike\CAirStrike.h"
#include "Source/Scripts/BackgroundTile\CBackgroundTile.h"
#include "Source/Scripts/CCamMoveScript.h"
#include "Source/Scripts/CCamPerspectiveMove.h"
#include "Source/Scripts/CharacterScript\AnimHandler\CCharacterAnimHandler.h"
#include "Source/Scripts/CharacterScript\AnimHandler\EnemyAnimHandler\CEnemyAnimHandler.h"
#include "Source/Scripts/CharacterScript\AnimHandler\PlayerAnimHandler\CPlayerAnimHandler.h"
#include "Source/Scripts/CharacterScript\CCharacterScript.h"
#include "Source/Scripts/CharacterScript\CharacterStat\CCharacterStat.h"
#include "Source/Scripts/CharacterScript\CharacterStat\EnemyStat\CEnemyStat.h"
#include "Source/Scripts/CharacterScript\CharacterStat\PlayerStat\CPlayerStat.h"
#include "Source/Scripts/CharacterScript\EnemyScript\CEnemyScript.h"
#include "Source/Scripts/CharacterScript\EnemyScript\Devil\CDevil.h"
#include "Source/Scripts/CharacterScript\EnemyScript\Devil\CFlameLineHandler.h"
#include "Source/Scripts/CharacterScript\EnemyScript\EnemySpawnHandler\CEnemySpawnHandler.h"
#include "Source/Scripts/CharacterScript\EnemyScript\Mummy\CMummy.h"
#include "Source/Scripts/CharacterScript\EnemyScript\PerceptionHandler\CPerceptionHandler.h"
#include "Source/Scripts/CharacterScript\EnemyScript\Runner\CRunner.h"
#include "Source/Scripts/CharacterScript\EnemyScript\Vampire\CVampire.h"
#include "Source/Scripts/CharacterScript\PlayerScript\CPlayerScript.h"
#include "Source/Scripts/CharacterScript\PlayerScript\CPlayerWeaponHandler.h"
#include "Source/Scripts/CharacterScript\PlayerScript\InvenScript\CEquipmentScript.h"
#include "Source/Scripts/CharacterScript\PlayerScript\InvenScript\CInvenScript.h"
#include "Source/Scripts/CharacterScript\PlayerScript\StructureHandler\CStructureHandler.h"
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
#include "Source/Scripts/ExplosionDome\CExplosion.h"
#include "Source/Scripts/ExplosionDome\CExplosionDome.h"
#include "Source/Scripts/ExplosionDome\CFirePillarHandler.h"
#include "Source/Scripts/FirstSpawnLocManager\CFirstSpawnLocManager.h"
#include "Source/Scripts/Item\CItem.h"
#include "Source/Scripts/Obstacle\CObstacle.h"
#include "Source/Scripts/ProjectileScript\CGrenade.h"
#include "Source/Scripts/ProjectileScript\CRocketProjectile.h"
#include "Source/Scripts/RoundHandler\CRoundHandler.h"
#include "Source/Scripts/StatScript\CStatScript.h"
#include "Source/Scripts/Structure\CBarrel.h"
#include "Source/Scripts/Structure\CStructure.h"
#include "Source/Scripts/Structure\CTurret.h"
#include "Source/Scripts/Structure\StructureStat\CStructureStat.h"
#include "Source/Scripts/Temp\CAirStrikePreview.h"
#include "Source/Scripts/UIScript\CButton.h"
#include "Source/Scripts/UIScript\CCrossHair.h"
#include "Source/Scripts/UIScript\CGameUI.h"
#include "Source/Scripts/UIScript\CProgressBar.h"
#include "Source/Scripts/UIScript\CText.h"
#include "Source/Scripts/UIScript\InGameUIManager\CIngameUIManager.h"
#include "Source/Scripts/UIScript\UIAnimation\CUIAnimation.h"
#include "Source/Scripts/UIScript\UIAnimation\CUIAnimationGroup.h"

void ScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CBackgroundScript");
	_vec.push_back(L"CBulletMgrScript");
	_vec.push_back(L"CBulletScript");
	_vec.push_back(L"CAirStrike");
	_vec.push_back(L"CBackgroundTile");
	_vec.push_back(L"CCamMoveScript");
	_vec.push_back(L"CCamPerspectiveMove");
	_vec.push_back(L"CEnemyAnimHandler");
	_vec.push_back(L"CPlayerAnimHandler");
	_vec.push_back(L"CCharacterStat");
	_vec.push_back(L"CEnemyStat");
	_vec.push_back(L"CPlayerStat");
	_vec.push_back(L"CEnemyScript");
	_vec.push_back(L"CDevil");
	_vec.push_back(L"CFlameLineHandler");
	_vec.push_back(L"CEnemySpawnHandler");
	_vec.push_back(L"CMummy");
	_vec.push_back(L"CPerceptionHandler");
	_vec.push_back(L"CRunner");
	_vec.push_back(L"CVampire");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CPlayerWeaponHandler");
	_vec.push_back(L"CEquipmentScript");
	_vec.push_back(L"CInvenScript");
	_vec.push_back(L"CStructureHandler");
	_vec.push_back(L"CWeaponMinigun");
	_vec.push_back(L"CWeaponPistol");
	_vec.push_back(L"CWeaponRocket");
	_vec.push_back(L"CWeaponShotgun");
	_vec.push_back(L"CWeaponUzi");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"CFlipbookEffectScript");
	_vec.push_back(L"CMuzzleFlashScript");
	_vec.push_back(L"CMuzzleSmokeScript");
	_vec.push_back(L"CExplosion");
	_vec.push_back(L"CExplosionDome");
	_vec.push_back(L"CFirePillarHandler");
	_vec.push_back(L"CFirstSpawnLocManager");
	_vec.push_back(L"CItem");
	_vec.push_back(L"CObstacle");
	_vec.push_back(L"CGrenade");
	_vec.push_back(L"CRocketProjectile");
	_vec.push_back(L"CRoundHandler");
	_vec.push_back(L"CStatScript");
	_vec.push_back(L"CBarrel");
	_vec.push_back(L"CStructure");
	_vec.push_back(L"CTurret");
	_vec.push_back(L"CStructureStat");
	_vec.push_back(L"CAirStrikePreview");
	_vec.push_back(L"CButton");
	_vec.push_back(L"CCrossHair");
	_vec.push_back(L"CProgressBar");
	_vec.push_back(L"CText");
	_vec.push_back(L"CIngameUIManager");
	_vec.push_back(L"CUIAnimation");
	_vec.push_back(L"CUIAnimationGroup");
}

CScript * ScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CBackgroundScript" == _strScriptName)
		return new CBackgroundScript;
	if (L"CBulletMgrScript" == _strScriptName)
		return new CBulletMgrScript;
	if (L"CBulletScript" == _strScriptName)
		return new CBulletScript;
	if (L"CAirStrike" == _strScriptName)
		return new CAirStrike;
	if (L"CBackgroundTile" == _strScriptName)
		return new CBackgroundTile;
	if (L"CCamMoveScript" == _strScriptName)
		return new CCamMoveScript;
	if (L"CCamPerspectiveMove" == _strScriptName)
		return new CCamPerspectiveMove;
	if (L"CCharacterAnimHandler" == _strScriptName)
		return nullptr;
	if (L"CEnemyAnimHandler" == _strScriptName)
		return new CEnemyAnimHandler;
	if (L"CPlayerAnimHandler" == _strScriptName)
		return new CPlayerAnimHandler;
	if (L"CCharacterScript" == _strScriptName)
		return nullptr;
	if (L"CCharacterStat" == _strScriptName)
		return new CCharacterStat;
	if (L"CEnemyStat" == _strScriptName)
		return new CEnemyStat;
	if (L"CPlayerStat" == _strScriptName)
		return new CPlayerStat;
	if (L"CEnemyScript" == _strScriptName)
		return new CEnemyScript;
	if (L"CDevil" == _strScriptName)
		return new CDevil;
	if (L"CFlameLineHandler" == _strScriptName)
		return new CFlameLineHandler;
	if (L"CEnemySpawnHandler" == _strScriptName)
		return new CEnemySpawnHandler;
	if (L"CMummy" == _strScriptName)
		return new CMummy;
	if (L"CPerceptionHandler" == _strScriptName)
		return new CPerceptionHandler;
	if (L"CRunner" == _strScriptName)
		return new CRunner;
	if (L"CVampire" == _strScriptName)
		return new CVampire;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CPlayerWeaponHandler" == _strScriptName)
		return new CPlayerWeaponHandler;
	if (L"CEquipmentScript" == _strScriptName)
		return new CEquipmentScript;
	if (L"CInvenScript" == _strScriptName)
		return new CInvenScript;
	if (L"CStructureHandler" == _strScriptName)
		return new CStructureHandler;
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
	if (L"CExplosion" == _strScriptName)
		return new CExplosion;
	if (L"CExplosionDome" == _strScriptName)
		return new CExplosionDome;
	if (L"CFirePillarHandler" == _strScriptName)
		return new CFirePillarHandler;
	if (L"CFirstSpawnLocManager" == _strScriptName)
		return new CFirstSpawnLocManager;
	if (L"CItem" == _strScriptName)
		return new CItem;
	if (L"CObstacle" == _strScriptName)
		return new CObstacle;
	if (L"CGrenade" == _strScriptName)
		return new CGrenade;
	if (L"CRocketProjectile" == _strScriptName)
		return new CRocketProjectile;
	if (L"CRoundHandler" == _strScriptName)
		return new CRoundHandler;
	if (L"CStatScript" == _strScriptName)
		return new CStatScript;
	if (L"CBarrel" == _strScriptName)
		return new CBarrel;
	if (L"CStructure" == _strScriptName)
		return new CStructure;
	if (L"CTurret" == _strScriptName)
		return new CTurret;
	if (L"CStructureStat" == _strScriptName)
		return new CStructureStat;
	if (L"CAirStrikePreview" == _strScriptName)
		return new CAirStrikePreview;
	if (L"CButton" == _strScriptName)
		return new CButton;
	if (L"CCrossHair" == _strScriptName)
		return new CCrossHair;
	if (L"CGameUI" == _strScriptName)
		return nullptr;
	if (L"CProgressBar" == _strScriptName)
		return new CProgressBar;
	if (L"CText" == _strScriptName)
		return new CText;
	if (L"CIngameUIManager" == _strScriptName)
		return new CIngameUIManager;
	if (L"CUIAnimation" == _strScriptName)
		return new CUIAnimation;
	if (L"CUIAnimationGroup" == _strScriptName)
		return new CUIAnimationGroup;
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
	case (UINT)SCRIPT_TYPE::AIRSTRIKE:
		return new CAirStrike;
	case (UINT)SCRIPT_TYPE::BACKGROUNDTILE:
		return new CBackgroundTile;
	case (UINT)SCRIPT_TYPE::CAMMOVESCRIPT:
		return new CCamMoveScript;
	case (UINT)SCRIPT_TYPE::CAMPERSPECTIVEMOVE:
		return new CCamPerspectiveMove;
	case (UINT)SCRIPT_TYPE::CHARACTERANIMHANDLER:
		return nullptr;
	case (UINT)SCRIPT_TYPE::ENEMYANIMHANDLER:
		return new CEnemyAnimHandler;
	case (UINT)SCRIPT_TYPE::PLAYERANIMHANDLER:
		return new CPlayerAnimHandler;
	case (UINT)SCRIPT_TYPE::CHARACTERSCRIPT:
		return nullptr;
	case (UINT)SCRIPT_TYPE::CHARACTERSTAT:
		return new CCharacterStat;
	case (UINT)SCRIPT_TYPE::ENEMYSTAT:
		return new CEnemyStat;
	case (UINT)SCRIPT_TYPE::PLAYERSTAT:
		return new CPlayerStat;
	case (UINT)SCRIPT_TYPE::ENEMYSCRIPT:
		return new CEnemyScript;
	case (UINT)SCRIPT_TYPE::DEVIL:
		return new CDevil;
	case (UINT)SCRIPT_TYPE::FLAMELINEHANDLER:
		return new CFlameLineHandler;
	case (UINT)SCRIPT_TYPE::ENEMYSPAWNHANDLER:
		return new CEnemySpawnHandler;
	case (UINT)SCRIPT_TYPE::MUMMY:
		return new CMummy;
	case (UINT)SCRIPT_TYPE::PERCEPTIONHANDLER:
		return new CPerceptionHandler;
	case (UINT)SCRIPT_TYPE::RUNNER:
		return new CRunner;
	case (UINT)SCRIPT_TYPE::VAMPIRE:
		return new CVampire;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
	case (UINT)SCRIPT_TYPE::PLAYERWEAPONHANDLER:
		return new CPlayerWeaponHandler;
	case (UINT)SCRIPT_TYPE::EQUIPMENTSCRIPT:
		return new CEquipmentScript;
	case (UINT)SCRIPT_TYPE::INVENSCRIPT:
		return new CInvenScript;
	case (UINT)SCRIPT_TYPE::STRUCTUREHANDLER:
		return new CStructureHandler;
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
	case (UINT)SCRIPT_TYPE::EXPLOSION:
		return new CExplosion;
	case (UINT)SCRIPT_TYPE::EXPLOSIONDOME:
		return new CExplosionDome;
	case (UINT)SCRIPT_TYPE::FIREPILLARHANDLER:
		return new CFirePillarHandler;
	case (UINT)SCRIPT_TYPE::FIRSTSPAWNLOCMANAGER:
		return new CFirstSpawnLocManager;
	case (UINT)SCRIPT_TYPE::ITEM:
		return new CItem;
	case (UINT)SCRIPT_TYPE::OBSTACLE:
		return new CObstacle;
	case (UINT)SCRIPT_TYPE::GRENADE:
		return new CGrenade;
	case (UINT)SCRIPT_TYPE::ROCKETPROJECTILE:
		return new CRocketProjectile;
	case (UINT)SCRIPT_TYPE::ROUNDHANDLER:
		return new CRoundHandler;
	case (UINT)SCRIPT_TYPE::STATSCRIPT:
		return new CStatScript;
	case (UINT)SCRIPT_TYPE::BARREL:
		return new CBarrel;
	case (UINT)SCRIPT_TYPE::STRUCTURE:
		return new CStructure;
	case (UINT)SCRIPT_TYPE::TURRET:
		return new CTurret;
	case (UINT)SCRIPT_TYPE::STRUCTURESTAT:
		return new CStructureStat;
	case (UINT)SCRIPT_TYPE::AIRSTRIKEPREVIEW:
		return new CAirStrikePreview;
	case (UINT)SCRIPT_TYPE::BUTTON:
		return new CButton;
	case (UINT)SCRIPT_TYPE::CROSSHAIR:
		return new CCrossHair;
	case (UINT)SCRIPT_TYPE::GAMEUI:
		return nullptr;
	case (UINT)SCRIPT_TYPE::PROGRESSBAR:
		return new CProgressBar;
	case (UINT)SCRIPT_TYPE::TEXT:
		return new CText;
	case (UINT)SCRIPT_TYPE::INGAMEUIMANAGER:
		return new CIngameUIManager;
	case (UINT)SCRIPT_TYPE::UIANIMATION:
		return new CUIAnimation;
	case (UINT)SCRIPT_TYPE::UIANIMATIONGROUP:
		return new CUIAnimationGroup;
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
	case SCRIPT_TYPE::AIRSTRIKE:
		return L"CAirStrike";
	case SCRIPT_TYPE::BACKGROUNDTILE:
		return L"CBackgroundTile";
	case SCRIPT_TYPE::CAMMOVESCRIPT:
		return L"CCamMoveScript";
	case SCRIPT_TYPE::CAMPERSPECTIVEMOVE:
		return L"CCamPerspectiveMove";
	case SCRIPT_TYPE::CHARACTERANIMHANDLER:
		return L"CCharacterAnimHandler";
	case SCRIPT_TYPE::ENEMYANIMHANDLER:
		return L"CEnemyAnimHandler";
	case SCRIPT_TYPE::PLAYERANIMHANDLER:
		return L"CPlayerAnimHandler";
	case SCRIPT_TYPE::CHARACTERSCRIPT:
		return L"CCharacterScript";
	case SCRIPT_TYPE::CHARACTERSTAT:
		return L"CCharacterStat";
	case SCRIPT_TYPE::ENEMYSTAT:
		return L"CEnemyStat";
	case SCRIPT_TYPE::PLAYERSTAT:
		return L"CPlayerStat";
	case SCRIPT_TYPE::ENEMYSCRIPT:
		return L"CEnemyScript";
	case SCRIPT_TYPE::DEVIL:
		return L"CDevil";
	case SCRIPT_TYPE::FLAMELINEHANDLER:
		return L"CFlameLineHandler";
	case SCRIPT_TYPE::ENEMYSPAWNHANDLER:
		return L"CEnemySpawnHandler";
	case SCRIPT_TYPE::MUMMY:
		return L"CMummy";
	case SCRIPT_TYPE::PERCEPTIONHANDLER:
		return L"CPerceptionHandler";
	case SCRIPT_TYPE::RUNNER:
		return L"CRunner";
	case SCRIPT_TYPE::VAMPIRE:
		return L"CVampire";
	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
	case SCRIPT_TYPE::PLAYERWEAPONHANDLER:
		return L"CPlayerWeaponHandler";
	case SCRIPT_TYPE::EQUIPMENTSCRIPT:
		return L"CEquipmentScript";
	case SCRIPT_TYPE::INVENSCRIPT:
		return L"CInvenScript";
	case SCRIPT_TYPE::STRUCTUREHANDLER:
		return L"CStructureHandler";
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
	case SCRIPT_TYPE::EXPLOSION:
		return L"CExplosion";
	case SCRIPT_TYPE::EXPLOSIONDOME:
		return L"CExplosionDome";
	case SCRIPT_TYPE::FIREPILLARHANDLER:
		return L"CFirePillarHandler";
	case SCRIPT_TYPE::FIRSTSPAWNLOCMANAGER:
		return L"CFirstSpawnLocManager";
	case SCRIPT_TYPE::ITEM:
		return L"CItem";
	case SCRIPT_TYPE::OBSTACLE:
		return L"CObstacle";
	case SCRIPT_TYPE::GRENADE:
		return L"CGrenade";
	case SCRIPT_TYPE::ROCKETPROJECTILE:
		return L"CRocketProjectile";
	case SCRIPT_TYPE::ROUNDHANDLER:
		return L"CRoundHandler";
	case SCRIPT_TYPE::STATSCRIPT:
		return L"CStatScript";
	case SCRIPT_TYPE::BARREL:
		return L"CBarrel";
	case SCRIPT_TYPE::STRUCTURE:
		return L"CStructure";
	case SCRIPT_TYPE::TURRET:
		return L"CTurret";
	case SCRIPT_TYPE::STRUCTURESTAT:
		return L"CStructureStat";
	case SCRIPT_TYPE::AIRSTRIKEPREVIEW:
		return L"CAirStrikePreview";
	case SCRIPT_TYPE::BUTTON:
		return L"CButton";
	case SCRIPT_TYPE::CROSSHAIR:
		return L"CCrossHair";
	case SCRIPT_TYPE::GAMEUI:
		return L"CGameUI";
	case SCRIPT_TYPE::PROGRESSBAR:
		return L"CProgressBar";
	case SCRIPT_TYPE::TEXT:
		return L"CText";
	case SCRIPT_TYPE::INGAMEUIMANAGER:
		return L"CIngameUIManager";
	case SCRIPT_TYPE::UIANIMATION:
		return L"CUIAnimation";
	case SCRIPT_TYPE::UIANIMATIONGROUP:
		return L"CUIAnimationGroup";
	}
	return nullptr;
}