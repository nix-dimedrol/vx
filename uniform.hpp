#ifndef VX_UNIFORM_HPP
#define VX_UNIFORM_HPP

#if __cplusplus < 201703L
#error "vx_gl supports c++17 and above"
#endif

#include <GL/gl.h>

#include "utils.hpp"
#include "vec.hpp"
#include "mat.hpp"


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


template<typename>
struct __uniform_impl;

template<typename _T, size_t _N>
struct __uniform_impl<vec<_T, _N>>
{
	using proc_type = std::enable_if_t<valid_types::is_contain<_T>::value,
		GLvoid(*)(GLint, GLsizei, _T const *)>;
	static proc_type proc;
	static constexpr auto proc_name = _ct::make_string(_ct::make_string("glUniform"),
		_ct::make_string(char(_N + '0')), __get_proc_type_suffix<_T>(), _ct::make_string('v'));
};
template<typename _T, size_t _N> typename __uniform_impl<vec<_T, _N>>::proc_type
__uniform_impl<vec<_T, _N>>::proc = nullptr;

template<typename _T, size_t _C, size_t _R>
struct __uniform_impl<mat<_T, _C, _R>>
{
	using proc_type = std::enable_if_t<valid_types::is_contain<_T>::value,
		GLvoid(*)(GLint, GLsizei, GLboolean, _T const *)>;
	static proc_type proc;
	static constexpr auto proc_name = _ct::make_string(_ct::make_string("glUniformMatrix"),
		_ct::__if<bool_constant<(_C == _R)>>(_ct::make_string(char(_C + '0')),
				_ct::make_string(_ct::make_string(char(_C + '0')), _ct::make_string('x'), _ct::make_string(char(_R + '0')))),
		__get_proc_type_suffix<_T>(), _ct::make_string('v'));
};
template<typename _T, size_t _C, size_t _R> typename __uniform_impl<mat<_T, _C, _R>>::proc_type
__uniform_impl<mat<_T, _C, _R>>::proc = nullptr;


template<typename _T, typename _Predicate>
void __load_uniform_proc(_Predicate _pred)
{
	using impl_type = __uniform_impl<_T>;
	impl_type::proc = reinterpret_cast<typename impl_type::proc_type>(
		_pred(impl_type::proc_name.data()));
}

} // namespace details


template<typename _T, typename _Predicate>
void load_uniform_procs(_Predicate _pred)
{
	_ct::__for<4>([&](auto it) { detail::__load_uniform_proc<vec<_T, it.value + 1>>(_pred); });
	_ct::__for<3>([&](auto col) {
		_ct::__for<3>([&](auto row){ detail::__load_uniform_proc<mat<_T, col.value + 2, row.value + 2>>(_pred);});
	});
}


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
