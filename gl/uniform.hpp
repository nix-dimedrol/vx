#ifndef VX_GL_UNIFORM_HPP
#define VX_GL_UNIFORM_HPP


#include "common.hpp"
#include "../math_types.hpp"


namespace vx
{
namespace gl
{

using valid_uniform_types = types_pack<float, int, unsigned>;


template<typename _T>
struct is_uniform_type_valid : valid_uniform_types::is_contain<_T> {};


template<typename> struct is_vec_type_valid : std::false_type {};
template<typename _T, length_t _N>
struct is_vec_type_valid<vec<_T, _N>> : conjunction<is_uniform_type_valid<_T>,
	bool_constant<(_N >= 2 && _N <= 4)>> {};

template<typename _T, length_t _N>
struct is_uniform_type_valid<vec<_T, _N>> : is_vec_type_valid<vec<_T, _N>> {};

template<typename> struct is_mat_type_valid : std::false_type {};
template<typename _T, length_t _C, length_t _R>
struct is_mat_type_valid<mat<_T, _C, _R>> : conjunction<is_uniform_type_valid<_T>,
	bool_constant<(_C >= 2 && _C <= 4 && _R >= 2 && _R <= 4)>> {};

template<typename _T, length_t _C, length_t _R>
struct is_uniform_type_valid<mat<_T, _C, _R>> : is_mat_type_valid<mat<_T, _C, _R>> {};


template<typename _T, typename _U = void>
using enable_if_uniform_valid = std::enable_if<is_uniform_type_valid<_T>::value, _U>;

template<typename _T, typename _U = void>
using enable_if_uniform_valid_t = typename enable_if_uniform_valid<_T, _U>::type;


namespace detail
{

template<typename _T>
constexpr auto __get_proc_type_suffix(void) noexcept;

template<> constexpr auto __get_proc_type_suffix<float>(void) noexcept { return _ct::make_string('f'); }
template<> constexpr auto __get_proc_type_suffix<int>(void) noexcept { return _ct::make_string('i'); }
template<> constexpr auto __get_proc_type_suffix<unsigned>(void) noexcept { return _ct::make_string("ui"); }


template<typename> struct __uniform_type_name;
template<typename _T, length_t _N>
struct __uniform_type_name<vec<_T, _N>>
{
	static constexpr auto value = _ct::make_string(_N + '0')
		+ __get_proc_type_suffix<_T>() + 'v';
};
template<typename _T, length_t _C, length_t _R>
struct __uniform_type_name<mat<_T, _C, _R>>
{
	static constexpr auto value = _ct::make_string("Matrix")
		+ _ct::__if<bool_constant<(_C == _R)>>(char(_C + '0'),
			_ct::make_string(_C + '0') + 'x' + char(_R + '0'))
			+ __get_proc_type_suffix<_T>() + 'v';
};


template<typename> struct __uniform_type_def;
template<typename _T, length_t _N>
struct __uniform_type_def<vec<_T, _N>>
{
	using type = GLvoid(*)(GLint, GLsizei, _T const *);
};
template<typename _T, length_t _C, length_t _R>
struct __uniform_type_def<mat<_T, _C, _R>>
{
	using type = GLvoid(*)(GLint, GLsizei, GLboolean, _T const *);
};


template<typename _T>
struct __impl_uniform
{
	using proc_type = typename __uniform_type_def<_T>::type;
	static constexpr auto proc_name = vx::_ct::make_string("glUniform")
		+ __uniform_type_name<_T>::value;
};

#ifdef VX_GL_DSA

template<typename> struct __program_uniform_type_def;
template<typename _T, length_t _N>
struct __program_uniform_type_def<vec<_T, _N>>
{
	using type = GLvoid(*)(GLuint, GLint, GLsizei, _T const *);
};
template<typename _T, length_t _C, length_t _R>
struct __program_uniform_type_def<mat<_T, _C, _R>>
{
	using type = GLvoid(*)(GLuint, GLint, GLsizei, GLboolean, _T const *);
};

template<typename _T>
struct __impl_program_uniform
{
	using proc_type = typename __program_uniform_type_def<_T>::type;
	static constexpr auto proc_name = vx::_ct::make_string("glProgramUniform")
		+ __uniform_type_name<_T>::value;
};

#endif

template<typename _T, typename _Predicate>
void __load_uniform_procs(_Predicate _pred)
{
	_ct::__for<1, 4>([&](auto it) {
		__load_proc<__impl_uniform<vec<_T, it.value>>>(_pred); });
	_ct::__for<2, 3>([&](auto col) {
		_ct::__for<2, 3>([&](auto row){
			__load_proc<__impl_uniform<mat<_T, col.value, row.value>>>(_pred); });
	});

#ifdef VX_GL_DSA
	_ct::__for<1, 4>([&](auto it) {
		__load_proc<__impl_program_uniform<vec<_T, it.value>>>(_pred); });
	_ct::__for<2, 3>([&](auto col) {
		_ct::__for<2, 3>([&](auto row){
			__load_proc<__impl_program_uniform<mat<_T, col.value, row.value>>>(_pred); });
	});
#endif
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
	detail::__load_uniform_procs(_pred, valid_uniform_types{});
}


template<typename _T, length_t _N>
enable_if_uniform_valid_t<vec<_T, _N>> uniform(GLint _loc,
	vec<_T, _N> const & _v)
{
	_tfunc<detail::__impl_uniform<vec<_T, _N>>>::proc(_loc,
		1, reinterpret_cast<_T const *>(&_v));
}

template<typename _T, length_t _N>
enable_if_uniform_valid_t<vec<_T, _N>> uniform(GLint _loc,
	vec<_T, _N> const * _arr, size_t _cnt)
{
	_tfunc<detail::__impl_uniform<vec<_T, _N>>>::proc(_loc,
		_cnt, reinterpret_cast<_T const *>(_arr));
}

template<typename _T, length_t _C, length_t _R>
enable_if_uniform_valid_t<mat<_T, _C, _R>> uniform(GLint _loc,
	mat<_T, _C, _R> const & _m, bool _trsp = false)
{
	_tfunc<detail::__impl_uniform<mat<_T, _C, _R>>>::proc(_loc,
		1, _trsp, reinterpret_cast<_T const *>(&_m));
}

template<typename _T, length_t _C, length_t _R>
enable_if_uniform_valid_t<mat<_T, _C, _R>> uniform(GLint _loc,
	mat<_T, _C, _R> const * _arr, size_t _cnt, bool _trsp = false)
{
	_tfunc<detail::__impl_uniform<mat<_T, _C, _R>>>::proc(_loc,
		_cnt, _trsp, reinterpret_cast<_T const *>(_arr));
}

template<typename _T>
enable_if_uniform_valid_t<_T> uniform(GLint _loc,
	_T _val)
{
	_tfunc<detail::__impl_uniform<vec<_T, 1>>>::proc(_loc,
		1, &_val);
}

template<typename _T>
enable_if_uniform_valid_t<_T> uniform(GLint _loc,
	_T const * _arr, size_t _cnt)
{
	_tfunc<detail::__impl_uniform<vec<_T, 1>>>::proc(_loc,
		_cnt, _arr);
}

} // namespace gl
} // namespace vx

#endif // VX_GL_UNIFORM_HPP
