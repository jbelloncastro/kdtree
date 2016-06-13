//
// KD-tree is a C++ header-only library with includes some
// implementations for multi-dimensional tree searches.
//
// Copyright (C) 2016 Jorge Bellon Castro
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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

template< typename Integral >
constexpr Integral power( Integral base, Integral exp )
{
	return exp==0? 1: base*power(base,exp-1);
}

} // namespace traits
} // namespace ads

#endif // KDTREE_TRAITS

