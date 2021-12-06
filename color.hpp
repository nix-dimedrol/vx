#ifndef VX_COLOR_HPP
#define VX_COLOR_HPP

#include "vec.hpp"


namespace vx
{


namespace rgb
{

template<typename _T, typename _Alfa = std::false_type, size_t _N = 3,
	typename = std::enable_if_t<_N >= 3 && std::is_floating_point<_T>::value>>
struct __color_impl;

template<typename _T, size_t _N>
struct __color_impl<_T, std::false_type, _N> : vec<_T, _N>
{
	using base_type = vec<_T, _N>;
	using value_type      = typename base_type::value_type;
	using reference       = typename base_type::reference;
	using const_reference = typename base_type::const_reference;

	constexpr reference       r(void) noexcept       { return this->template get<0>(); }
	constexpr const_reference r(void) const noexcept { return this->template get<0>(); }
	constexpr reference       g(void) noexcept       { return this->template get<1>(); }
	constexpr const_reference g(void) const noexcept { return this->template get<1>(); }
	constexpr reference       b(void) noexcept       { return this->template get<2>(); }
	constexpr const_reference b(void) const noexcept { return this->template get<2>(); }

	constexpr explicit __color_impl(void) noexcept {}

	constexpr __color_impl(vec<value_type, _N> const & _v) noexcept
		: base_type(_v) {}
};

template<typename _T>
struct __color_impl<_T, std::true_type> : __color_impl<_T, std::false_type, 4>
{
	using base_type = __color_impl<_T, std::false_type, 4>;
	using value_type      = typename base_type::value_type;
	using reference       = typename base_type::reference;
	using const_reference = typename base_type::const_reference;

	constexpr reference       a(void) noexcept       { return this->template get<3>(); }
	constexpr const_reference a(void) const noexcept { return this->template get<3>(); }

	constexpr explicit __color_impl(void) noexcept {}

	constexpr __color_impl(vec4<value_type> const & _v) noexcept
		: base_type(_v) {}
};


template<typename _T, typename _Alfa>
struct basic_color;

template<typename _T>
struct basic_color<_T, std::false_type> : __color_impl<_T, std::false_type>
{
	using base_type = __color_impl<_T, std::false_type>;
	using value_type = typename base_type::value_type;

	constexpr explicit basic_color(void) {}

	constexpr basic_color(vec3<value_type> const & _v)
		: base_type(_v) {}

	constexpr explicit basic_color(vec4<value_type> const & _v)
		: base_type(vec3<value_type>(_v)) {}
};

template<typename _T>
struct basic_color<_T, std::true_type> : __color_impl<_T, std::true_type>
{
	using base_type = __color_impl<_T, std::true_type>;
	using value_type = typename base_type::value_type;

	constexpr explicit basic_color(void) {}

	constexpr explicit basic_color(vec3<value_type> const & _v)
		: base_type(vec4<value_type>(_v, vec1<value_type>(static_cast<value_type>(1.0)))) {}

	constexpr basic_color(vec4<value_type> const & _v)
		: base_type(_v) {}
};


} // namespace rgb


template<typename _T>
using color3 = rgb::basic_color<_T, std::false_type>;

template<typename _T>
using color4 = rgb::basic_color<_T, std::true_type>;

using color3f = color3<float>;
using color4f = color4<float>;


} // namespace vx


#endif
