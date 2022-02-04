#ifndef VX_GL_UNIFORM_HPP
#define VX_GL_UNIFORM_HPP


#include "common.hpp"
#include "../math_types.hpp"


namespace vx::gl
{

using valid_uniform_prime_types = types_pack<float, int, unsigned>;


template<typename _T>
struct is_uniform_type_valid : valid_uniform_prime_types::is_contain<_T> {};
template<typename _T, length_t _N>
struct is_uniform_type_valid<vec<_T, _N>>
	: conjunction<is_uniform_type_valid<_T>,
	bool_constant<(_N >= 2 && _N <= 4)>> {};
template<typename _T, length_t _C, length_t _R>
struct is_uniform_type_valid<mat<_T, _C, _R>>
	: conjunction<is_uniform_type_valid<_T>,
	bool_constant<(_C >= 2 && _C <= 4 && _R >= 2 && _R <= 4)>> {};


template<typename _T, typename _U = void>
using enable_if_uniform_valid =
	std::enable_if<is_uniform_type_valid<_T>::value, _U>;

template<typename _T, typename _U = void>
using enable_if_uniform_valid_t =
	typename enable_if_uniform_valid<_T, _U>::type;


namespace detail
{

template<typename _T>
constexpr auto __get_proc_type_suffix(void) noexcept;

template<> constexpr auto
__get_proc_type_suffix<float>(void) noexcept {
	return 'f'; }
template<> constexpr auto
__get_proc_type_suffix<int>(void) noexcept {
	return 'i'; }
template<> constexpr auto
__get_proc_type_suffix<unsigned>(void) noexcept {
	return __ct::make_string("ui"); }

} // namespace details


template<typename> struct __impl_program_uniform;
template<typename _T, length_t _N>
struct __impl_program_uniform<vec<_T, _N>>
{
	using proc_type = GLvoid(*)(GLuint, GLint, GLsizei, _T const *);
	static constexpr auto proc_name = vx::__ct::make_string("glProgramUniform")
		+ (_N + '0') + detail::__get_proc_type_suffix<_T>() + 'v';
};
template<typename _T, length_t _C, length_t _R>
struct __impl_program_uniform<mat<_T, _C, _R>>
{
	using proc_type = GLvoid(*)(GLuint, GLint, GLsizei, GLboolean, _T const *);
	static constexpr auto proc_name =
		vx::__ct::make_string("glProgramUniformMatrix")
		+ __ct::__if<bool_constant<(_C == _R)>>((_C + '0'),
			__ct::make_string(_C + '0') + 'x' + (_R + '0'))
		+ detail::__get_proc_type_suffix<_T>() + 'v';
};

} // namespace vx::gl

#endif // VX_GL_UNIFORM_HPP
