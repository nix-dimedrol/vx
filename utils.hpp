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

#if __cplusplus >= 201703L
#include <string_view>
#elif defined (VX_USE_BOOST)
#include <boost/utility/string_view.hpp>
#endif


#define __VX_PP_EVAL0(...) __VA_ARGS__
#define __VX_PP_EVAL1(...) __VX_PP_EVAL0(__VX_PP_EVAL0(__VA_ARGS__))
#define __VX_PP_EVAL2(...) __VX_PP_EVAL1(__VX_PP_EVAL1(__VA_ARGS__))
#define __VX_PP_EVAL3(...) __VX_PP_EVAL2(__VX_PP_EVAL2(__VA_ARGS__))
#define __VX_PP_EVAL4(...) __VX_PP_EVAL3(__VX_PP_EVAL3(__VA_ARGS__))
#define __VX_PP_EVAL5(...) __VX_PP_EVAL4(__VX_PP_EVAL4(__VA_ARGS__))
#define __VX_PP_EVAL6(...) __VX_PP_EVAL5(__VX_PP_EVAL5(__VA_ARGS__))
#define __VX_PP_EVAL7(...) __VX_PP_EVAL6(__VX_PP_EVAL6(__VA_ARGS__))
#define __VX_PP_EVAL8(...) __VX_PP_EVAL7(__VX_PP_EVAL7(__VA_ARGS__))

#define VX_PP_EVAL __VX_PP_EVAL8
#define VX_PP_EVAL_DEPTH 0x100


#define VX_PP_ARG_1(expr1, ...) expr1
#define VX_PP_ARG_2(expr1, expr2, ...) expr2

#define VX_PP_CAT(expr1, expr2) expr1 ## expr2

#define __VX_PP_NOT_0 1, 1
#define __VX_PP_NOT_IMPL(...) VX_PP_ARG_2(__VA_ARGS__, 0)
#define VX_PP_NOT(expr) __VX_PP_NOT_IMPL(VX_PP_CAT(__VX_PP_NOT_, expr))

#define VX_PP_BOOLEAN(expr) VX_PP_NOT(VX_PP_NOT(expr))

#define __VX_PP_IF_IMPL_1(...) __VA_ARGS__
#define __VX_PP_IF_IMPL_0(...)
#define __VX_PP_IF_IMPL(cond) VX_PP_CAT(__VX_PP_IF_IMPL_, cond)
#define VX_PP_IF(expr) __VX_PP_IF_IMPL(VX_PP_BOOLEAN(expr))

#define __VX_PP_IF_ELSE_IMPL_1(...) __VA_ARGS__ __VX_PP_IF_IMPL_0
#define __VX_PP_IF_ELSE_IMPL_0(...) __VX_PP_IF_IMPL_1
#define __VX_PP_IF_ELSE_IMPL(cond) VX_PP_CAT(__VX_PP_IF_ELSE_IMPL_, cond)
#define VX_PP_IF_ELSE(expr) __VX_PP_IF_ELSE_IMPL(VX_PP_BOOLEAN(expr))

#define __VX_PP_ARGS_LIST_END() 0
#define VX_PP_ARGS_LIST_EMPTY(...) \
	VX_PP_NOT(VX_PP_ARG_1(__VX_PP_ARGS_LIST_END __VA_ARGS__)())

#define __VX_PP_VOID()

#define __VX_PP_MAP_LIST_GLITCH() VX_PP_MAP_LIST
#define VX_PP_MAP_LIST(mproc, expr1, expr2, ...) mproc(expr1), \
	VX_PP_IF_ELSE(VX_PP_ARGS_LIST_EMPTY(__VA_ARGS__)) (mproc(expr2)) ( \
	__VX_PP_MAP_LIST_GLITCH __VX_PP_VOID __VX_PP_VOID ()()() \
		(mproc, expr2, __VA_ARGS__))


namespace vx
{

using std::size_t;


template<bool _V>
using bool_constant = std::integral_constant<bool, _V>;


#ifdef VX_USE_BOOST
using boost::noncopyable;
#else
struct noncopyable
{
protected:
	noncopyable(noncopyable const &) = delete;
	noncopyable& operator= (noncopyable const &) = delete;

