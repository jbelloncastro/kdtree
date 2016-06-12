
#ifndef KDTREE
#define KDTREE

#include "detail/kdtree_traits.hpp"
#include "detail/kdtree_node.hpp"

#include <random>

namespace ads {

template < typename T,
	typename = traits::require_kdtree_valid_datatype<T>
	>
class RelaxedKDTree
{
	public:
		typedef T                                   Key;
		typedef detail::mask_type<T>                Mask;
		typedef detail::relaxed_kdtree_node_base<T> Node;

		RelaxedKDTree() :
			_root(nullptr)
		{
		}

		RelaxedKDTree( std::initializer_list<Key> ilist ) :
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

} // namespace ads

#endif // KDTREE

