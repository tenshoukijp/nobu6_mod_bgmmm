/*
** $Id: linit.c,v 1.14.1.1 2007/12/27 13:02:25 roberto Exp $
** Initialization of libraries for lua.c
** See Copyright Notice in lua.h
*/


#define linit_c
#define LUA_LIB

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"


static const luaL_Reg lualibs[] = {
	{"", luaopen_base},
	{LUA_LOADLIBNAME, luaopen_package},
	{LUA_TABLIBNAME, luaopen_table},
	{LUA_IOLIBNAME, luaopen_io},
	{LUA_OSLIBNAME, luaopen_os},
	{LUA_STRLIBNAME, luaopen_string},
	{LUA_MATHLIBNAME, luaopen_math},
	{LUA_BITLIBNAME, luaopen_bit32},
	{LUA_DBLIBNAME, luaopen_debug},
	{LUA_LFSLIBNAME, luaopen_lfs},
	{LUA_CP932LIBNAME, luaopen_cp932},
	{LUA_UTF8LIBNAME, luaopen_utf8},
//	{LUA_LUACOMLIBNAME, luaopen_luacom},
#ifdef HIDEMARU_HJLUA
//	{LUA_FFILIBNAME, luaopen_ffi},
	{LUA_CJSONLIBNAME, luaopen_cjson },
//	{LUA_XMLLIBNAME, luaopen_xml },
#endif
#ifdef LUA_CONSOLE_INTERPRETER
	{LUA_CLSLIBNAME, luaopen_cls},
#endif
	{NULL, NULL}
};


LUALIB_API void luaL_openlibs (lua_State *L) {
	const luaL_Reg *lib = lualibs;
	for (; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}
}

