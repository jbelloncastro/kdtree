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

namespace ads {
namespace detail {

template < typename T >
struct quadtree_node
{
	// Constants
	static constexpr std::size_t D = std::tuple_size<T>::value; //!< Specifies the number of dimensions

	// Type members
	typedef T                     Key;
	typedef quadtree_node<T>        Node;
	typedef std::array<Node*,power(2,D)> SuccessorTable;

	constexpr
	std::size_t find_spot( const Key& k, std::size_t dim = D-1 )
	{
		return dim==0?
			std::get<dim>(_key) < std::get<dim>(k) :
			(std::get<dim>(_key) < std::get<dim>(k))<<dim;
	}

	// Function members
	bool insert( const Key& k )
	{
		bool inserted = false;
		std::size_t spot = find_spot(k);
		// Note: spot is 0 if all dimension comparisons
		// are greater or equal
		if( spot == 0 && _key == k ) {
			inserted = false;
		} else if( _successor[spot] ) {
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
		} else if( _successor[spot] ) {
			return _successor[spot]->find(k);
		} else {
			return nullptr;
		}
	}

	// Partial match
	std::list<const Key*> find( const Key& k, const mask_type<Key>& mask )
	{
		std::list<const Key*> list;
		if( matches_partially<Key>()( _key, k, mask ) ) {
			list.push_back(&_key);
		}

		return list;
	}

	// Data members
	SuccessorTable _successors;
	T _key; //!< Contains the stored key
};

} // namespace detail
} // namespace ads

#endif // QUADTREE_NODE

