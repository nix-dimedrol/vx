#ifndef VX_GL_HPP
#define VX_GL_HPP


#include "gl/shader.hpp"


#define VX_GL_REQ_MAJOR 4
#define VX_GL_REQ_MINOR 2


#ifdef VX_GL_EXPAND_DIRECT_CALLS
#define VX_GL_CALL ::vx::gl::__VX_GL_CALL
#endif


namespace vx::gl
{

namespace detail
{

template<typename _T, typename _Predicate>
static void __load_proc(_Predicate _pred)
{
	using impl_type = _tfunc<_T>;
	impl_type::proc = reinterpret_cast<typename impl_type::proc_type>(
		_pred(impl_type::proc_name.data()));
}

template<typename _Predicate, typename... _Args>
static void __load_procs(_Predicate _pred, types_pack<_Args...>)
{
	int ret[]{0, ((void)__load_proc<_Args>(_pred), 0)...};
}

template<typename _T, typename _Predicate>
static void __load_uniform_procs(_Predicate _pred)
{
	__ct::__for<1, 4>([&](auto it) {
		__load_proc<__impl_program_uniform<vec<_T, it.value>>>(_pred); });
	__ct::__for<2, 3>([&](auto col) {
		__ct::__for<2, 3>([&](auto row){
			__load_proc<__impl_program_uniform<
				mat<_T, col.value, row.value>>>(_pred); }); });
}

template<typename _Predicate, typename... _Args>
static void __load_uniform_procs(_Predicate _pred, types_pack<_Args...>)
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
	detail::__load_procs(_pred, types_pack_concat<__program_procs_pack,
		__shader_procs_pack>{});
	detail::__load_uniform_procs(_pred, valid_uniform_prime_types{});
}

template<typename _Predicate>
void load_procs(_Predicate _pred)
{
	std::error_code _ec;
	load_procs(std::move(_pred), _ec);
	if (_ec) throw std::system_error{_ec};
}

} // namespace vx::gl

#endif
