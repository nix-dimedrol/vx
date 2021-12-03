#ifndef VX_COMMON_HPP
#define VX_COMMON_HPP

#include <utility>

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

} // namespace detail


} // namespace vx

#endif // COMMON_H