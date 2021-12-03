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


} // namespace vx

#endif // COMMON_H
