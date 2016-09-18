#ifndef NODE_H
#define	NODE_H

#include "defs.h"

class Node {
public:
	Node();
	Node(double _posx, double _posy, int _links[4], int _linkTypes[4], int _velocity[4], int _capacity[4]);
	~Node();
	
	double posx, posy;
	int links[4];
	int linkTypes[4];
    bool linkStatus[4];
    bool infoStatus[4];
    bool congested[4];
    double timeCongested[4];
    int toll[4];
	int velocity[4];
	int capacity[4];
	int numLinks;
    
    int quo[4];
    double lastChange[4];
};

#endif