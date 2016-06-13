
#include "kdtree.hpp"

#include <iostream>
#include <random>

typedef std::tuple<int,char,float> Key;
typedef ads::detail::mask_type<Key> Mask;

std::default_random_engine gen;

Key generateKey() {
	// Instantiate random generator and set up uniform distributions;
	static std::uniform_int_distribution<int>    int_dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	static std::uniform_int_distribution<char>   char_dist(std::numeric_limits<char>::min(), std::numeric_limits<char>::max());
	static std::uniform_real_distribution<float> float_dist(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());

	// Relaxed kdtree discriminant is generated randomly
	return std::make_tuple( int_dist(gen), char_dist(gen), float_dist(gen) );
}

Mask generateMask() {
	constexpr std::size_t D = std::tuple_size<Key>::value;
	static std::uniform_int_distribution<unsigned long long> mask_dist(0, 1<<(D-1));
	return Mask( mask_dist(gen) );
}

int main( int argc, char* argv[] ) {
#ifndef USE_STANDARD
	ads::relaxed_kdtree<Key> tree;
#else
	ads::standard_kdtree<Key> tree;
#endif

	std::size_t elements = 10000;
	if( argc > 1 )
		elements = atoi(argv[1]);
		
	for( int i = 0; i < elements; i++ ) {
		tree.insert( generateKey() );
	}
	for( int i = 0; i < elements; i++ ) {
		tree.find( generateKey(), generateMask() );
	}

	return 0;
}

