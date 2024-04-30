#include "rat_lua.h"
#define RAT_IMPL
#include "rat.h"

SHARED_EXPORT rat_t *check_rat(lua_State *L, int idx) {
  rat_t *rp = luaL_checkudata(L, idx, "rat");
  if (rp == NULL) luaL_error(L, "value is not a rational number");
  return rp;
}

SHARED_EXPORT int is_rat(lua_State *L, int idx) {
  rat_t *rp = lua_touserdata(L, idx);
  if (rp == NULL) return 0;
  lua_getmetatable(L, idx);              // +1
  luaL_getmetatable(L, "rat");           // +1
  int is_rat = lua_rawequal(L, -2, -1);  // +0
  lua_pop(L, 2);                         // -2
  return is_rat;
}

SHARED_EXPORT rat_t opt_rat(lua_State *L, int idx, rat_t def) {
  rat_t *rp = lua_touserdata(L, idx);
  if (is_rat(L, idx)) return *rp;
  return def;
}

SHARED_EXPORT int push_rat(lua_State *L, rat_t val) {
  rat_t *rp = lua_newuserdata(L, sizeof(rat_t));
  luaL_getmetatable(L, "rat");
  lua_setmetatable(L, -2);
  *rp = val;
  return 1;
}

static int ratL_new(lua_State *L) {
  switch(lua_gettop(L)) {
    case 2: return push_rat(L, (rat_t) {luaL_checkinteger(L, 1), luaL_checkinteger(L, 2)});
    default: return push_rat(L, rat_from_double(luaL_checknumber(L, 1)));
  }
}
static int ratL_unm(lua_State *L) {
  rat_t r = *check_rat(L, 1);
  r.num = -r.num;
  return push_rat(L, r);
}
static int ratL_add(lua_State *L) {
  return push_rat(L, rat_add(*check_rat(L, 1), *check_rat(L, 2)));
}
static int ratL_sub(lua_State *L) {
  return push_rat(L, rat_sub(*check_rat(L, 1), *check_rat(L, 2)));
}
static int ratL_mul(lua_State *L) {
  return push_rat(L, rat_mul(*check_rat(L, 1), *check_rat(L, 2)));
}
static int ratL_div(lua_State *L) {
  return push_rat(L, rat_div(*check_rat(L, 1), *check_rat(L, 2)));
}
static int ratL_mod(lua_State *L) {
  return push_rat(L, rat_mod(*check_rat(L, 1), *check_rat(L, 2)));
}
static int ratL_eq(lua_State *L) {
  lua_pushboolean(L, rat_equals(*check_rat(L, 1), *check_rat(L, 2)));
  return 1;
}
static int ratL_lt(lua_State *L) {
  lua_pushboolean(L, rat_compare(*check_rat(L, 1), *check_rat(L, 2)) < 0);
  return 1;
}
static int ratL_le(lua_State *L) {
  lua_pushboolean(L, rat_compare(*check_rat(L, 1), *check_rat(L, 2)) <= 0);
  return 1;
}
static int ratL_compare(lua_State *L) {
  lua_pushinteger(L, rat_compare(*check_rat(L, 1), *check_rat(L, 2)));
  return 1;
}
static int ratL_tostring(lua_State *L) {
  rat_t *rp = check_rat(L, 1);
  char *buf = lua_newuserdata(L, 48);
  sprintf_s(buf, 48, "%lld/%lld", rp->num, rp->div); // using sprintf because lua's pushfstring always casts to int for %d, which is too small for int64
  lua_pushstring(L, buf);
  lua_remove(L, -2); // remove temporary buffer
  return 1;
}
static int ratL_tonumber(lua_State *L) {
  rat_t *rp = check_rat(L, 1);
  double val = INFINITY;
  if (rp->div != 0) val = rat_cast(lua_Number, *rp);
  lua_pushnumber(L, val);
  return 1;
}
static int ratL_concat(lua_State *L) {
  if (is_rat(L, 1)) {
    lua_pushcfunction(L, ratL_tostring);
  } else {
    lua_getglobal(L, "tostring");
  }
  lua_pushvalue(L, 1);
  lua_call(L, 1, 1);
  if (is_rat(L, 2)) {
    lua_pushcfunction(L, ratL_tostring);
  } else {
    lua_getglobal(L, "tostring");
  }
  lua_pushvalue(L, 2);
  lua_call(L, 1, 1);
  lua_concat(L, 2);
  return 1;
}
static int ratL_abs(lua_State *L) {
  rat_t r = *check_rat(L, 1);
  if (r.num < 0) r.num = -r.num;
  if (r.div < 0) r.div = -r.div;
  return push_rat(L, r);
}
static int ratL_min(lua_State *L) {
  int n = lua_gettop(L); /* number of arguments */
  rat_t *dmin = check_rat(L, 1);
  int i;
  for (i = 2; i <= n; i++) {
    rat_t *d = check_rat(L, i);
    if (rat_compare(*d, *dmin) < 0) {
      dmin = d;
    }
  }
  return push_rat(L, *dmin);
}
static int ratL_max(lua_State *L) {
  int n = lua_gettop(L); /* number of arguments */
  rat_t *dmax = check_rat(L, 1);
  int i;
  for (i = 2; i <= n; i++) {
    rat_t *d = check_rat(L, i);
    if (rat_compare(*d, *dmax) > 0) {
      dmax = d;
    }
  }
  return push_rat(L, *dmax);
}
static int ratL_index(lua_State *L) {
  rat_t *rp = check_rat(L, 1);
  
  lua_Integer i = lua_tointeger(L, 2);
  if (i > 0 && i <= 2) {
    lua_Integer val;
    switch(i) {
      case 1: val = (lua_Integer) rp->num; break;
      case 2: val = (lua_Integer) rp->div; break;
    }
    lua_pushinteger(L, val);
    return 1;
  }

  luaL_getmetatable(L, "rat");
  lua_pushvalue(L, 2);
  lua_rawget(L, -2);
  lua_remove(L, -2);
  return 1;
}
static int ratL_newindex(lua_State *L) {
  rat_t *rp = check_rat(L, 1);
  lua_Integer i = luaL_checkinteger(L, 2);
  lua_Integer val = luaL_checkinteger(L, 3);
  switch(i) {
    case 1: rp->num = val; break;
    case 2: rp->div = val; break;
    default: luaL_error(L, "Rational number index out of range (%d), must be 1 or 2", i);
  }
  return 0;
}

