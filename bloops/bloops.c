#include <stdlib.h>
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
  lua_getiuservalue(L, 1, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops instance");
  bloops *B = lua_touserdata(L, -1);

  bloops_play(B);
  //while (!bloops_is_done(B)) usleep(1000);
  return 0;
}

static int l_bloops_is_stopped(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  lua_getiuservalue(L, 1, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops instance");
  bloops *B = lua_touserdata(L, -1);

  lua_pushboolean(L, bloops_is_done(B));
  return 1;
}

static int l_bloops_sound(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  lua_Integer waveform = luaL_optinteger(L, 2, BLOOPS_SQUARE);

  bloopsaphone *P = bloops_square();
  P->params.type = waveform;

  lua_newuserdatauv(L, 0, 1);
  lua_pushlightuserdata(L, P);
  lua_setiuservalue(L, -2, 1);
  lua_getfield(L, 1, "Sound");
  lua_setmetatable(L, -2);

  return 1;
}

static int l_bloops_tune(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  luaL_checktype(L, 2, LUA_TUSERDATA);
  const char *notes = luaL_checkstring(L, 3);

  lua_getiuservalue(L, 1, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops instance");
  bloops *B = lua_touserdata(L, -1);

  lua_getiuservalue(L, 2, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops.Sound instance");
  bloopsaphone *phone = lua_touserdata(L, -1);

  bloopsatrack *track = bloops_track(B, phone, (char *)notes, strlen(notes));

  lua_newuserdatauv(L, 0, 1);
  lua_pushlightuserdata(L, track);
  lua_setiuservalue(L, -2, 1);
  lua_getfield(L, 1, "Track");
  lua_setmetatable(L, -2);

  return 1;
}

static int l_bloops_sleep(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  lua_Number seconds = luaL_checknumber(L, 2);

  usleep((lua_Integer)(seconds * 1000000));

  return 0;
}

static int l_bloops_meta_tostring(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  lua_getiuservalue(L, 1, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops instance");
  bloops *B = lua_touserdata(L, -1);

  lua_pushfstring(L, "Bloops: %p", B);
  return 1;
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

static int l_bloops_meta_newindex(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  lua_getiuservalue(L, 1, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops instance");
  bloops *B = lua_touserdata(L, -1);

  if (lua_isstring(L, 2)) {
    lua_pushvalue(L, 2);
    const char *key = lua_tostring(L, -1);
    if (!strcmp(key, "tempo")) {
      B->tempo = luaL_checkinteger(L, 3);
      return 0;
    }
  }

  if (!lua_getmetatable(L, 1)) {
    return luaL_argerror(L, 1, "not a valid Bloops instance");
  }

  lua_pushvalue(L, 2);
  lua_pushvalue(L, 3);
  lua_rawset(L, -3);
  return 0;
}

static int l_bloops_sound_test(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  lua_getiuservalue(L, 1, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops.Sound instance");
  bloopsaphone *P = lua_touserdata(L, -1);

  bloops *B = bloops_new();
  B->tempo = 120;
  bloopsatrack *T = bloops_track2(B, P, "C");
  T->notes[0].tone = 'n';
  memcpy(&T->params, &P->params, sizeof(bloopsaparams));
  bloops_play(B);
  bloops_track_destroy(T);
  bloops_destroy(B);

  return 0;
}

static int l_bloops_sound_meta_tostring(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  lua_getiuservalue(L, 1, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops.Sound instance");
  bloopsaphone *P = lua_touserdata(L, -1);

  char *str = bloops_sound_str(P);
  lua_pushfstring(L, "Bloops.Sound: %p\n%s", P, str);
  free(str);
  
  return 1;
}

static int l_bloops_track_meta_tostring(lua_State *L) {
  luaL_checktype(L, 1, LUA_TUSERDATA);
  lua_getiuservalue(L, 1, 1);
  luaL_argcheck(L, lua_islightuserdata(L, -1), 1, "not a valid Bloops.Track instance");
  bloopsatrack *track = lua_touserdata(L, -1);

  char *str = bloops_track_str(track);
  lua_pushfstring(L, "Bloops.Track: %p\n%s", track, str);
  free(str);

  return 1;
}

static const luaL_Reg bloops_methods[] = {
  {"new", l_bloops_new},
  {"play", l_bloops_play},
  {"is_stopped", l_bloops_is_stopped},
  {"sound", l_bloops_sound},
  {"tune", l_bloops_tune},
  {"sleep", l_bloops_sleep},
  {"__tostring", l_bloops_meta_tostring},
  {"__index", l_bloops_meta_index},
  {"__newindex", l_bloops_meta_newindex},
  {NULL, NULL}
};

static const luaL_Reg bloops_sound_methods[] = {
  {"test", l_bloops_sound_test},
  {"__tostring", l_bloops_sound_meta_tostring},
  {NULL, NULL}
};

static const luaL_Reg bloops_track_methods[] = {
  {"__tostring", l_bloops_track_meta_tostring},
  {NULL, NULL}
};

extern int luaopen_bloops(lua_State *L) {
  luaL_newlib(L, bloops_methods);

  lua_pushinteger(L, BLOOPS_SQUARE);
  lua_setfield(L, -2, "SQUARE");
  lua_pushinteger(L, BLOOPS_SAWTOOTH);
  lua_setfield(L, -2, "SAWTOOTH");
  lua_pushinteger(L, BLOOPS_SINE);
  lua_setfield(L, -2, "SINE");
  lua_pushinteger(L, BLOOPS_NOISE);
  lua_setfield(L, -2, "NOISE");

  luaL_newlib(L, bloops_sound_methods);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_setfield(L, -2, "Sound");

  luaL_newlib(L, bloops_track_methods);
  lua_setfield(L, -2, "Track");

  lua_setglobal(L, "Bloops");

  return 0;
}
