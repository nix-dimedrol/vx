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

} // namespace vx


#endif // VX_MATH_TYPES_HPP
