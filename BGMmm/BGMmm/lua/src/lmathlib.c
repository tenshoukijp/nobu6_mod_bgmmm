/*
** $Id: lmathlib.c,v 1.67.1.1 2007/12/27 13:02:25 roberto Exp $
** Standard mathematical library
** See Copyright Notice in lua.h
*/
#define _CRT_SECURE_NO_WARNINGS


#include <stdlib.h>
#include <math.h>

#define lmathlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


#undef PI
#define PI (3.14159265358979323846)
#define RADIANS_PER_DEGREE (PI/180.0)



static int math_abs (lua_State *L) {
	lua_pushnumber(L, fabs(luaL_checknumber(L, 1)));
	return 1;
}

static int math_sin (lua_State *L) {
	lua_pushnumber(L, sin(luaL_checknumber(L, 1)));
	return 1;
}

static int math_sinh (lua_State *L) {
	lua_pushnumber(L, sinh(luaL_checknumber(L, 1)));
	return 1;
}

static int math_cos (lua_State *L) {
	lua_pushnumber(L, cos(luaL_checknumber(L, 1)));
	return 1;
}

static int math_cosh (lua_State *L) {
	lua_pushnumber(L, cosh(luaL_checknumber(L, 1)));
	return 1;
}

static int math_tan (lua_State *L) {
	lua_pushnumber(L, tan(luaL_checknumber(L, 1)));
	return 1;
}

static int math_tanh (lua_State *L) {
	lua_pushnumber(L, tanh(luaL_checknumber(L, 1)));
	return 1;
}

static int math_asin (lua_State *L) {
	lua_pushnumber(L, asin(luaL_checknumber(L, 1)));
	return 1;
}

static int math_acos (lua_State *L) {
	lua_pushnumber(L, acos(luaL_checknumber(L, 1)));
	return 1;
}

static int math_atan (lua_State *L) {
	lua_pushnumber(L, atan(luaL_checknumber(L, 1)));
	return 1;
}

