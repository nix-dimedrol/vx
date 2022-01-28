#ifndef VX_MATH_TYPES_HPP
#define VX_MATH_TYPES_HPP

#include "utils.hpp"


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

#ifdef VX_USE_GLM_TYPES
using glm::length_t;
template<typename _T, length_t _N>
using vec = glm::vec<_N, _T>;
template<typename _T, length_t _C, length_t _R>
using mat = glm::mat<_C, _R, _T>;
#else
using length_t = size_t;
#endif

template<typename _T> using vec2 = vec<_T, 2>;
template<typename _T> using vec3 = vec<_T, 3>;
template<typename _T> using vec4 = vec<_T, 4>;

using vec2f = vec2<float>;
using vec3f = vec3<float>;
using vec4f = vec4<float>;

using vec2i = vec2<int>;
using vec3i = vec3<int>;
using vec4i = vec4<int>;

} // namespace vx


#endif // VX_MATH_TYPES_HPP
