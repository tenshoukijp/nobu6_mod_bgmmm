
#include <windows.h>

#include "lua_interface.h"

__declspec(dllexport) lua_State* luaL_getstate(void) {
	return L;
}

int lua_State_Err = 0; // luaステートがバグった時に明示的にフラグを立てる。
__declspec(dllexport) int luaL_getstate_err(void) {
	return lua_State_Err;
}