	noncopyable(void) = default;
};
#endif


#if __cplusplus >= 201703L

using std::string_view;
using std::conjunction;
using std::disjunction;

#else

#ifdef VX_USE_BOOST
using boost::string_view;
#endif

template<typename...> struct conjunction;
template<typename _T> struct conjunction<_T> : _T {};
template<typename _Arg0, typename... _Args> struct conjunction<_Arg0, _Args...>
	: bool_constant<_Arg0::value && conjunction<_Args...>::value> {};


template<typename...> struct disjunction;
template<typename _T> struct disjunction<_T> : _T {};
template<typename _Arg0, typename... _Args> struct disjunction<_Arg0, _Args...>
	: bool_constant<_Arg0::value || disjunction<_Args...>::value> {};

#endif


namespace detail
{

template<typename _T, typename...> struct is_pack_contain : std::false_type {};
template<typename _T, typename _Arg, typename... _Args>
struct is_pack_contain<_T, _Arg, _Args...>
	: disjunction<std::is_same<_T, _Arg>, is_pack_contain<_T, _Args...>> {};


template<typename...> struct is_pack_same;
template<typename _T> struct is_pack_same<_T> : std::true_type {};
template<typename _Arg0, typename _Arg1, typename... _Args>
struct is_pack_same<_Arg0, _Arg1, _Args...>
	: conjunction<std::is_same<_Arg0, _Arg1>,
		is_pack_same<_Arg1, _Args...>> {};


template<typename _T, _T...> struct __sum;
template<typename _T> struct __sum<_T> {
	static constexpr _T value = _T{}; };
template<typename _T, _T _Arg0, _T... _Args>
struct __sum<_T, _Arg0, _Args...> {
	static constexpr _T value = _Arg0 + __sum<_T, _Args...>::value; };


template<typename>
struct __if_impl;

template<>
struct __if_impl<std::true_type>
{
	template<typename _Then, typename _Else>
	static constexpr auto get(_Then&& _th, _Else&&) { return _th; }
};

template<>
struct __if_impl<std::false_type>
{
	template<typename _Then, typename _Else>
	static constexpr auto get(_Then&&, _Else&& _el) { return _el; }
};

} // namespace detail


template<typename> struct sum;
template<typename _T, _T... _Args>
struct sum<std::integer_sequence<_T, _Args...>>
	: detail::__sum<_T, _Args...> {};


template<typename... _Args>
struct types_pack
{
	using is_same = detail::is_pack_same<_Args...>;

	template<typename _T>
	using is_contain = detail::is_pack_contain<_T, _Args...>;
};

template<typename... _Args0, typename... _Args1>
static constexpr types_pack<_Args0..., _Args1...> __pack_concat_impl
	(types_pack<_Args0...>, types_pack<_Args1...>) noexcept { return {}; }


template<typename...> struct types_pack_concat;
template<typename _Arg0, typename _Arg1>
struct types_pack_concat<_Arg0, _Arg1>
	: decltype (__pack_concat_impl(_Arg0{}, _Arg1{})) {};
template<typename _Arg, typename... _Args>
struct types_pack_concat<_Arg, _Args...>
	: decltype (__pack_concat_impl(_Arg{}, types_pack_concat<_Args...>{})){};


namespace __ct
{


template<size_t _First, typename _Predicate, size_t... _Args>
void __for(_Predicate _pred, std::index_sequence<_Args...>) {
	int ret[]{0, ((void)_pred(
		std::integral_constant<size_t, _First + _Args>{}), 0)...}; }

template<size_t _First, size_t _N, typename _Predicate>
void __for(_Predicate _pred) {
	__for<_First>(_pred, std::make_index_sequence<_N>{}); }


template<typename _Cond, typename _Then, typename _Else>
constexpr auto __if(_Then&& _th, _Else&& _el) {
	return detail::__if_impl<_Cond>::get(_th, _el); }


template<typename _T, size_t _N>
struct __array : std::array<_T, _N>
{
	using base_type       = std::array<_T, _N>;
	using value_type      = typename base_type::value_type;
	using reference       = typename base_type::reference;
	using const_reference = typename base_type::const_reference;

	constexpr explicit __array(void) noexcept {}

	template<size_t _M, typename = std::enable_if_t<(_N >= _M)>>
	constexpr explicit __array(value_type const (&_ar)[_M]) noexcept
		: __array(_ar, std::make_index_sequence<_M>{}) {}

