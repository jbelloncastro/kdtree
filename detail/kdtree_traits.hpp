
#ifndef KDTREE_TRAITS
#define KDTREE_TRAITS

#include <tuple>
#include <array>
#include <utility>

namespace ads {
namespace traits {

template< typename T >
struct is_kdtree_valid_datatype : public std::false_type {};

template< typename T, std::size_t N >
struct is_kdtree_valid_datatype< std::array<T,N> > : public std::true_type {};

template< typename T, typename U >
struct is_kdtree_valid_datatype< std::pair<T,U> > : public std::true_type {};

template< typename... Ts >
struct is_kdtree_valid_datatype< std::tuple<Ts...> > : public std::true_type {};

template< typename T >
using require_kdtree_valid_datatype = typename
  std::enable_if<is_kdtree_valid_datatype<T>::value>::type;

} // namespace traits
} // namespace ads

#endif // KDTREE_TRAITS

