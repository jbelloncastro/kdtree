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

#ifndef KDTREE_NODE
#define KDTREE_NODE

#include "kdtree_common.hpp"
#include "kdtree_traits.hpp"

#include <array>
#include <list>
#include <tuple>
#include <type_traits>

namespace ads {
namespace detail {

template < typename T, std::size_t discriminant = 0 >
struct kdtree_node
{
	// Constants
	//! Specifies the number of dimensions
	static constexpr std::size_t D = std::tuple_size<T>::value;
	static constexpr std::size_t next_discriminant = (discriminant+1)%D;

	// Constraints
	static_assert( discriminant < D, "Discriminant value out of range" );
	static_assert( 0 <= discriminant, "Discriminant value out of range" );

	// Type members
	typedef T                                Key;
	typedef kdtree_node<T,discriminant>      Node;
	typedef kdtree_node<T,next_discriminant> SuccessorNode;
	typedef std::array<SuccessorNode*,2>     SuccessorTable;

	// Member functions
	// Constructors
	kdtree_node( const Key& key ) :
		_successors(),
		_key(key)
	{
	}

	~kdtree_node()
	{
		for( SuccessorNode* child: _successors ) {
			if( child )
				delete child;
		}
	}

	const Key& getKey() const { return _key; }

	// Insertion
	bool insert( const Key& k2 )
	{
		bool inserted = false;
		if( std::get<discriminant>(_key) < std::get<discriminant>(k2) ) {
			if( _successors[0] ) {
				inserted = _successors[0]->insert(k2);
			} else {
				_successors[0] = SuccessorNode::create_node(k2);
				inserted = true;
			}
		} else if( _key != k2 ) {
			if( _successors[1] ) {
				inserted = _successors[1]->insert(k2);
			} else {
				_successors[0] = SuccessorNode::create_node(k2);
				inserted = true;
			}
		}
		return inserted;
	}

	// Exact search
	const Key* find( const Key& k2 ) const
	{
		if( std::get<discriminant>(_key) < std::get<discriminant>(k2) ) {
			if( _successors[0] )
				return _successors[0]->find(k2);
			else
				return nullptr;
		} else if( _key == k2 ) {
			return &_key;
		} else {
			if( _successors[1] )
				return _successors[1]->find(k2);
			else
				return nullptr;
		}
	}

	// Partial match
	std::list<const Key*> find( const Key& k2, const mask_type<T>& mask ) const
	{
		std::list<const Key*> list;
		const bool ignore_dimension = mask[discriminant];
		const bool left = ignore_dimension
		               || std::get<discriminant>(_key) < std::get<discriminant>(k2);
		const bool right = ignore_dimension || !left;

		if( matches_partially<Key>()( _key, k2, mask ) ) {
			list.push_back( &_key );
		}
		if( left && _successors[0] ) {
			list.splice( list.begin(), _successors[0]->find(k2, mask) );
		}
		if( left && _successors[1] ) {
			list.splice( list.begin(), _successors[1]->find(k2, mask) );
		}
		return list;
	}

	// Orthogonal range search
	// Assumes lower(i) <= upper(i) for all i = [0,D-1]
	std::list<const Key*> find( const Key& lower, const Key& upper ) const
	{
		std::list<const Key*> list;
		bool left = std::get<discriminant>(lower) <= std::get<discriminant>(_key);
		bool right = std::get<discriminant>(_key) <= std::get<discriminant>(upper);

		if( left || right ) {
			list.push_back( &_key );
		}
		if( left && _successors[0] ) {
			list.splice( list.begin(), _successors[0]->find( lower, upper ) );
		}
		if( right && _successors[1] ) {
			list.splice( list.begin(), _successors[0]->find( lower, upper ) );
		}
	}


	static Node* create_node( const Key& k )
	{
		return new kdtree_node( k );
	}

	// Data members
	SuccessorTable _successors; //!< Contains a pointer for two successors (binary tree)
	Key _key; //!< Contains the stored key
};

} // namespace detail
} // namespace ads

#endif // KDTREE_NODE

