
#include "TSModCommand.h"
#include "TSModPatchProcessMemory.h"

#include "lua/lua_interface.h"

#include <Windows.h>

#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;


/*----------------------------------------------------------------*
 IOProcessMemory�n�錾
 *----------------------------------------------------------------*/



// cmd�̃e�[�u���̒l���擾
int ImportCmdTable(lua_State* L) {
	lua_getglobal(L, "cmd");

	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		lua_pushvalue(L, -2);

		string key = lua_tostring(L, -1);
		int value = lua_tointeger(L, -2);

		// �e��ϐ��ɐݒ肷��B
		TSModCommand::setCommandSwitch((char *)key.c_str(), value);

		lua_pop(L, 2);
	}

	// �X�^�b�N���폜
	lua_pop(L, 1);

	return 1;
}


/*----------------------------------------------------------------*
 Lua����TSMod.ini��ǂݍ���
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

		// �G���[�����t���O�𗧂ĂĂ����B����DLL�����p���邽�߁B
		lua_State_Err = 1;

		// ���@�G���[������
		string errmsg = lua_tostring(L, -1);
		errmsg += "\n"; // ������s�t���Ă���
		lua_error_and_exit(L, errmsg.c_str());
		return;

	// ����
	} else {
		// cmd�e�[�u���̃C���|�[�g
		ImportCmdTable(L);
	}
}


/*----------------------------------------------------------------*
 IOProcessMemory�n����
 *----------------------------------------------------------------*/
bool isDonePatchProcessMemory = false;

void TDModPatchProcessMemory() { 
	
	if ( isDonePatchProcessMemory == false ) {
		isDonePatchProcessMemory = true;
	}

	// �Ώۂ̃v���Z�X�n���h���̃������ɑ΂��A�p�b�`�f�[�^�Ɋ�Â��ď���������

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


