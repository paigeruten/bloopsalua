#include <unistd.h>
#include "bloopsaphone.h"
#include "lua.h"
#include "lauxlib.h"

static void bloopsaplay(const char *notes) {
  bloops *B = bloops_new();
  B->tempo = 120;
  bloopsaphone *P = bloops_square();
  bloopsatrack *track = bloops_track2(B, P, (char *)notes);
  bloops_sound_destroy(P);
  bloops_play(B);
  while (!bloops_is_done(B)) usleep(1000);
  bloops_track_destroy(track);
  bloops_destroy(B);
}

static int l_bloopsaplay(lua_State *L) {
  const char *notes = luaL_checkstring(L, 1);
  bloopsaplay(notes);
  return 0;
}

extern int luaopen_lbloops(lua_State *L) {
  lua_pushcfunction(L, l_bloopsaplay);
  lua_setglobal(L, "bloopsaplay");
  return 0;
}