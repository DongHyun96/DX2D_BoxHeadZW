#pragma once

#include <vector>
#include <string>

enum SCRIPT_TYPE
{
	BACKGROUNDSCRIPT,
	BULLETMGRSCRIPT,
	BULLETSCRIPT,
	CAMMOVESCRIPT,
	MONSTERSCRIPT,
	OBSTACLE,
	PLAYERANIMHANDLER,
	PLAYERSCRIPT,
	PLAYERSWAPWEAPON,
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
