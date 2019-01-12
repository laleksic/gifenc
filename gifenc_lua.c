// Lua binding
// 2018 Luka Aleksic <laleksic@mail.ru>

#ifdef _WIN32
#	define DLL_EXPORT __declspec(dllexport)
#else
#	define DLL_EXPORT __attribute__((visibility("default")))
#endif

#include "gifenc.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "compat-5.3.h"

static int new_gif (lua_State *L)
{
	if(lua_gettop(L) != 4) {
		return luaL_error(L, "");
	}
	
	const char *fname = luaL_checkstring(L, 1);
	uint16_t width = luaL_checkinteger(L, 2);
	uint16_t height = luaL_checkinteger(L, 3);
	int loop = luaL_checkinteger(L, 4);

	ge_GIF **gif = lua_newuserdata(L, sizeof(ge_GIF*));
	*gif = ge_new_gif(fname, width, height, NULL, 8, loop);
	
	luaL_getmetatable(L, "gifenc.gif");
	lua_setmetatable(L, -2);
	
	return 1;
}

static int add_frame (lua_State *L)
{
	if(lua_gettop(L) != 2) {
		return luaL_error(L, "");
	}
	
	ge_GIF **gif = luaL_checkudata(L, 1, "gifenc.gif");
	uint16_t delay = luaL_checkinteger(L, 2);
	ge_add_frame(*gif, delay);

	return 0;
}

static int close_gif (lua_State *L)
{
	ge_GIF **gif = lua_touserdata(L, 1);
	ge_close_gif(*gif);

	return 0;
}

static int plot_pixel (lua_State *L)
{
	if(lua_gettop(L) != 3) {
		return luaL_error(L, "");
	}
	
	ge_GIF **gif = luaL_checkudata(L, 1, "gifenc.gif");
	int i = luaL_checkinteger(L, 2);
	int color = luaL_checkinteger(L, 3);
	
	int w = (*gif)->w;
	int h = (*gif)->h;
	
	luaL_argcheck(L, i>=0 && i<w*h, 2, "");
	luaL_argcheck(L, color>=0 && color < 256, 3, "");
	
	(*gif)->frame[i] = color;
	return 0;
}

static const struct luaL_Reg gifenc [] = {
	{"new_gif", new_gif},
	
	{NULL, NULL}
};

static const struct luaL_Reg gif_methods [] = {
	{"__gc", close_gif},
	{"plot_pixel", plot_pixel},
	{"add_frame", add_frame},
	{NULL, NULL}
};

DLL_EXPORT int luaopen_gifenc (lua_State *L)
{
	luaL_newmetatable(L, "gifenc.gif");
	
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);
		
	luaL_setfuncs(L, gif_methods, 0);
	
	lua_newtable(L);
	luaL_setfuncs(L, gifenc, 0);

	return 1;
}
