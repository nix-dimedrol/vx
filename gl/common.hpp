#ifndef VX_GL_COMMON_HPP
#define VX_GL_COMMON_HPP


#if __cplusplus < 201703L
#error "vx_gl supports c++17 and above"
#endif


#include <GL/gl.h>

#include "../utils.hpp"



#define __VX_GL_DECL_PROC(impl_name, _proc_type, _proc_name) struct __impl_ ## impl_name { \
		using proc_type = _proc_type; \
		static constexpr auto proc_name = __ct::make_string(_proc_name); \
	}


namespace vx::gl
{

template<typename _T>
struct _tfunc : _T
{
	using proc_type = typename _T::proc_type;
	static proc_type proc;
};
template<typename _T> typename _tfunc<_T>::proc_type _tfunc<_T>::proc{};

} // namespace vx::gl


#endif // VX_GL_COMMON_HPP
