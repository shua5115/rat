#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "rat.h"

#if defined(_MSC_VER)
#	define SHARED_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#	define SHARED_EXPORT __attribute__((visibility("default")))
#else
#	define SHARED_EXPORT
#endif

SHARED_EXPORT rat_t *check_rat(lua_State *L, int idx);
SHARED_EXPORT int is_rat(lua_State *L, int idx);
SHARED_EXPORT rat_t opt_rat(lua_State *L, int idx, rat_t def);
SHARED_EXPORT int push_rat(lua_State *L, rat_t val);

SHARED_EXPORT int luaopen_rat(lua_State *L);