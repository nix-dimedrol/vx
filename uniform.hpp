#ifndef VX_UNIFORM_HPP
#define VX_UNIFORM_HPP

#if __cplusplus < 201703L
#error "vx_gl supports c++17 and above"
#endif

#include <GL/gl.h>

#include "utils.hpp"
#include "vec.hpp"


namespace vx
{
namespace gl
{

using valid_types = types_pack<float, int>;

namespace detail
{

template<typename _T>
constexpr auto __get_proc_type_suffix(void) noexcept;

template<> constexpr auto __get_proc_type_suffix<float>(void) noexcept { return _ct::make_string('f'); }
template<> constexpr auto __get_proc_type_suffix<int>(void) noexcept { return _ct::make_string('i'); }


template<typename _T, typename = std::enable_if_t<valid_types::is_contain<_T>::value>>
using __uniform_vec_proc_type = GLvoid(*)(GLint, GLsizei, _T const *);

//template<typename _T, typename = std::enable_if_t<valid_types::is_contain<_T>::value>>
//using __uniform_mat_proc_type = GLvoid(*)(GLint, GLsizei, GLboolean, _T const *);


template<typename>
struct __uniform_impl;

template<typename _T, size_t _N>
struct __uniform_impl<vec<_T, _N>>
{
	using proc_type = __uniform_vec_proc_type<_T>;
	static proc_type proc;
};
template<typename _T, size_t _N> __uniform_vec_proc_type<_T>
__uniform_impl<vec<_T, _N>>::proc = nullptr;


template<typename>
struct __uniform_proc_name;

template<typename _T, size_t _N>
struct __uniform_proc_name<vec<_T, _N>>
{
	static constexpr auto value = _ct::make_string(_ct::make_string("glUniform"),
		_ct::make_string(char(_N + '0')), __get_proc_type_suffix<_T>(), _ct::make_string('v'));
};


template<typename _T, typename _Predicate>
constexpr void __load_uniform_proc(_Predicate _pred)
{
	__uniform_impl<_T>::proc = static_cast<typename __uniform_impl<_T>::proc_type>(
		_pred(__uniform_proc_name<_T>::value.data()));
}

template<typename _T, typename _Predicate>
constexpr void __load_uniform_vec_procs(_Predicate _pred)
{
	__load_uniform_proc<vec<_T, 1>>();
	__load_uniform_proc<vec<_T, 2>>();
	__load_uniform_proc<vec<_T, 3>>();
	__load_uniform_proc<vec<_T, 4>>();
}

} // namespace detail


template<typename _T, size_t _N>
GLvoid load_uniform(GLint _loc, vec<_T, _N> const & _v)
{
	detail::__uniform_impl<vec<_T, _N>>::proc(_loc, 1, _v.data());
}

template<typename _T, size_t _N>
GLvoid load_uniform(GLint _loc, size_t _cnt, vec<_T, _N> const * _arr)
{
	detail::__uniform_impl<vec<_T, _N>>::proc(_loc, _cnt, _arr);
}

} // namespace gl
} // namespace vx

#endif