static int math_atan2 (lua_State *L) {
	lua_pushnumber(L, atan2(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
	return 1;
}

static int math_ceil (lua_State *L) {
	lua_pushnumber(L, ceil(luaL_checknumber(L, 1)));
	return 1;
}

static int math_floor (lua_State *L) {
	lua_pushnumber(L, floor(luaL_checknumber(L, 1)));
	return 1;
}


/**
 * sprintf関数を利用して, 任意の小数点以下を四捨五入する
 * @param[in] x 四捨五入を行う浮動小数点数
 * @param[in] precision 小数点以下の桁数
 * @return 四捨五入の結果
 */
static double _math_round(double x, int precision) {
	char s[256] = {0};

    sprintf(s, "%.*f", precision - 1, x);
    return strtod(s, NULL);
}

static int math_round (lua_State *L) {
	lua_Number x = luaL_checknumber(L, 1);
	int precision = luaL_optint(L, 2, 1);  // 2 番目の引数が数値であれば、 それを int にキャストした数値を返します。 その引数が存在しないか nil であれば、 1 を返します。 いずれでもなければエラーを発生します。 

	lua_Number d = _math_round(x, precision);

	lua_pushnumber(L, d);
	return 1;
}


static int math_fmod (lua_State *L) {
	lua_pushnumber(L, fmod(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
	return 1;
}

static int math_modf (lua_State *L) {
	double ip;
	double fp = modf(luaL_checknumber(L, 1), &ip);
	lua_pushnumber(L, ip);
	lua_pushnumber(L, fp);
	return 2;
}

static int math_sqrt (lua_State *L) {
	lua_pushnumber(L, sqrt(luaL_checknumber(L, 1)));
	return 1;
}

static int math_pow (lua_State *L) {
	lua_pushnumber(L, pow(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
	return 1;
}

static int math_log (lua_State *L) {
	lua_pushnumber(L, log(luaL_checknumber(L, 1)));
	return 1;
}

static int math_log10 (lua_State *L) {
	lua_pushnumber(L, log10(luaL_checknumber(L, 1)));
	return 1;
}

static int math_exp (lua_State *L) {
	lua_pushnumber(L, exp(luaL_checknumber(L, 1)));
	return 1;
}

static int math_deg (lua_State *L) {
	lua_pushnumber(L, luaL_checknumber(L, 1)/RADIANS_PER_DEGREE);
	return 1;
}

static int math_rad (lua_State *L) {
	lua_pushnumber(L, luaL_checknumber(L, 1)*RADIANS_PER_DEGREE);
	return 1;
}

static int math_frexp (lua_State *L) {
	int e;
	lua_pushnumber(L, frexp(luaL_checknumber(L, 1), &e));
	lua_pushinteger(L, e);
	return 2;
}

static int math_ldexp (lua_State *L) {
	lua_pushnumber(L, ldexp(luaL_checknumber(L, 1), luaL_checkint(L, 2)));
	return 1;
}



static int math_min (lua_State *L) {
	int n = lua_gettop(L);  /* number of arguments */
	lua_Number dmin = luaL_checknumber(L, 1);
	int i;
	for (i=2; i<=n; i++) {
		lua_Number d = luaL_checknumber(L, i);
		if (d < dmin)
			dmin = d;
	}
	lua_pushnumber(L, dmin);
	return 1;
}


static int math_max (lua_State *L) {
	int n = lua_gettop(L);  /* number of arguments */
	lua_Number dmax = luaL_checknumber(L, 1);
	int i;
	for (i=2; i<=n; i++) {
		lua_Number d = luaL_checknumber(L, i);
		if (d > dmax)
			dmax = d;
	}
	lua_pushnumber(L, dmax);
	return 1;
}


static int math_random (lua_State *L) {
	/* the `%' avoids the (rare) case of r==1, and is needed also because on
	some systems (SunOS!) `rand()' may return a value larger than RAND_MAX */
	lua_Number r = (lua_Number)(rand()%RAND_MAX) / (lua_Number)RAND_MAX;
	switch (lua_gettop(L)) {  /* check number of arguments */
	case 0: {  /* no arguments */
		lua_pushnumber(L, r);  /* Number between 0 and 1 */
		break;
			}
	case 1: {  /* only upper limit */
		int u = luaL_checkint(L, 1);
		luaL_argcheck(L, 1<=u, 1, "interval is empty");
		lua_pushnumber(L, floor(r*u)+1);  /* int between 1 and `u' */
		break;
			}
	case 2: {  /* lower and upper limits */
		int l = luaL_checkint(L, 1);
		int u = luaL_checkint(L, 2);
		luaL_argcheck(L, l<=u, 2, "interval is empty");
		lua_pushnumber(L, floor(r*(u-l+1))+l);  /* int between `l' and `u' */
		break;
			}
	default: return luaL_error(L, "wrong number of arguments");
	}
	return 1;
}


static int math_randomseed (lua_State *L) {
	srand(luaL_checkint(L, 1));
    (void)rand(); /* discard first value to avoid undesirable correlations */
	return 0;
}

static int math_type (lua_State *L) {
	if (lua_type(L, 1) == LUA_TNUMBER) {
		double d = luaL_checknumber(L, 1);
		if ( d == floor(d) ) {
			lua_pushliteral(L, "integer");
		}
		else {
			lua_pushliteral(L, "float"); 
		}
	}
	else {
		luaL_checkany(L, 1);
		lua_pushnil(L);
	}
	return 1;
}


static int math_tointeger (lua_State *L) {
	if (lua_type(L, 1) == LUA_TNUMBER) {
		double d = luaL_checknumber(L, 1);
		double fd = floor(d);
		if ( d == fd && INT_MIN <= d && d <= INT_MAX ) {
			lua_pushnumber(L, fd);
		}
		else {
			lua_pushnil(L);
		}
	}
	else {
		luaL_checkany(L, 1);
		lua_pushnil(L);
	}
	return 1;
}


static int math_ult (lua_State *L) {
  lua_Integer a = luaL_checkinteger(L, 1);
  lua_Integer b = luaL_checkinteger(L, 2);
  lua_pushboolean(L, (unsigned long long)a < (unsigned long long)b);
  return 1;
}




static const luaL_Reg mathlib[] = {
	{"abs",   math_abs},
	{"acos",  math_acos},
	{"asin",  math_asin},
	{"atan2", math_atan2},
	{"atan",  math_atan},
	{"ceil",  math_ceil},
	{"cosh",   math_cosh},
	{"cos",   math_cos},
	{"deg",   math_deg},
	{"exp",   math_exp},
	{"floor", math_floor},
	{"fmod",   math_fmod},
	{"frexp", math_frexp},
	{"ldexp", math_ldexp},
	{"log10", math_log10},
    {"ult",   math_ult},
	{"log",   math_log},
	{"max",   math_max},
	{"min",   math_min},
	{"modf",   math_modf},
	{"pow",   math_pow},
	{"rad",   math_rad},
	{"round",  math_round},
	{"random",     math_random},
	{"randomseed", math_randomseed},
	{"sinh",   math_sinh},
	{"sin",   math_sin},
	{"sqrt",  math_sqrt},
	{"tanh",   math_tanh},
	{"tan",   math_tan},
	{"type",   math_type},
	{"tointeger", math_tointeger},
	{NULL, NULL}
};


/*
** Open math library
*/
LUALIB_API int luaopen_math (lua_State *L) {
	luaL_register(L, LUA_MATHLIBNAME, mathlib);
	lua_pushnumber(L, PI);
	lua_setfield(L, -2, "pi");
	lua_pushnumber(L, HUGE_VAL);
	lua_setfield(L, -2, "huge");
	lua_pushinteger(L, INT_MAX);
	lua_setfield(L, -2, "maxinteger");
	lua_pushinteger(L, INT_MIN);
	lua_setfield(L, -2, "mininteger");
#if defined(LUA_COMPAT_MOD)
	lua_getfield(L, -1, "fmod");
	lua_setfield(L, -2, "mod");
#endif
	return 1;
}

