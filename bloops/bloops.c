#include <string.h>
#include <unistd.h>
#include "bloopsaphone.h"
#include "lua.h"
#include "lauxlib.h"

static int l_bloops_new(lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);

  bloops *B = bloops_new();
  B->tempo = 120;

  lua_newuserdatauv(L, 0, 1);
  lua_pushlightuserdata(L, B);
  lua_setiuservalue(L, -2, 1);
  lua_insert(L, -2);
  lua_setmetatable(L, -2);

  return 1;
}

static int l_bloops_play(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  const char *notes = luaL_checkstring(L, 2);

  lua_getiuservalue(L, 1, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops instance");
  bloops *B = lua_touserdata(L, -1);

  bloopsaphone *P = bloops_square();
  bloopsatrack *track = bloops_track2(B, P, (char *)notes);
  bloops_sound_destroy(P);
  bloops_play(B);
  while (!bloops_is_done(B)) usleep(1000);
  bloops_track_destroy(track);
  //bloops_destroy(B);
  return 0;
}

static int l_bloops_meta_index(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  lua_getiuservalue(L, 1, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops instance");
  bloops *B = lua_touserdata(L, -1);

  if (lua_isstring(L, 2)) {
    lua_pushvalue(L, 2);
    const char *key = lua_tostring(L, -1);
    if (!strcmp(key, "tempo")) {
      lua_pushinteger(L, B->tempo);      
      return 1;
    }
  }

  if (!lua_getmetatable(L, 1)) {
    return luaL_argerror(L, 1, "not a valid Bloops instance");
  }

  lua_pushvalue(L, 2);
  lua_rawget(L, -2);
  return 1;
}

static const luaL_Reg bloops_methods[] = {
  {"new", l_bloops_new},
  {"play", l_bloops_play},
  {NULL, NULL}
};

extern int luaopen_bloops(lua_State *L) {
  luaL_newlib(L, bloops_methods);
  lua_pushliteral(L, "__index");
  lua_pushcfunction(L, l_bloops_meta_index);
  lua_rawset(L, -3);
  lua_setglobal(L, "Bloops");

  return 0;
}
