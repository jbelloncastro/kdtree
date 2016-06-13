
#include "kdtree.hpp"

#include <iostream>

typedef std::tuple<int,char> Key;

int main() {
#ifdef USE_STANDARD
	ads::standard_kdtree<Key> tree;
#elif USE_QUADTREE
	ads::quadtree<Key> tree;
#else
	ads::relaxed_kdtree<Key> tree;
#endif
	for( int i = 0; i < 10; i++ ) {
			tree.insert( std::make_tuple(i,'a'+i) );
	}
	for( int i = 0; i < 10; i++ ) {
			bool found = tree.find( std::make_tuple(i,'a'+i) );
			if( found )
				std::cout << "Found! ";
			else
				std::cout << "Not found... ";
			std::cout << i << ", " << (char)('a'+i) << std::endl;
	}


	bool found = tree.find( std::make_tuple(3,'a') );
	if( found )
		std::cout << "Found! ";
	else
		std::cout << "Not found... ";
	std::cout << "3,a" << std::endl;

	return 0;
}

