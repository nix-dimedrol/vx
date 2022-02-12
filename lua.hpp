#ifndef VX_LUA_HPP
#define VX_LUA_HPP


#if __cplusplus < 201703L
#error "vx_lua supports c++17 and above"
#endif


extern "C"
{
#include "lua.h"
}

#include <functional>
#include "utils.hpp"


#define __VX_LUA_DECLARE_REGISTRY_MARK(expr) static const void* (expr){&(expr)}


namespace vx
{
namespace lua
{

struct nil : std::false_type {};


void push(lua_State* _L, nil) { lua_pushnil(_L); }
void push(lua_State* _L, bool _val){ lua_pushboolean(_L, _val); }
void push(lua_State* _L, lua_Integer _val) { lua_pushinteger(_L, _val); }
void push(lua_State* _L, lua_Number _val) { lua_pushnumber(_L, _val); }


void push(lua_State* _L, string_view const & _val) {
	lua_pushlstring(_L, _val.data(), _val.size()); }


template<typename... _Args> void push(lua_State* _L,
	std::tuple<_Args...> const & _val)
{
	__ct::__for<0, std::size(_val)>([&](auto it) {
		push(_L, std::get<it.value>(_val)); });
}


using proc_type = std::function<int(lua_State*)>;

namespace detail
{

static int __proc_dispatch(lua_State* _L)
{
	auto _proc = *static_cast<proc_type*>(lua_touserdata(_L, 1));
	lua_remove(_L, 1);
	return _proc(_L);
}

static int __proc_cleanup(lua_State* _L)
{
	static_cast<proc_type*>(lua_touserdata(_L, 1))->~proc_type();
	return 0;
}

} // namespace detail


__VX_LUA_DECLARE_REGISTRY_MARK(__proc_registry_mark);


void register_proc_table(lua_State* _L)
{
	lua_createtable(_L, 0, 2);
	lua_pushcfunction(_L, detail::__proc_dispatch);
	lua_setfield(_L, -2, "__call");
	lua_pushcfunction(_L, detail::__proc_cleanup);
	lua_setfield(_L, -2, "__gc");
	lua_rawsetp(_L, LUA_REGISTRYINDEX, __proc_registry_mark);
}

void push(lua_State* _L, proc_type const & _func)
{
	new (lua_newuserdata(_L, sizeof (proc_type))) proc_type{_func};
	lua_rawgetp(_L, LUA_REGISTRYINDEX, __proc_registry_mark);
	lua_setmetatable(_L, -2);
}

void push(lua_State* _L, lua_CFunction _func)
{
	push(_L, proc_type{_func});
}


} // namespace lua
} // namespace vx


#endif
