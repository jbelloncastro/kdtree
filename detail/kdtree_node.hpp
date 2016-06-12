
#ifndef KDTREE_NODE
#define KDTREE_NODE

#include <array>
#include <list>
#include <random>
#include <stdexcept>
#include <tuple>
#include <type_traits>

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
	typedef T                           Key;
	typedef relaxed_kdtree_node_base<T> Node;
	typedef std::array<Node*,2>         SuccessorTable;

	// Member functions
	// Constructors
	relaxed_kdtree_node_base( const Key& key ) :
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

	const Key& getKey() const { return _key; }

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

	virtual bool insert( const Key& k2 ) = 0;

	// Exact search
	virtual const Key* find( const Key& k2 ) const = 0;

	// Partial match
	virtual std::list<const Key*> find( const Key& k2, const mask_type<Key>& mask ) const = 0;

	// Orthogonal range seach
	// Assumes lower(i) <= upper(i) for all i = [0,D-1]
	virtual std::list<const Key*> find( const Key& lower, const Key& upper ) const = 0;

	// Data members
	SuccessorTable _successors; //!< Contains a pointer for two successors (binary tree)
	Key _key; //!< Contains the stored key

	static Node* create_node( const Key& k );
};

template < typename T, std::size_t discriminant = std::tuple_size<T>::value-1 >
struct relaxed_kdtree_node : public relaxed_kdtree_node_base<T> {
	static_assert( discriminant < std::tuple_size<T>::value, "Discriminant value out of range" );
	static_assert( 0 <= discriminant, "Discriminant value out of range" );

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

	virtual bool insert( const Key& k2 )
	{
		bool inserted = false;
		const Key& k1 = this->getKey();
		if( std::get<discriminant>(k1) < std::get<discriminant>(k2) ) {
			if( this->_successors[0] ) {
				inserted = this->_successors[0]->insert(k2);
			} else {
				this->_successors[0] = Base::create_node(k2);
				inserted = true;
			}
		} else if( k1 != k2 ) {
			if( this->_successors[1] ) {
				inserted = this->_successors[1]->insert(k2);
			} else {
				this->_successors[0] = Base::create_node(k2);
				inserted = true;
			}
		}
		return false;
	}

	// Exact search
	virtual const Key* find( const Key& k2 ) const
	{
		const Key& k1 = this->getKey();

		if( std::get<discriminant>(k1) < std::get<discriminant>(k2) ) {
			if( this->_successors[0] )
				return this->_successors[0]->find(k2);
			else
				return nullptr;
		} else if( k1 == k2 ) {
			return &k1;
		} else {
			if( this->_successors[1] )
				return this->_successors[1]->find(k2);
			else
				return nullptr;
		}
	}

	// Partial match
	virtual std::list<const Key*> find( const Key& k2, const mask_type<T>& mask ) const
	{
		std::list<const Key*> list;
		const Key& k1 = this->getKey();

		if( matches_partially<Key>()( k1, k2, mask ) )
			list.push_back( &k1 );
		if( std::get<discriminant>(k1) < std::get<discriminant>(k2)
		    && this->_successors[0] ) {
			list.splice( list.begin(), this->_successors[0]->find(k2, mask) );
		} else if ( this->_successors[1] ) {
			list.splice( list.begin(), this->_successors[1]->find(k2, mask) );
		}
		return list;
	}

	// Orthogonal range seach
	// Assumes lower(i) <= upper(i) for all i = [0,D-1]
	virtual std::list<const Key*> find( const Key& lower, const Key& upper ) const {
		std::list<const Key*> list;
		const Key& k1 = this->getKey();

		bool left = std::get<discriminant>(lower) <= std::get<discriminant>(k1);
		bool right = std::get<discriminant>(k1) <= std::get<discriminant>(upper);

		if( left || right ) {
			list.push_back( &k1 );
		}
		if( left && this->_successors[0] ) {
			list.splice( list.begin(), this->_successors[0]->find( lower, upper ) );
		}
		if( right && this->_successors[1] ) {
			list.splice( list.begin(), this->_successors[0]->find( lower, upper ) );
		}
	}

	static Base* create_node( const T& k, std::size_t discr, std::true_type );
	static Base* create_node( const T& k, std::size_t discr, std::false_type );
};

template< typename T, std::size_t Discr >
relaxed_kdtree_node_base<T>* relaxed_kdtree_node<T,Discr>::create_node( const T& key, std::size_t discriminant, std::false_type )
{
	constexpr bool last_dimension = Discr == 1;
	if( discriminant == Discr ) {
		return static_cast<relaxed_kdtree_node_base<T>*>( new relaxed_kdtree_node<T,Discr>( key ) );
	} else {
		return relaxed_kdtree_node<T,Discr-1>::create_node( key, discriminant, std::integral_constant<bool,last_dimension>() );
	}
}

template< typename T, std::size_t Discr >
relaxed_kdtree_node_base<T>* relaxed_kdtree_node<T,Discr>::create_node( const T& key, std::size_t discriminant, std::true_type )
{
	if( discriminant == Discr ) {
		return static_cast<relaxed_kdtree_node_base<T>*>( new relaxed_kdtree_node<T,Discr>( key ) );
	} else {
		throw std::invalid_argument( "Discriminant value unexpected. Valid values: [0, D-1]" );
	}
}

template< typename T >
relaxed_kdtree_node_base<T>* relaxed_kdtree_node_base<T>::create_node( const T& key )
{
	constexpr std::size_t max_dimension = relaxed_kdtree_node_base<T>::D-1;

	// Instantiate random generator and set up uniform distribution;
	static std::default_random_engine gen;
	static std::uniform_int_distribution<> dis(0, max_dimension);

	// Relaxed kdtree discriminant is generated randomly
	std::size_t discriminant = dis( gen );

	return relaxed_kdtree_node<T,max_dimension>::create_node( key, discriminant, std::false_type() );
}

} // namespace detail
} // namespace ads

#endif // KDTREE_NODE
