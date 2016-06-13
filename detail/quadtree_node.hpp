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

#ifndef QUADTREE_NODE
#define QUADTREE_NODE

#include "kdtree_common.hpp"
#include "kdtree_traits.hpp"

namespace ads {
namespace detail {

template< typename Node, std::size_t d = std::tuple_size<typename Node::Key>::value-1 >
struct forward_partial_match
{
	typedef typename Node::Key  Key;
	typedef typename Node::Mask Mask;

	std::list<const Key*> operator()( const Node& node, const Key& k, const Mask& m, std::size_t position )
	{
		using next_level = forward_partial_match<Node,d-1>;

		std::list<const Key*> list;
		if( m[d] ) {
			list =
				next_level()( node, k, m, position | 0<<d );
			list.splice( list.begin(),
				next_level()( node, k, m, position | 1<<d ) );
		} else {
			const bool lt = std::get<d>(node._key) < std::get<d>(k);
			list = next_level()( node, k, m, position | lt<<d );
		}
		return list;
	}
};

template< typename Node >
struct forward_partial_match<Node,-1>
{
	typedef typename Node::Key  Key;
	typedef typename Node::Mask Mask;

	std::list<const Key*> operator()( const Node& node, const Key& k, const Mask& m, std::size_t position )
	{
		return node._successor[position]->find( k, m );
	}
};

template < typename T >
struct quadtree_node
{
	// Constants
	static constexpr std::size_t D = std::tuple_size<T>::value; //!< Specifies the number of dimensions

	// Type members
	typedef T                            Key;
	typedef mask_type<T>                 Mask;
	typedef quadtree_node<T>             Node;
	typedef std::array<Node*,power(2ul,D)> SuccessorTable;

	constexpr
	std::size_t find_position( const Key& k, std::size_t dim = D-1 )
	{
		return dim==0?
			std::get<dim>(_key) < std::get<dim>(k) :
			(std::get<dim>(_key) < std::get<dim>(k))<<dim;
	}

	// Function members
	bool insert( const Key& k )
	{
		bool inserted = false;
		std::size_t spot = find_position(k);
		// Note: spot is 0 if all dimension comparisons
		// are greater or equal
		if( spot == 0 && _key == k ) {
			inserted = false;
		} else if( _successors[spot] ) {
			inserted = _successors[spot]->insert(k);
		} else {
			_successors[spot] = new quadtree_node(k);
			inserted = true;
		}
		return inserted;
	}

	// Exact search
	const Key* find( const Key& k ) const
	{
		std::size_t spot = find_spot(k);
		if( spot == 0 && _key == k ) {
			return &k;
		} else if( _successors[spot] ) {
			return _successors[spot]->find(k);
		} else {
			return nullptr;
		}
	}

	// Partial match
	std::list<const Key*> find( const Key& k, const Mask& m )
	{
		std::list<const Key*> list;
		if( matches_partially<Key>()( _key, k, m ) ) {
			list.push_back(&_key);
		}

		list.splice( list.begin(),
			forward_partial_match<Node>( _key, k, m ) );
		return list;
	}

	// Data members
	SuccessorTable _successors;
	T _key; //!< Contains the stored key
};

} // namespace detail
} // namespace ads

#endif // QUADTREE_NODE

