#ifndef VX_MAT_HPP
#define VX_MAT_HPP

#include "common.hpp"
#include "vec.hpp"

#ifdef VX_GLM_EXT
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif

namespace vx
{


namespace detail
{

template<typename _T, size_t _C, size_t _R>
using __mat_arr = std::array<vec<_T, _R>, _C>;

#ifdef VX_GLM_EXT

template<typename _T, size_t _C, size_t _R>
constexpr __mat_arr<_T, _C, _R> __glm_mat_to_arr(glm::mat<_C, _R, _T> const & _mat)
{
	return *static_cast<__mat_arr<_T, _C, _R> const *>(static_cast<void const *>(&_mat));
}

template<typename _T, size_t _C, size_t _R>
constexpr glm::mat<_C, _R, _T> __arr_to_glm_mat(__mat_arr<_T, _C, _R> const & _mat)
{
	return *static_cast<glm::mat<_C, _R, _T> const *>(static_cast<void const *>(&_mat));
}

#endif

} // namespace detail



template<typename _T, size_t _C, size_t _R>
struct __mat_impl : detail::__mat_arr<_T, _C, _R> {};


#ifdef VX_GLM_EXT

template<typename _T>
struct __mat_impl<_T, 4, 4> : detail::__mat_arr<_T, 4, 4>
{
	constexpr void translate(vec3<_T> const & _v) noexcept { *this = glm::translate(*this, _v); }
	constexpr void scale(vec3<_T> const & _v) noexcept { *this = glm::scale(*this, _v); }
	constexpr void rotate(vec3<_T> const & _v) noexcept { *this = glm::rotate(*this, _v); }
};

#endif


template<typename _T, size_t _C, size_t _R>
struct __mat_base : __mat_impl<_T, _C, _R>
{
	using base_type  = __mat_impl<_T, _C, _R>;
	using vec_type   = typename base_type::value_type;
	using value_type = typename vec_type::value_type;

	template<size_t _N>
	constexpr vec_type & get(void) noexcept { return std::get<_N>(*this); }
	template<size_t _N>
	constexpr vec_type const & get(void) const noexcept { return std::get<_N>(*this); }

	template<size_t _N, size_t _M>
	constexpr value_type & get(void) noexcept { return std::get<_M>(std::get<_N>(*this)); }
	template<size_t _N, size_t _M>
	constexpr value_type const & get(void) const noexcept { return std::get<_M>(std::get<_N>(*this)); }

	constexpr value_type * data(void) noexcept { return base_type::data()->data(); }
	constexpr value_type const * data(void) const noexcept { return base_type::data()->data(); }
};


template<typename _T, size_t _C, size_t _R>
struct mat : __mat_base<_T, _C, _R>
{
	using base_type  = __mat_base<_T, _C, _R>;
	using value_type = typename base_type::value_type;
	using self_type  = mat<value_type, _C, _R>;

#ifdef VX_GLM_EXT

	using glm_type = glm::mat<_C, _R, value_type>;

	constexpr mat(glm_type & _m) noexcept
		: self_type(*static_cast<self_type*>(static_cast<void*>(&_m))) {}

	constexpr mat(glm_type const & _m) noexcept
		: self_type(*static_cast<self_type const *>(static_cast<void const *>(&_m))) {}

	constexpr glm_type & to_glm(void) noexcept
	{ return *static_cast<glm_type*>(static_cast<void*>(this)); }
	constexpr glm_type const & to_glm(void) const noexcept
	{ return *static_cast<glm_type*>(static_cast<void*>(this)); }


#endif

};


}

#endif // MAT_HPP
