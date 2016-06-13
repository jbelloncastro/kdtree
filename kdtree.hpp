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

#ifndef KDTREE
#define KDTREE

#include "detail/kdtree_traits.hpp"
#include "detail/kdtree_node.hpp"
#include "detail/quadtree_node.hpp"
#include "detail/relaxed_kdtree_node.hpp"

namespace ads {

template < typename T,
	typename Node,
	typename = traits::require_kdtree_valid_datatype<T>
	>
class generic_kdtree
{
	public:
		typedef T                    Key;
		typedef detail::mask_type<T> Mask;

		generic_kdtree() :
			_root(nullptr)
		{
		}

		generic_kdtree( std::initializer_list<Key> ilist ) :
			_root(nullptr)
		{
			for( const Key& item : ilist )
				insert( item );
		}

		bool empty() const
		{
			return !_root;
		}

		bool insert( const Key& k )
		{
			bool inserted = false;
			if( empty() ) {
				_root = Node::create_node( k );
				inserted = true;
			} else {
				inserted = _root->insert( k );
			}
			return inserted;
		}

		// Exact search
		const Key* find( const Key& k ) const
		{
			if( empty() )
				return nullptr;
			else
				return _root->find(k);
		}

		// Partial match
		std::list<const Key*> find( const Key& k, const Mask& mask ) const
		{
			if( empty() )
				// Return empty list
				return std::list<const Key*>();
			else
				return _root->find(k, mask);
		}

		// Orthogonal range seach
		std::list<const Key*> find( const Key& lower, const Key& upper ) const
		{
			if( empty() )
				// Return empty list
				return std::list<const Key*>();
			else
				return _root->find(lower, upper);
		}

	private:
		Node* _root;
};

template < typename T >
using relaxed_kdtree = generic_kdtree<T, detail::relaxed_kdtree_node_base<T> >;

template < typename T >
using standard_kdtree = generic_kdtree<T, detail::kdtree_node<T> >;

template < typename T >
using quadtree = generic_kdtree<T, detail::quadtree_node<T> >;

} // namespace ads

#endif // KDTREE

