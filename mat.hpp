// WIP!

#ifndef VX_MAT_HPP
#define VX_MAT_HPP

#include "utils.hpp"
#include "vec.hpp"

#ifdef VX_GLM_EXT
#include <glm/matrix.hpp>
#endif

namespace vx
{

template<typename _T, size_t _C, size_t _R, typename = std::enable_if_t<(_C >= 2 && _R >= 2)>>
struct mat : std::array<vec<_T, _C>, _R>
{
	using base_type  = std::array<vec<_T, _C>, _R>;
	using vec_type   = typename base_type::value_type;
	using value_type = typename vec_type::value_type;

	constexpr value_type * data(void) noexcept { return base_type::data()->data(); }
	constexpr value_type const * data(void) const noexcept { return base_type::data()->data(); }
};


#ifdef VX_GLM_EXT

template<typename _T, size_t _C, size_t _R>
constexpr glm::mat<_C, _R, _T> & to_glm(mat<_T, _C, _R> & _m) noexcept
{
	return *static_cast<glm::mat<_C, _R, _T>*>(static_cast<void*>(&_m));
}

template<typename _T, size_t _C, size_t _R>
constexpr glm::mat<_C, _R, _T> const & to_glm(mat<_T, _C, _R> const & _m) noexcept
{
	return *static_cast<glm::mat<_C, _R, _T> const *>(static_cast<void const *>(_m));
}

template<typename _T, glm::length_t _C, glm::length_t _R>
constexpr mat<_T, _C, _R> & from_glm(glm::mat<_C, _R, _T> & _m) noexcept
{
	return *static_cast<mat<_T, _C, _R>*>(static_cast<void*>(&_m));
}

template<typename _T, glm::length_t _C, glm::length_t _R>
constexpr mat<_T, _C, _R> const & from_glm(glm::mat<_C, _R, _T> const & _m) noexcept
{
	return *static_cast<mat<_T, _C, _R> const *>(static_cast<void const *>(&_m));
}

#endif

}

#endif // MAT_HPP
