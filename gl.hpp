#ifndef VX_GL_HPP
#define VX_GL_HPP

#include "gl/uniform.hpp"
#include "gl/shader.hpp"
#include "gl/error.hpp"


#ifdef VX_GL_DSA
#define VX_GL_REQ_MAJOR 4
#define VX_GL_REQ_MINOR 2
#else
#define VX_GL_REQ_MAJOR 3
#define VX_GL_REQ_MINOR 3
#endif


namespace vx
{
namespace gl
{

namespace detail
{

template<typename _T, typename _Predicate>
void __load_proc(_Predicate _pred)
{
	using impl_type = _tfunc<_T>;
	impl_type::proc = reinterpret_cast<typename impl_type::proc_type>(
		_pred(impl_type::proc_name.data()));
}

template<typename _Predicate, typename... _Args>
void __load_procs(_Predicate _pred, types_pack<_Args...>)
{
	int ret[]{0, ((void)__load_proc<_Args>(_pred), 0)...};
}

template<typename _T, typename _Predicate>
void __load_uniform_procs(_Predicate _pred)
{
	_ct::__for<1, 4>([&](auto it) {
		__load_proc<__impl_uniform<vec<_T, it.value>>>(_pred);
	});
	_ct::__for<2, 3>([&](auto col) {
		_ct::__for<2, 3>([&](auto row){
			__load_proc<__impl_uniform<mat<_T, col.value, row.value>>>(_pred);
		});
	});

#ifdef VX_GL_DSA
	_ct::__for<1, 4>([&](auto it) {
		__load_proc<__impl_program_uniform<vec<_T, it.value>>>(_pred);
	});
	_ct::__for<2, 3>([&](auto col) {
		_ct::__for<2, 3>([&](auto row){
			__load_proc<__impl_program_uniform<mat<_T, col.value, row.value>>>(_pred);
		});
	});
#endif
}

template<typename _Predicate, typename... _Args>
void __load_uniform_procs(_Predicate _pred, types_pack<_Args...>)
{
	int ret[]{0, ((void)__load_uniform_procs<_Args>(_pred), 0)...};
}

} // namespace detail


template<typename _Predicate>
void load_procs(_Predicate _pred, std::error_code & _ec)
{
	GLint _major, _minor;
	glGetIntegerv(GL_MAJOR_VERSION, &_major);
	glGetIntegerv(GL_MINOR_VERSION, &_minor);
	if (_major < VX_GL_REQ_MAJOR ||
		_major == VX_GL_REQ_MAJOR && _minor < VX_GL_REQ_MINOR)
	{
		_ec = error::invalid_context_version;
		return;
	}
	detail::__load_procs(_pred,
		__pack_concat<__program_types_pack, __shader_types_pack>{});
	detail::__load_uniform_procs(_pred, valid_uniform_types{});
}

template<typename _Predicate>
void load_procs(_Predicate _pred)
{
	std::error_code _ec;
	load_procs(std::move(_pred), _ec);
	if (_ec) throw std::system_error{_ec};
}

} // namespace gl
} // namespace vx

#endif
