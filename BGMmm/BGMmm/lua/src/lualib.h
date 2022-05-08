/*
** $Id: lualib.h,v 1.36.1.1 2007/12/27 13:02:25 roberto Exp $
** Lua standard libraries
** See Copyright Notice in lua.h
*/


#ifndef lualib_h
#define lualib_h

#include "lua.h"


/* Key to file-handle type */
#define LUA_FILEHANDLE		"FILE*"


#define LUA_COLIBNAME	"coroutine"
LUALIB_API int (luaopen_base) (lua_State *L);

#define LUA_TABLIBNAME	"table"
LUALIB_API int (luaopen_table) (lua_State *L);

#define LUA_IOLIBNAME	"io"
LUALIB_API int (luaopen_io) (lua_State *L);

#define LUA_OSLIBNAME	"os"
LUALIB_API int (luaopen_os) (lua_State *L);

#define LUA_STRLIBNAME	"string"
LUALIB_API int (luaopen_string) (lua_State *L);

#define LUA_MATHLIBNAME	"math"
LUALIB_API int (luaopen_math) (lua_State *L);

#define LUA_BITLIBNAME	"bit32"
LUALIB_API int (luaopen_bit32) (lua_State *L);

#define LUA_DBLIBNAME	"debug"
LUALIB_API int (luaopen_debug) (lua_State *L);

#define LUA_LOADLIBNAME	"package"
LUALIB_API int (luaopen_package) (lua_State *L);

#define LUA_LFSLIBNAME	"lfs"
LUALIB_API int (luaopen_lfs)(lua_State *L);

#define LUA_CP932LIBNAME	"cp932"
LUALIB_API int (luaopen_cp932) (lua_State *L);

#define LUA_UTF8LIBNAME	"utf8"
LUALIB_API int (luaopen_utf8) (lua_State *L);

#define LUA_CJSONLIBNAME	"cjson"
LUALIB_API int (luaopen_cjson)(lua_State *L);

#define LUA_XMLLIBNAME	"xml"
LUALIB_API int (luaopen_xml)(lua_State *L);

#define LUA_LUACOMLIBNAME	"luacom"
LUALIB_API int (luaopen_luacom) (lua_State *L);

#define LUA_FFILIBNAME	"ffi"
LUALIB_API int (luaopen_ffi) (lua_State *L);

#ifdef LUA_CONSOLE_INTERPRETER
#define LUA_CLSLIBNAME	"cls"
LUALIB_API int (luaopen_cls) (lua_State *L);
#endif



/* open all previous libraries */
LUALIB_API void (luaL_openlibs) (lua_State *L); 



#ifndef lua_assert
#define lua_assert(x)	((void)0)
#endif


#endif
