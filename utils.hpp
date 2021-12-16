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
	: conjunction<std::is_same<_Arg0, _Arg1>, is_pack_same<_Arg1, _Args...>> {};


template<typename _T, _T...> struct __sum;
template<typename _T> struct __sum<_T>
{
	static constexpr _T value = _T{};
};
template<typename _T, _T _Arg0, _T... _Args> struct __sum<_T, _Arg0, _Args...>
{
	static constexpr _T value = _Arg0 + __sum<_T, _Args...>::value;
};


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


template<typename _T, typename _Derived>
struct __handling_entity : noncopyable
{
	using handle_type  = _T;
	using derived_type = _Derived;
	using self_type    = __handling_entity<handle_type, derived_type>;

	explicit __handling_entity(self_type && _other) : _M_handle(std::move(_other._M_handle))
	{ _other._M_handle = handle_type{}; }

	self_type & operator= (self_type && _other)
	{
		if (this == &_other) return *this;
		if (*this) derived_type::destroy_resource(_M_handle);
		_M_handle = std::move(_other._M_handle);
		_other._M_handle = handle_type{};
		return *this;
	}

	~__handling_entity(void) { if (*this) derived_type::destroy_resource(_M_handle); }

	constexpr explicit operator bool (void) const noexcept { return static_cast<bool>(_M_handle); }
	constexpr bool operator! (void) const noexcept { return !static_cast<bool>(*this); }

protected:

	constexpr handle_type & handle(void) noexcept { return _M_handle; }
	constexpr handle_type const & handle(void) const noexcept { return  _M_handle; }

	void destroy(void)
	{
		derived_type::destroy_resource(_M_handle);
		_M_handle = 0;
	}

	explicit __handling_entity(handle_type const & _value = handle_type{}) : _M_handle(_value) {}

private:

	handle_type _M_handle;
};


namespace _ct
{


template<typename _Predicate, size_t... _Args>
void __for(_Predicate _pred, std::index_sequence<_Args...>)
{
	int ret[]{0, ((void)_pred(std::integral_constant<size_t, _Args>{}), 0)...};
}

template<size_t _N, typename _Predicate>
void __for(_Predicate _pred)
{
	__for(_pred, std::make_index_sequence<_N>{});
}


template<typename _Cond, typename _Then, typename _Else>
constexpr auto __if(_Then&& _th, _Else&& _el)
{
	return detail::__if_impl<_Cond>::get(_th, _el);
}


template<typename _T, size_t _N>
struct s_array : std::array<_T, _N>
{
	using base_type       = std::array<_T, _N>;
	using value_type      = typename base_type::value_type;
	using reference       = typename base_type::reference;
	using const_reference = typename base_type::const_reference;

	constexpr explicit s_array(void) noexcept {}

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

	template<size_t _M, size_t... _Indices>
	constexpr explicit s_array(value_type const (&_ar)[_M], std::index_sequence<_Indices...>) noexcept
		: base_type{_ar[_Indices]...} {}

	template<size_t _M, size_t _L, size_t... _Indices1, size_t... _Indices2>
	constexpr explicit s_array(
		s_array<value_type, _M> const & _v1, std::index_sequence<_Indices1...>,
		s_array<value_type, _L> const & _v2, std::index_sequence<_Indices2...>) noexcept
		: base_type{std::get<_Indices1>(_v1)..., std::get<_Indices2>(_v2)...} {}

	template<size_t _M, size_t... _Indices, typename = std::enable_if_t<(_M > _N)>>
	constexpr explicit s_array(s_array<value_type, _M> const & _v, std::index_sequence<_Indices...>) noexcept
		: base_type({std::get<_Indices>(_v)...}) {}

	template<typename _U, size_t... _Indices>
	constexpr explicit s_array(s_array<_U, _N> const & _v, std::index_sequence<_Indices...>) noexcept
		: base_type({std::get<_Indices>(_v)...}) {}
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

} // namespace _ct
} // namespace vx

#endif // VX_UTILS_HPP
