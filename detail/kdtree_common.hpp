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

namespace ads {
namespace detail {

template< typename T >
using mask_type = std::array<bool, std::tuple_size<T>::value>;

template < typename T, std::size_t I = std::tuple_size<T>::value-1 >
struct matches_partially
{
	bool operator()( const T& lhs, const T& rhs, const mask_type<T>& mask )
	{
		return matches_partially<T,I>()(lhs, rhs, mask) &&
		       ( !std::get<I>(mask) ||
		         std::get<I>(lhs) == std::get<I>(rhs) );
	}
};

template < typename T >
struct matches_partially<T,0>
{
	bool operator() ( const T& lhs, const T& rhs, const mask_type<T>& mask )
	{
		return !std::get<0>(mask) || std::get<0>(lhs) == std::get<0>(rhs);
	}
};

} // namespace detail
} // namespace ads

#endif // KDTREE_COMMON


