#ifndef VX_GL_UNIFORM_HPP
#define VX_GL_UNIFORM_HPP


#include "common.hpp"

#ifdef VX_USE_GLM_TYPES
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#else
#include "vec.hpp"
#include "mat.hpp"
#endif


namespace vx
{
namespace gl
{

using valid_types = types_pack<float, int>;


#ifdef VX_USE_GLM_TYPES
using glm::length_t;
template<typename _T, length_t _N>
using vec = glm::vec<_N, _T>;
template<typename _T, length_t _C, length_t _R>
using mat = glm::mat<_C, _R, _T>;
#else
using length_t = size_t;
#endif


namespace detail
{

template<typename _T>
constexpr auto __get_proc_type_suffix(void) noexcept;

template<> constexpr auto __get_proc_type_suffix<float>(void) noexcept { return _ct::make_string('f'); }
template<> constexpr auto __get_proc_type_suffix<int>(void) noexcept { return _ct::make_string('i'); }


#ifdef VX_USE_GLM_TYPES
#endif

template<typename>
struct __impl_uniform;

template<typename _T, length_t _N>
struct __impl_uniform<vec<_T, _N>>
{
	using proc_type = std::enable_if_t<valid_types::is_contain<_T>::value,
		GLvoid(*)(GLint, GLsizei, _T const *)>;
	static constexpr auto proc_name = _ct::make_string(_ct::make_string("glUniform"),
		_ct::make_string(char(_N + '0')), __get_proc_type_suffix<_T>(), _ct::make_string('v'));
};

template<typename _T, length_t _C, length_t _R>
struct __impl_uniform<mat<_T, _C, _R>>
{
	using proc_type = std::enable_if_t<valid_types::is_contain<_T>::value,
		GLvoid(*)(GLint, GLsizei, GLboolean, _T const *)>;
	static constexpr auto proc_name = _ct::make_string(_ct::make_string("glUniformMatrix"),
		_ct::__if<bool_constant<(_C == _R)>>(_ct::make_string(char(_C + '0')),
			_ct::make_string(_ct::make_string(char(_C + '0')), _ct::make_string('x'), _ct::make_string(char(_R + '0')))),
		__get_proc_type_suffix<_T>(), _ct::make_string('v'));
};


template<typename _T, typename _Predicate>
void __load_uniform_procs(_Predicate _pred)
{
	_ct::__for<4>([&](auto it) { __load_proc<__impl_uniform<vec<_T, it.value + 1>>>(_pred); });
	_ct::__for<3>([&](auto col) {
		_ct::__for<3>([&](auto row){ __load_proc<__impl_uniform<mat<_T, col.value + 2, row.value + 2>>>(_pred);});
	});
}

template<typename _Predicate, typename... _Args>
void __load_uniform_procs(_Predicate _pred, types_pack<_Args...>)
{
	int ret[]{0, ((void)__load_uniform_procs<_Args>(_pred), 0)...};
}

} // namespace details



template<typename _Predicate>
void load_uniform_procs(_Predicate _pred)
{
	detail::__load_uniform_procs(_pred, valid_types{});
}


template<typename _T, length_t _N>
GLvoid load_uniform(GLint _loc, vec<_T, _N> const & _v)
{
	_tfunc<detail::__impl_uniform<vec<_T, _N>>>::proc(_loc, 1,
		reinterpret_cast<_T const *>(&_v));
}

template<typename _T, length_t _N>
GLvoid load_uniform(GLint _loc, vec<_T, _N> const * _arr, size_t _cnt)
{
	_tfunc<detail::__impl_uniform<vec<_T, _N>>>::proc(_loc, _cnt,
		reinterpret_cast<_T const *>(_arr));
}

template<typename _T, length_t _C, length_t _R>
GLvoid load_uniform(GLint _loc, mat<_T, _C, _R> const & _m, bool _trsp = false)
{
	_tfunc<detail::__impl_uniform<mat<_T, _C, _R>>>::proc(_loc, 1, _trsp,
		reinterpret_cast<_T const *>(&_m));
}

template<typename _T, length_t _C, length_t _R>
GLvoid load_uniform(GLint _loc, mat<_T, _C, _R> const * _arr, size_t _cnt, bool _trsp = false)
{
	_tfunc<detail::__impl_uniform<mat<_T, _C, _R>>>::proc(_loc, _cnt, _trsp,
		reinterpret_cast<_T const *>(_arr));
}

template<typename _T>
std::enable_if_t<valid_types::is_contain<_T>::value, GLvoid>
load_uniform(GLint _loc, _T _val)
{
	_tfunc<detail::__impl_uniform<vec<_T, 1>>>::proc(_loc, 1, &_val);
}

template<typename _T>
std::enable_if_t<valid_types::is_contain<_T>::value, GLvoid>
load_uniform(GLint _loc, _T const * _arr, size_t _cnt)
{
	_tfunc<detail::__impl_uniform<vec<_T, 1>>>::proc(_loc, _cnt, _arr);
}

} // namespace gl
} // namespace vx

#endif // VX_GL_UNIFORM_HPP
