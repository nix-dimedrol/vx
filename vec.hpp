#ifndef VX_VEC_HPP
#define VX_VEC_HPP

#include <array>

#include "common.hpp"

#ifdef VX_GLM_EXT
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#endif


namespace vx
{

template<typename _T, size_t _N>
struct __vec_impl : std::array<_T, _N>
{
	using base_type       = std::array<_T, _N>;
	using value_type      = typename base_type::value_type;
	using reference       = typename base_type::reference;
	using const_reference = typename base_type::const_reference;

	template<size_t _M>
	constexpr reference get(void) noexcept { return std::get<_M>(*this); }
	template<size_t _M>
	constexpr const_reference get(void) const noexcept { return std::get<_M>(*this); }

	constexpr explicit __vec_impl(void) noexcept {}

	template<size_t... _Values>
	constexpr explicit __vec_impl(value_type _v, long_sequence_pack<_Values...>) noexcept
		: base_type{(_v * static_cast<value_type>(_Values))...} {}

	constexpr explicit __vec_impl(value_type _v) noexcept
		: __vec_impl(_v, ones_sequence_make<_N>{}) {}

	template<size_t... _Indices>
	constexpr explicit __vec_impl(value_type const (&_ar)[_N], long_sequence_pack<_Indices...>) noexcept
		: base_type{_ar[_Indices]...} {}

	constexpr explicit __vec_impl(value_type const (&_ar)[_N]) noexcept
		: __vec_impl(_ar, index_sequence_make<_N>{}) {}


	template<size_t _M, size_t... _Indices1, size_t... _Indices2>
	constexpr explicit __vec_impl(
		__vec_impl<value_type, _M> const & _v1, long_sequence_pack<_Indices1...>,
		__vec_impl<value_type, _N - _M> const & _v2, long_sequence_pack<_Indices2...>) noexcept
		: base_type{_v1.template get<_Indices1>()..., _v2.template get<_Indices2>()...} {}

	template<size_t _M>
	constexpr explicit __vec_impl(
		__vec_impl<value_type, _M> const & _v1, __vec_impl<value_type, _N - _M> const & _v2) noexcept
		: __vec_impl(_v1, index_sequence_make<_M>{}, _v2, index_sequence_make<_N - _M>{}) {}

	template<size_t _M, size_t... _Args,
		typename = std::enable_if_t<_N == long_sequence_pack<_M, _Args...>::sum::value>>
	constexpr explicit __vec_impl(__vec_impl<value_type, _M> const & _first,
		__vec_impl<value_type, _Args> const &... _other) noexcept
		: __vec_impl(_first, __vec_impl<value_type, long_sequence_pack<_Args...>::sum::value>(_other...)) {}

	template<size_t _M, size_t... _Indices, typename = std::enable_if_t<(_M > _N)>>
	constexpr explicit __vec_impl(__vec_impl<value_type, _M> const & _v, long_sequence_pack<_Indices...>) noexcept
		: base_type({_v.template get<_Indices>()...}) {}

	template<size_t _M, typename = std::enable_if_t<(_M > _N)>>
	constexpr explicit __vec_impl(__vec_impl<value_type, _M> const & _v) noexcept
		: __vec_impl(_v, index_sequence_make<_N>{}) {}

	template<typename _U, size_t... _Indices>
	constexpr explicit __vec_impl(__vec_impl<_U, _N> const & _v, long_sequence_pack<_Indices...>) noexcept
		: base_type({_v.template get<_Indices>()...}) {}

	template<typename _U>
	constexpr explicit __vec_impl(__vec_impl<_U, _N> const & _v) noexcept
		: __vec_impl(_v, index_sequence_make<_N>{}) {}

};


template<typename _T, size_t _N,
	typename = std::enable_if_t<(_N > 0 && _N <= 4) && std::is_arithmetic<_T>::value>>
struct vec : __vec_impl<_T, _N>
{
	using base_type  = __vec_impl<_T, _N>;
	using value_type = typename base_type::value_type;
	using this_type  = vec<value_type, _N>;

	constexpr explicit vec(void) noexcept {}

	template<size_t _M, size_t... _Args,
		typename = std::enable_if_t<_N == long_sequence_pack<_M, _Args...>::sum::value>>
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

#ifdef VX_GLM_EXT

	using glm_type = glm::vec<_N, value_type>;

	static constexpr this_type & from_glm(glm_type & _v) noexcept
	{ return *static_cast<this_type*>(static_cast<void*>(&_v)); }

	static constexpr this_type const & from_glm(glm_type const & _v) noexcept
	{ return *static_cast<this_type const*>(static_cast<void const *>(&_v)); }

	constexpr glm_type & to_glm(void) noexcept
	{ return *static_cast<glm_type*>(static_cast<void*>(this)); }
	constexpr glm_type const & to_glm(void) const noexcept
	{ return *static_cast<glm_type*>(static_cast<void*>(this)); }

#endif

	static constexpr vec<value_type, _N> one(void) noexcept
	{ return static_cast<this_type>(base_type(1)); }

	static constexpr vec<value_type, _N> zero(void) noexcept
	{ return static_cast<this_type>(base_type(0)); }

protected:

	constexpr explicit vec(base_type const & _v) noexcept
		: base_type(_v) {}
};

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
