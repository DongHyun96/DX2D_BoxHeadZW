#pragma once

#include <vector>
#include <string>

enum SCRIPT_TYPE
{
	BACKGROUNDSCRIPT,
	BULLETMGRSCRIPT,
	BULLETSCRIPT,
	CAMMOVESCRIPT,
	CHARACTERSCRIPT,
	ENEMYANIMHANDLER,
	ENEMYSCRIPT,
	PLAYERANIMHANDLER,
	PLAYERSCRIPT,
	PLAYERWEAPONHANDLER,
	EQUIPMENTSCRIPT,
	INVENSCRIPT,
	WEAPONPISTOL,
	WEAPONSCRIPT,
	MONSTERSCRIPT,
	OBSTACLE,
	STATSCRIPT,
	PLAYERSTAT,
	END,
};

using namespace std;
class CScript;

class ScriptMgr
{
public:
	static void GetScriptInfo(vector<wstring>& _vec);
	static CScript * GetScript(const wstring& _strScriptName);
	static CScript * GetScript(UINT _iScriptType);
	static const wchar_t * GetScriptName(CScript * _pScript);
};
