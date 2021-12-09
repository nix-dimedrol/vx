#ifndef VX_UTILS_HPP
#define VX_UTILS_HPP

#if __cplusplus < 201402L
#error "vx supports c++14 and above"
#endif

#include <utility>
#include <array>

#ifdef VX_USE_BOOST
#include <boost/utility.hpp>
#endif

namespace vx
{

using std::size_t;

#ifdef VX_USE_BOOST
using boost::noncopyable;
#else
struct noncopyable
{
protected:
	noncopyable(noncopyable const &) = delete;
	noncopyable& operator= (noncopyable const &) = delete;
};
#endif

namespace detail
{

template<typename...> struct __and;
template<typename _T> struct __and<_T> : _T {};
template<typename _Arg0, typename... _Args> struct __and<_Arg0, _Args...>
	: std::integral_constant<bool, _Arg0::value && __and<_Args...>::value> {};


template<typename...> struct __or;
template<typename _T> struct __or<_T> : _T {};
template<typename _Arg0, typename... _Args> struct __or<_Arg0, _Args...>
	: std::integral_constant<bool, _Arg0::value || __or<_Args...>::value> {};


template<typename _T, typename... _Args> struct is_pack_contain;
template<typename _T, typename _Arg, typename... _Args>
struct is_pack_contain<_T, _Arg, _Args...>
	: __or<std::is_same<_T, _Arg>, is_pack_contain<_T, _Args...>> {};


template<typename...> struct is_pack_same;
template<typename _T> struct is_pack_same<_T> : std::true_type {};
template<typename _Arg0, typename _Arg1, typename... _Args>
struct is_pack_same<_Arg0, _Arg1, _Args...>
	: __and<std::is_same<_Arg0, _Arg1>, is_pack_same<_Arg1, _Args...>> {};


template<typename _T, _T...> struct __sum;
template<typename _T> struct __sum<_T>
{
	static constexpr _T value = _T{};
};
template<typename _T, _T _Arg0, _T... _Args> struct __sum<_T, _Arg0, _Args...>
{
	static constexpr _T value = _Arg0 + __sum<_T, _Args...>::value;
};

template<size_t _N>
struct __num
{
	static constexpr size_t value = _N;
};

} // namespace detail


template<typename>
struct sum;

template<typename _T, _T... _Args>
struct sum<std::integer_sequence<_T, _Args...>> : detail::__sum<_T, _Args...> {};


template<typename ..._Args>
struct types_pack
{
	using is_same = detail::is_pack_same<_Args...>;

	template<typename _T>
	using is_contain = detail::is_pack_contain<_T, _Args...>;
};


template<size_t _N, size_t... _Values>
struct ones_sequence_make : ones_sequence_make<_N - 1, 1, _Values...> {};
template<size_t... _Values>
struct ones_sequence_make<0, _Values...> : std::index_sequence<_Values...> {};


namespace _ct
{


template<typename _Predicate, size_t... _Args>
void __for(_Predicate _pred, std::index_sequence<_Args...>)
{
	int ret[]{0, ((void)_pred(detail::__num<_Args>{}), 0)...};
}

template<size_t _N, typename _Predicate>
void __for(_Predicate _pred)
{
	__for(_pred, std::make_index_sequence<_N>{});
}


template<typename _T, size_t _N>
struct s_array : std::array<_T, _N>
{
	using base_type       = std::array<_T, _N>;
	using value_type      = typename base_type::value_type;
	using reference       = typename base_type::reference;
	using const_reference = typename base_type::const_reference;

	template<size_t _M>
	constexpr reference get(void) noexcept { return std::get<_M>(*this); }
	template<size_t _M>
	constexpr const_reference get(void) const noexcept { return std::get<_M>(*this); }

	constexpr explicit s_array(void) noexcept {}

	constexpr explicit s_array(value_type _v) noexcept
		: s_array(_v, ones_sequence_make<_N>{}) {}

	template<size_t _M, typename = std::enable_if_t<(_N >= _M)>>
	constexpr explicit s_array(value_type const (&_ar)[_M]) noexcept
		: s_array(_ar, std::make_index_sequence<_M>{}) {}

	template<size_t _M, size_t _L, typename = std::enable_if_t<(_N >= _M + _L)>>
	constexpr explicit s_array(
		s_array<value_type, _M> const & _v1, s_array<value_type, _L> const & _v2) noexcept
		: s_array(_v1, std::make_index_sequence<_M>{}, _v2, std::make_index_sequence<_L>{}) {}

	template<size_t _M, size_t... _Args,
		typename = std::enable_if_t<_N >= sum<std::index_sequence<_M, _Args...>>::value>>
	constexpr explicit s_array(s_array<value_type, _M> const & _first,
		s_array<value_type, _Args> const &... _other) noexcept
		: s_array(_first, s_array<value_type, sum<std::index_sequence<_Args...>>::value>(_other...)) {}

	template<size_t _M, typename = std::enable_if_t<(_M > _N)>>
	constexpr explicit s_array(s_array<value_type, _M> const & _v) noexcept
		: s_array(_v, std::make_index_sequence<_N>{}) {}

	template<typename _U>
	constexpr explicit s_array(s_array<_U, _N> const & _v) noexcept
		: s_array(_v, std::make_index_sequence<_N>{}) {}

protected:

	template<size_t... _Values>
	constexpr explicit s_array(value_type _v, std::index_sequence<_Values...>) noexcept
		: base_type{static_cast<value_type>(_v * static_cast<value_type>(_Values))...} {}

	template<size_t _M, size_t... _Indices>
	constexpr explicit s_array(value_type const (&_ar)[_M], std::index_sequence<_Indices...>) noexcept
		: base_type{_ar[_Indices]...} {}

	template<size_t _M, size_t _L, size_t... _Indices1, size_t... _Indices2>
	constexpr explicit s_array(
		s_array<value_type, _M> const & _v1, std::index_sequence<_Indices1...>,
		s_array<value_type, _L> const & _v2, std::index_sequence<_Indices2...>) noexcept
		: base_type{_v1.template get<_Indices1>()..., _v2.template get<_Indices2>()...} {}

	template<size_t _M, size_t... _Indices, typename = std::enable_if_t<(_M > _N)>>
	constexpr explicit s_array(s_array<value_type, _M> const & _v, std::index_sequence<_Indices...>) noexcept
		: base_type({_v.template get<_Indices>()...}) {}

	template<typename _U, size_t... _Indices>
	constexpr explicit s_array(s_array<_U, _N> const & _v, std::index_sequence<_Indices...>) noexcept
		: base_type({_v.template get<_Indices>()...}) {}
};


template<size_t _N>
using s_string = s_array<char, _N>;


template<size_t _N>
constexpr auto make_string(char const (&_arr)[_N]) noexcept
{
	return s_string<_N>(_arr);
}

constexpr auto make_string(char _s) noexcept
{
	return s_string<2>({_s, 0});
}

template<size_t _N, size_t _M>
constexpr auto make_string(s_string<_N> const & _v1, s_string<_M> const & _v2) noexcept
{
	return s_string<_N + _M - 1>(s_string<_N - 1>(_v1), _v2);
}

template<size_t _Arg0, size_t... _Args, typename = std::enable_if_t<(sizeof... (_Args) > 1)>>
constexpr auto make_string(s_string<_Arg0> const & _first, s_string<_Args> const &... _other) noexcept
{
	return make_string(_first, make_string(_other...));
}


}


} // namespace vx

#endif // VX_UTILS_HPP
