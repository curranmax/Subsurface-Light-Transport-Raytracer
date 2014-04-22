#ifndef _HASH_H_
#define _HASH_H_

//Copied from RPI Graphics hw3

#include <unordered_map>

class Edge;
class Triangle;
#include "vertex.h"

#define LARGE_PRIME_A 10007
#define LARGE_PRIME_B 11003

// ===================================================================================
// DIRECTED EDGES are stored in a hash table using a simple hash
// function based on the indices of the start and end vertices
// ===================================================================================

inline unsigned int ordered_two_int_hash(unsigned int a, unsigned int b) {
	return LARGE_PRIME_A * a + LARGE_PRIME_B * b;
}

struct orderedvertexpairhash {
	size_t operator()(std::pair<Vertex*,Vertex*> p) const {
		return ordered_two_int_hash(p.first->getIndex(),p.second->getIndex());
	}
};

struct orderedsamevertexpair {
	bool operator()(std::pair<Vertex*,Vertex*> p1, std::pair<Vertex*,Vertex*>p2) const {
		if (p1.first->getIndex() == p2.first->getIndex() && p1.second->getIndex() == p2.second->getIndex())
			return true;
		return false;
	}
};

typedef std::unordered_map<std::pair<Vertex*,Vertex*>,Edge*,orderedvertexpairhash,orderedsamevertexpair> edgeshashtype;
#endif
