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

#ifndef KDTREE_COMMON
#define KDTREE_COMMON

#include <bitset>

namespace ads {
namespace detail {

template< typename T >
using mask_type = std::bitset<std::tuple_size<T>::value>;

template< typename Integral >
constexpr Integral power( Integral base, Integral exp )
{
	return exp==0? 1: base*power(base,exp-1);
}

template < typename T, std::size_t I = std::tuple_size<T>::value-1 >
struct matches_partially
{
	bool operator()( const T& lhs, const T& rhs, const mask_type<T>& mask )
	{
		return matches_partially<T,I-1>()(lhs, rhs, mask) &&
		       ( !mask[I] ||
		         std::get<I>(lhs) == std::get<I>(rhs) );
	}
};

template < typename T >
struct matches_partially<T,0>
{
	bool operator() ( const T& lhs, const T& rhs, const mask_type<T>& mask )
	{
		return !mask[0] || std::get<0>(lhs) == std::get<0>(rhs);
	}
};

} // namespace detail
} // namespace ads

#endif // KDTREE_COMMON


