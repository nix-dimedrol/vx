#ifndef VX_VEC_HPP
#define VX_VEC_HPP

#include "utils.hpp"

#ifdef VX_GLM_EXT
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#endif


namespace vx
{

template<typename _T, size_t _N,
	typename = std::enable_if_t<(_N > 0 && _N <= 4) && std::is_arithmetic<_T>::value>>
struct vec : _ct::s_array<_T, _N>
{
	using base_type  = _ct::s_array<_T, _N>;
	using value_type = typename base_type::value_type;
	using self_type  = vec<value_type, _N>;

	constexpr explicit vec(void) noexcept {}

	template<size_t _M, size_t... _Args,
		typename = std::enable_if_t<_N == size_sequence_pack<_M, _Args...>::sum::value>>
	constexpr explicit vec(vec<value_type, _M> const & _first,
		vec<value_type, _Args> const &... _other) noexcept
		: base_type(_first, _other...) {}

	template<typename... _Args, typename = std::enable_if_t<sizeof ...(_Args) == _N &&
		types_pack<value_type, std::remove_reference_t<std::remove_const_t<_Args>>...>::is_same::value>>
	constexpr vec(_Args... args) noexcept : base_type({args...}) {}

	template<size_t _M, typename = std::enable_if_t<(_M > _N)>>
	constexpr explicit vec(vec<value_type, _M> const & _v) noexcept
		: base_type(_v) {}

	template<typename _U>
	constexpr explicit vec(vec<_U, _N> const & _v) noexcept
		: base_type(_v) {}

	constexpr explicit vec(_T _v) : base_type(_v) {}

#ifdef VX_GLM_EXT

	using glm_type = glm::vec<_N, value_type>;


	constexpr vec(glm_type & _v) noexcept
		: self_type(*static_cast<self_type*>(static_cast<void*>(&_v))) {}

	constexpr vec(glm_type const & _v) noexcept
		: self_type(*static_cast<self_type const *>(static_cast<void const *>(&_v))) {}


	constexpr glm_type & to_glm(void) noexcept
	{ return *static_cast<glm_type*>(static_cast<void*>(this)); }
	constexpr glm_type const & to_glm(void) const noexcept
	{ return *static_cast<glm_type*>(static_cast<void*>(this)); }

#endif

};


#ifdef VX_GLM_EXT

template<typename _T, size_t _N>
constexpr glm::vec<_N, _T> & to_glm(vec<_T, _N> & _v) noexcept
{
	return *static_cast<glm::vec<_N, _T>*>(static_cast<void*>(&_v));
}

template<typename _T, size_t _N>
constexpr glm::vec<_N, _T> const & to_glm(vec<_T, _N> const & _v) noexcept
{
	return *static_cast<glm::vec<_N, _T> const *>(static_cast<void const *>(&_v));
}

template<typename _T, size_t _N>
constexpr vec<_T, _N> & from_glm(glm::vec<_N, _T> & _v) noexcept
{
	return *static_cast<vec<_T, _N>*>(static_cast<void*>(&_v));
}

template<typename _T, size_t _N>
constexpr vec<_T, _N> const & from_glm(glm::vec<_N, _T> const & _v) noexcept
{
	return *static_cast<vec<_T, _N> const *>(static_cast<void const *>(&_v));
}

#endif

template<typename _T> using vec1 = vec<_T, 1>;
template<typename _T> using vec2 = vec<_T, 2>;
template<typename _T> using vec3 = vec<_T, 3>;
template<typename _T> using vec4 = vec<_T, 4>;

using vec1f = vec1<float>;
using vec2f = vec2<float>;
using vec3f = vec3<float>;
using vec4f = vec4<float>;

} // namespace vx


#endif
