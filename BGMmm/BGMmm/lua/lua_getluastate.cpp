
#include <windows.h>

#include "lua_interface.h"

__declspec(dllexport) lua_State* luaL_getstate(void) {
	return L;
}

int lua_State_Err = 0; // lua�X�e�[�g���o�O�������ɖ����I�Ƀt���O�𗧂Ă�B
__declspec(dllexport) int luaL_getstate_err(void) {
	return lua_State_Err;
}
