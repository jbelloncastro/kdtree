
#ifndef KDTREE_NODE
#define KDTREE_NODE

#include <array>
#include <list>
#include <tuple>

namespace ads {
namespace detail {

template< typename T >
using mask_type = std::array<bool, std::tuple_size<T>::value>;

template < typename T, std::size_t I = std::tuple_size<t>::value >
constexpr
bool matches_partially( const T& lhs, const T& rhs, const mask_type& mask )
{
	return matches_partially<T,I>(lhs, rhs, mask) &&
	       ( !std::get<I>(mask) || 
	         std::get<I>(lhs) == std::get<I>(rhs) );
}

template < typename T >
constexpr
bool matches_partially<T,0>( const T& lhs, const T& rhs, const mask_type& mask )
{
	return !std::get<0>(mask) || std::get<0>(lhs) == std::get<0>(rhs);
}

template < typename T >
struct kdtree_node_base
{
	// Constants
	static constexpr std::size_t D = std::tuple_size<T>::value; //!< Specifies the number of dimensions

	// Type members
	typedef kdtree_node_base<T>   Node;
	typedef std::array<Node*,2*D> SuccessorTable;

	// Data members
	SuccessorTable _successors;
	T _key; //!< Contains the stored key
};

template < typename T >
struct relaxed_kdtree_node_base
{
	// Constants
	static constexpr std::size_t D = std::tuple_size<T>::value; //!< Specifies the number of dimensions

	// Type members
	typedef kdtree_node_base<T>        Node;
	typedef std::array<Node*,2> SuccessorTable;

	// Member functions
	// Constructors
	relaxed_kdtree_node_base( const T& key ) :
		_successors(),
		_key(key)
	{
	}

	virtual ~relaxed_kdtree_node_base()
	{
		for( Node* child: _successors ) {
			if( child )
				delete child;
		}
	}

	const T& getKey() const { return _key; }

	Node* getSuccessor( std::size_t position ) const noexcept
	{
		return _successors[position];
	}

	template< std::size_t position >
	Node* getSuccessor() const noexcept
	{
		static_assert( position < std::tuple_size<decltype(_successors)>::value, "Parameter exceeds its maximum value" );
		return std::get<position>(_successors);
	}

	// Data members
	SuccessorTable _successors; //!< Contains a pointer for two successors (binary tree)
	T _key; //!< Contains the stored key

	static create_node( const Key& k, std::size_t discriminant );
};

template < typename T, std::size_t discriminant >
struct relaxed_kdtree_node : public relaxed_kdtree_node_base<T> {
	// Member types
	typedef relaxed_kdtree_node_base<T> Base;
	typedef T                           Key;

	// Member functions
	// Constructor
	relaxed_kdtree_node( const Key& key ) :
		Base( key )
	{
	}

	virtual ~relaxed_kdtree_node()
	{
	}

	bool insert( const Key& k2 )
	{
		bool inserted = false;
		const Key& k1 = this->getKey();
		if( std::get<discriminant>(k1) < std::get<discriminant>(k2) ) {
			if( _successors[0] ) {
				inserted = _succesors[0]->insert(k2);
			} else {
				_successors[0] = Base::create_node(k2);
				inserted = true;
			}
		} else if( k1 != k2 ) {
			if( _successors[1] ) {
				inserted = _successors[1]->insert(k2);
			} else {
				_successors[0] = Base::create_node(k2);
				inserted = true;
			}
		}
		return false;
	}

	// Exact search
	const Key* find( const Key& k2 ) const
	{
		const Key& k1 = this->getKey();

		if( std::get<discriminant>(k1) < std::get<discriminant>(k2) ) {
			if( _successors[0] )
				return _successors[0]->find(k2);
			else
				return nullptr;
		} else if( k1 == k2 ) {
			return &k1;
		} else {
			if( _successors[1] )
				return _successors[1]->find(k2);
			else
				return nullptr;
		}
	}

	// Partial match
	std::list<const Key*> find( const Key& k2, const mask_type& mask ) const
	{
		std::list<const Key*> list;
		const Key& k1 = this->getKey();

		if( matches_partially( k1, k2, mask ) )
			list.push_back( &k1 );
		if( std::get<discriminant>(k1) < std::get<discriminant>(k2)
		    && _successor[0] ) {
			list.splice_after( _successor[0]->find(k2, mask) );
		} else if ( _successor[1] ) {
			list.splice_after( _successor[1]->find(k2, mask) );
		}
		return list;
	}

	// Orthogonal range seach
	// Assumes lower(i) <= upper(i) for all i = [0,D-1]
	std::list<const Key*> find( const Key& lower, const Key& upper ) const {
		std::list<const Key*> list;
		const Key& k1 = this->getKey();

		bool left = std::get<discriminant>(lower) <= std::get<discriminant>(k1);
		bool right = std::get<discriminant>(k1) <= std::get<discriminant>(upper);

		if( left || right ) {
			list.push_back( &k1 );
		}
		if( left && _successor[0] ) {
			list.splice_after( _successor[0]->find( lower, upper ) );
		}
		if( right && _successor[1] ) {
			list.splice_after( _successor[0]->find( lower, upper ) );
		}
	}
};

template< typename T, std::size_t Discr >
relaxed_kdtree_node_base<T>* create_node_aux( const T& key, std::size_t discriminant )
{
	if( discriminant == Discr ) {
		return static_cast<relaxed_kdtree_node_base<T>*>( new relaxed_kdtree_node<T,Discr>( key ) );
	} else {
		return create_node_aux<Discr-1>( key, discriminant );
	}
}

template< typename T >
relaxed_kdtree_node_base<T>* create_node_aux<T,0>( const T& key, std::size_t discriminant )
{
	if( discriminant == 0 ) {
		return static_Cast<relaxed_kdtree_node_base<T>*>( new relaxed_kdtree_node_base<T,Discr>(key) );
	}
	// If we arrive at this point, an invalid value of discriminant was provided.
	std::stringstream ss;
	ss << "Error: cannot create a relaxed KDtree node. "
              " Invalid discriminant "
	   << discriminant
	   << std::endl;
	throw std::runtime_error( ss.str() );
}

template< typename T >
relaxed_kdtree_node_base<T>* relaxed_kdtree_node_base<T>::create_node( const T& key )
{
	// Instantiate random generator and set up uniform distribution;
	static std::default_random_engine gen;
	static std::uniform_int_distribution<> dis(0, D-1);

	// Relaxed kdtree discriminant is generated randomly
	std::size_t discriminant = dis( gen );
	return create_node_aux<relaxed_kdtree_node_base<T>::D-1>( key, discriminant );
}

} // namespace detail
} // namespace ads

#endif // KDTREE_NODE