	template<size_t _M, size_t _L,
		typename = std::enable_if_t<(_N >= _M + _L)>>
	constexpr explicit __array(__array<value_type, _M> const & _v1,
		__array<value_type, _L> const & _v2) noexcept
		: __array(_v1, std::make_index_sequence<_M>{}, _v2,
			std::make_index_sequence<_L>{}) {}

	template<size_t _M, size_t... _Args,
		typename = std::enable_if_t<_N >=
			sum<std::index_sequence<_M, _Args...>>::value>>
	constexpr explicit __array(__array<value_type, _M> const & _first,
		__array<value_type, _Args> const &... _other) noexcept
		: __array(_first, __array<value_type,
			sum<std::index_sequence<_Args...>>::value>(_other...)) {}

	template<size_t _M, typename = std::enable_if_t<(_M > _N)>>
	constexpr explicit __array(__array<value_type, _M> const & _v) noexcept
		: __array(_v, std::make_index_sequence<_N>{}) {}

	template<typename _U>
	constexpr explicit __array(__array<_U, _N> const & _v) noexcept
		: __array(_v, std::make_index_sequence<_N>{}) {}

protected:

	template<size_t _M, size_t... _Indices>
	constexpr explicit __array(value_type const (&_ar)[_M],
		std::index_sequence<_Indices...>) noexcept
		: base_type{_ar[_Indices]...} {}

	template<size_t _M, size_t _L, size_t... _Indices1, size_t... _Indices2>
	constexpr explicit __array(__array<value_type, _M> const & _v1,
		std::index_sequence<_Indices1...>, __array<value_type, _L> const & _v2,
		std::index_sequence<_Indices2...>) noexcept
		: base_type{std::get<_Indices1>(_v1)...,
			std::get<_Indices2>(_v2)...} {}

	template<size_t _M, size_t... _Indices,
		typename = std::enable_if_t<(_M > _N)>>
	constexpr explicit __array(__array<value_type, _M> const & _v,
		std::index_sequence<_Indices...>) noexcept
		: base_type({std::get<_Indices>(_v)...}) {}

	template<typename _U, size_t... _Indices>
	constexpr explicit __array(__array<_U, _N> const & _v,
		std::index_sequence<_Indices...>) noexcept
		: base_type({std::get<_Indices>(_v)...}) {}
};


template<size_t _N>
using __string = __array<char, _N>;


template<size_t _N>
constexpr auto make_string(char const (&_arr)[_N]) noexcept {
	return __string<_N>(_arr); }

constexpr auto make_string(char _s) noexcept {
	return __string<2>({_s, 0}); }

template<size_t _N, size_t _M>
constexpr auto make_string(
	__string<_N> const & _v1, __string<_M> const & _v2) noexcept {
		return __string<_N + _M - 1>(__string<_N - 1>(_v1), _v2); }

template<size_t _Arg0, size_t... _Args,
	typename = std::enable_if_t<(sizeof... (_Args) > 1)>>
constexpr auto make_string(__string<_Arg0> const & _first,
	__string<_Args> const &... _other) noexcept {
		return make_string(_first, make_string(_other...)); }


template<size_t _N, size_t _M>
constexpr auto operator+ (__ct::__string<_N> const & _v1,
	__ct::__string<_M> const & _v2) noexcept {
		return __ct::make_string(_v1, _v2); }

template<size_t _N>
constexpr auto operator+ (__ct::__string<_N> const & _v1, char _c) noexcept {
	return __ct::make_string(_v1, __ct::make_string(_c)); }

template<size_t _N>
constexpr auto operator+ (char _c, __ct::__string<_N> const & _v1) noexcept {
	return __ct::make_string(__ct::make_string(_c), _v1); }

template<size_t _N, size_t _M>
constexpr auto operator+ (__ct::__string<_N> const & _v1,
	char const (&_arr)[_M]) noexcept {
		return __ct::make_string(_v1, __ct::make_string(_arr)); }

template<size_t _N, size_t _M>
constexpr auto operator+ (char const (&_arr)[_M],
	__ct::__string<_N> const & _v1) noexcept {
		return __ct::make_string(__ct::make_string(_arr), _v1); }

} // namespace __ct
} // namespace vx

#endif // VX_UTILS_HPP
