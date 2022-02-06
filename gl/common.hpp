#ifndef VX_GL_COMMON_HPP
#define VX_GL_COMMON_HPP


#if __cplusplus < 201703L
#error "vx_gl supports c++17 and above"
#endif


#include <GL/gl.h>

#include "../utils.hpp"


#define __VX_GL_GET_IMPL(expr) VX_PP_CAT(__impl_, expr)

#define __VX_GL_DECL_PROC(impl_name, _proc_type, _proc_name) \
	struct __VX_GL_GET_IMPL(impl_name) { \
		using proc_type = _proc_type; \
		static constexpr auto proc_name = __ct::make_string(_proc_name); }

#define __VX_GL_PACK_TO_TPARAMS(...) \
	VX_PP_EVAL(VX_PP_MAP_LIST(__VX_GL_GET_IMPL, __VA_ARGS__))>
#define __VX_GL_DECL_PROC_PACK(pack_name, ...) using pack_name = \
	types_pack<__VX_GL_PACK_TO_TPARAMS

#define __VX_GL_CALL(impl_name) _tfunc<__VX_GL_GET_IMPL(impl_name)>::proc


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