SHARED_EXPORT int luaopen_rat(lua_State *L) {
  static const struct luaL_reg ratmt[] = {
    {"__add", ratL_add},
    {"__sub", ratL_sub},
    {"__mul", ratL_mul},
    {"__div", ratL_div},
    {"__mod", ratL_mod},
    {"__unm", ratL_unm},
    {"__eq", ratL_eq},
    {"__lt", ratL_lt},
    {"__le", ratL_le},
    {"__tostring", ratL_tostring},
    {"__index", ratL_index},
    {"__newindex", ratL_newindex},
    {"__concat", ratL_concat},
    {"tonumber", ratL_tonumber},
    {"abs", ratL_abs},
    {"max", ratL_max},
    {"min", ratL_min},
    {"compare", ratL_compare},
    {NULL, NULL}
  };
  luaL_newmetatable(L, "rat");
  // Register all above functions in the created metatable
  lua_pushvalue(L, -1);
  luaL_register(L, NULL, ratmt);
  // // Set the metatable's __index to reference itself
  // lua_pushstring(L, "__index");
  // lua_pushvalue(L, -2);
  // lua_rawset(L, -3);  // rawset(metatable, "__index", metatable)
  // Set global "rat" and return the constructor function
  lua_register(L, "rat", ratL_new);
  lua_pushcfunction(L, ratL_new);
  return 1;
}