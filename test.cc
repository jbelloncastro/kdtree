
#include "kdtree.hpp"

#include <iostream>

int main() {
	ads::RelaxedKDTree<std::tuple<int,char> > tree;
	for( int i = 0; i < 10; i++ ) {
			tree.insert( std::make_tuple(i,'a'+i) );
	}
	for( int i = 0; i < 10; i++ ) {
			bool found = tree.find( std::make_tuple(i,'a'+i) );
			if( found )
				std::cout << "Found! " << i << ", " << (char)('a'+i) << std::endl;
	}
	return 0;
}

