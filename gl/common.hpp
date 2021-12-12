#ifndef VX_GL_COMMON_HPP
#define VX_GL_COMMON_HPP


#if __cplusplus < 201703L
#error "vx_gl supports c++17 and above"
#endif


#include <GL/gl.h>

#include "utils.hpp"


#define __VX_GL_DECL_PROC(impl_name, _proc_type, _proc_name) struct __impl_ ## impl_name { \
		using proc_type = _proc_type; \
		static constexpr auto proc_name = _ct::make_string(_proc_name); \
	}

namespace vx
{
namespace gl
{

template<typename _T>
struct __func : _T
{
	using proc_type = typename _T::proc_type;
	static proc_type proc;
};
template<typename _T> typename __func<_T>::proc_type __func<_T>::proc = nullptr;


template<typename _T, typename _Predicate>
void __load_proc(_Predicate _pred)
{
	using impl_type = __func<_T>;
	impl_type::proc = reinterpret_cast<typename impl_type::proc_type>(
		_pred(impl_type::proc_name.data()));
}

template<typename _Predicate, typename... _Args>
void __load_procs(_Predicate _pred, types_pack<_Args...>)
{
	int ret[]{0, ((void)__load_proc<_Args>(_pred), 0)...};
}


}
}


#endif // VX_GL_COMMON_HPP
