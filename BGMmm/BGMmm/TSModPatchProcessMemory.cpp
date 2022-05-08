
#include "TSModCommand.h"
#include "TSModPatchProcessMemory.h"

#include "lua/lua_interface.h"

#include <Windows.h>

#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;


/*----------------------------------------------------------------*
 IOProcessMemory系宣言
 *----------------------------------------------------------------*/



// cmdのテーブルの値を取得
int ImportCmdTable(lua_State* L) {
	lua_getglobal(L, "cmd");

	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		lua_pushvalue(L, -2);

		string key = lua_tostring(L, -1);
		int value = lua_tointeger(L, -2);

		// 各種変数に設定する。
		TSModCommand::setCommandSwitch((char *)key.c_str(), value);

		lua_pop(L, 2);
	}

	// スタックを削除
	lua_pop(L, 1);

	return 1;
}


/*----------------------------------------------------------------*
 LuaからTSMod.iniを読み込む
 *----------------------------------------------------------------*/
BOOL isIniFirstExist = FALSE;
void ImportTSModIni(char *szFileName) { 

	if (!isIniFirstExist) {
		PushEmptyTable(L, "cmd");
		PushEmptyTable(L, "txt");
		PushEmptyTable(L, "asm");

		isIniFirstExist = TRUE;
	}

	if(luaL_loadfile(L,szFileName) || lua_pcall(L, 0, 0, 0)) {

		// エラー発生フラグを立てておく。他のDLLが利用するため。
		lua_State_Err = 1;

		// 文法エラーだった
		string errmsg = lua_tostring(L, -1);
		errmsg += "\n"; // これ改行付けておく
		lua_error_and_exit(L, errmsg.c_str());
		return;

	// 正常
	} else {
		// cmdテーブルのインポート
		ImportCmdTable(L);
	}
}


/*----------------------------------------------------------------*
 IOProcessMemory系処理
 *----------------------------------------------------------------*/
bool isDonePatchProcessMemory = false;

void TDModPatchProcessMemory() { 
	
	if ( isDonePatchProcessMemory == false ) {
		isDonePatchProcessMemory = true;
	}

	// 対象のプロセスハンドルのメモリに対し、パッチデータに基づいて書き換える

	struct stat  st;
	if (stat("TSMod.ini", &st) == 0) {
		try {
			ImportTSModIni("TSMod.ini");
		} catch (...) { }
	}
	if (stat("TSModEx.ini", &st) == 0) {
		try {
			ImportTSModIni("TSModEx.ini");
		} catch (...) { }
	}
}


